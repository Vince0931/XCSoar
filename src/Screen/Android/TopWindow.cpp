/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Screen/TopWindow.hpp"
#include "Screen/Custom/Cache.hpp"
#include "Screen/OpenGL/Surface.hpp"
#include "Screen/OpenGL/Shapes.hpp"
#include "Screen/Custom/TopCanvas.hpp"
#include "Event/Queue.hpp"
#include "Event/Android/Loop.hpp"
#include "Event/Globals.hpp"
#include "Android/Main.hpp"
#include "Android/NativeView.hpp"

void
TopWindow::Invalidate()
{
  invalidated = true;
}

void
TopWindow::AnnounceResize(UPixelScalar width, UPixelScalar height)
{
  ScopeLock protect(paused_mutex);
  resized = true;
  new_width = width;
  new_height = height;
}

bool
TopWindow::ResumeSurface()
{
  /* Try to reinitialize OpenGL.  This often fails on the first
     attempt (IllegalArgumentException "Make sure the SurfaceView or
     associated SurfaceHolder has a valid Surface"), therefore we're
     trying again until we're successful. */

  assert(paused);

  if (!native_view->initSurface())
    /* failed - retry later */
    return false;

  paused = false;
  resumed = false;

  screen->Resume();

  ::SurfaceCreated();

  RefreshSize();

  return true;
}

bool
TopWindow::CheckResumeSurface()
{
  return (!resumed || ResumeSurface()) && !paused && surface_valid;
}

void
TopWindow::RefreshSize()
{
  UPixelScalar width, height;

  {
    ScopeLock protect(paused_mutex);
    if (!resized)
      return;

    resized = false;
    width = new_width;
    height = new_height;
  }

  Resize(width, height);
}

void
TopWindow::OnResize(PixelSize new_size)
{
  if (native_view != nullptr) {
    native_view->SetSize(new_size.cx, new_size.cy);
    screen->OnResize(new_size);
  }

  ContainerWindow::OnResize(new_size);
}

void
TopWindow::OnPause()
{
  if (paused)
    return;

  TextCache::Flush();
  OpenGL::DeinitShapes();

  SurfaceDestroyed();

  native_view->deinitSurface();

  paused_mutex.Lock();
  paused = true;
  resumed = false;
  paused_cond.Signal();
  paused_mutex.Unlock();
}

void
TopWindow::OnResume()
{
  if (!paused)
    return;

  /* tell TopWindow::Expose() to reinitialize OpenGL */
  resumed = true;

  /* schedule a redraw */
  Invalidate();
}

static bool
match_pause_and_resume(const Event &event, void *ctx)
{
  return event.type == Event::PAUSE || event.type == Event::RESUME;
}

void
TopWindow::Pause()
{
  event_queue->Purge(match_pause_and_resume, nullptr);
  event_queue->Push(Event::PAUSE);

  paused_mutex.Lock();
  while (!paused)
    paused_cond.Wait(paused_mutex);
  paused_mutex.Unlock();
}

void
TopWindow::Resume()
{
  event_queue->Purge(match_pause_and_resume, nullptr);
  event_queue->Push(Event::RESUME);
}

bool
TopWindow::OnEvent(const Event &event)
{
  switch (event.type) {
    Window *w;

  case Event::NOP:
  case Event::QUIT:
  case Event::TIMER:
  case Event::USER:
  case Event::CALLBACK:
    break;

  case Event::KEY_DOWN:
    w = GetFocusedWindow();
    if (w == nullptr)
      w = this;

    return w->OnKeyDown(event.param);

  case Event::KEY_UP:
    w = GetFocusedWindow();
    if (w == nullptr)
      w = this;

    return w->OnKeyUp(event.param);

  case Event::MOUSE_MOTION:
    // XXX keys
    return OnMouseMove(event.point.x, event.point.y, 0);

  case Event::MOUSE_DOWN:
    return double_click.Check(event.point)
      ? OnMouseDouble(event.point.x, event.point.y)
      : OnMouseDown(event.point.x, event.point.y);

  case Event::MOUSE_UP:
    double_click.Moved(event.point);

    return OnMouseUp(event.point.x, event.point.y);

  case Event::MOUSE_WHEEL:
    return OnMouseWheel(event.point.x, event.point.y, (int)event.param);

  case Event::POINTER_DOWN:
    return OnMultiTouchDown();

  case Event::POINTER_UP:
    return OnMultiTouchUp();

  case Event::RESIZE:
    if (!surface_valid)
      /* postpone the resize if we're paused; the real resize will be
         handled by TopWindow::refresh() as soon as XCSoar is
         resumed */
      return true;

    if (unsigned(event.point.x) == GetWidth() &&
        unsigned(event.point.y) == GetHeight())
      /* no-op */
      return true;

    /* it seems the first page flip after a display orientation change
       is ignored on Android (tested on a Dell Streak / Android
       2.2.2); let's do one dummy call before we really draw
       something */
    screen->Flip();

    Resize(event.point.x, event.point.y);
    return true;

  case Event::PAUSE:
    OnPause();
    return true;

  case Event::RESUME:
    OnResume();
    return true;
  }

  return false;
}

int
TopWindow::RunEventLoop()
{
  Refresh();

  EventLoop loop(*event_queue, *this);
  Event event;
  while (IsDefined() && loop.Get(event))
    loop.Dispatch(event);

  return 0;
}

void
TopWindow::PostQuit()
{
  event_queue->Push(Event::QUIT);
}
