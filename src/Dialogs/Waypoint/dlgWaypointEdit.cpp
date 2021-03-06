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

#include "WaypointDialogs.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "Dialogs/Message.hpp"
#include "Widget/RowFormWidget.hpp"
#include "Form/DataField/Enum.hpp"
#include "Form/DataField/GeoPoint.hpp"
#include "UIGlobals.hpp"
#include "Waypoint/Waypoint.hpp"
#include "Interface.hpp"
#include "Language/Language.hpp"

class WaypointEditWidget final : public RowFormWidget {
  enum Rows {
    NAME,
    COMMENT,
    LOCATION,
    ELEVATION,
    TYPE,
  };

  Waypoint value;

public:
  WaypointEditWidget(const DialogLook &look, Waypoint _value)
    :RowFormWidget(look), value(_value) {}

  const Waypoint &GetValue() const {
    return value;
  }

private:
  /* virtual methods from Widget */
  virtual void Prepare(ContainerWindow &parent, const PixelRect &rc) override;
  virtual bool Save(bool &changed) override;
};

static constexpr StaticEnumChoice waypoint_types[] = {
  { 0, N_("Turnpoint"), nullptr },
  { 1, N_("Airport"), nullptr },
  { 2, N_("Landable"), nullptr },
  { 0 }
};

void
WaypointEditWidget::Prepare(ContainerWindow &parent, const PixelRect &rc)
{
  AddText(_("Name"), nullptr, value.name.c_str());
  AddText(_("Comment"), nullptr, value.comment.c_str());
  Add(_("Location"), nullptr, new GeoPointDataField(value.location,UIGlobals::GetFormatSettings().coordinate_format));
  AddFloat(_("Altitude"), nullptr,
           _T("%.0f %s"), _T("%.0f"),
           fixed(0), fixed(30000), fixed(5), false,
           UnitGroup::ALTITUDE, value.elevation);
  AddEnum(_("Type"), nullptr, waypoint_types,
          value.IsAirport() ? 1u : (value.IsLandable() ? 2u : 0u ));
}

bool
WaypointEditWidget::Save(bool &_changed)
{
  bool changed = false;
  value.name = GetValueString(NAME);
  value.comment = GetValueString(COMMENT);
  value.location = ((GeoPointDataField &)GetDataField(LOCATION)).GetValue();
  changed |= SaveValue(ELEVATION, UnitGroup::ALTITUDE, value.elevation);
  _changed |= changed;

  switch (GetValueInteger(TYPE)) {
  case 1:
    value.flags.turn_point = true;
    value.type = Waypoint::Type::AIRFIELD;
    break;

  case 2:
    value.type = Waypoint::Type::OUTLANDING;
    break;

  default:
    value.type = Waypoint::Type::NORMAL;
    value.flags.turn_point = true;
    break;
  };

  return true;
}

bool
dlgWaypointEditShowModal(Waypoint &way_point)
{
  if (UIGlobals::GetFormatSettings().coordinate_format ==
      CoordinateFormat::UTM) {
    ShowMessageBox(
        _("Sorry, the waypoint editor is not yet available for the UTM coordinate format."),
        _("Waypoint Editor"), MB_OK);
    return false;
  }

  const DialogLook &look = UIGlobals::GetDialogLook();
  WidgetDialog dialog(look);
  WaypointEditWidget widget(look, way_point);
  dialog.CreateAuto(UIGlobals::GetMainWindow(), _("Waypoint Editor"), &widget);
  dialog.AddButton(_("OK"), mrOK);
  dialog.AddButton(_("Cancel"), mrCancel);
  const int result = dialog.ShowModal();
  dialog.StealWidget();

  if (result != mrOK)
    return false;

  way_point = widget.GetValue();
  return true;
}
