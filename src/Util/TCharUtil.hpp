/*
 * Copyright (C) 2011-2015 Max Kellermann <max@duempel.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XCSOAR_TCHAR_UTIL_HPP
#define XCSOAR_TCHAR_UTIL_HPP

#ifndef _UNICODE
#error Cannot use this header without _UNICODE
#endif

#include <tchar.h>

constexpr
static inline bool
IsASCII(const TCHAR ch)
{
  return (ch & ~0x7f) == 0;
}

static inline bool
IsWhitespaceOrNull(const TCHAR ch)
{
  return (unsigned)ch <= 0x20;
}

static inline bool
IsWhitespaceNotNull(const TCHAR ch)
{
  return ch > 0 && ch <= 0x20;
}

constexpr
static inline bool
IsPrintableASCII(TCHAR ch)
{
  return IsASCII(ch) && ch >= 0x20;
}

constexpr
static inline bool
IsDigitASCII(TCHAR ch)
{
  return ch >= _T('0') && ch <= _T('9');
}

constexpr
static inline bool
IsUpperAlphaASCII(TCHAR ch)
{
  return ch >= _T('A') && ch <= _T('Z');
}

constexpr
static inline bool
IsLowerAlphaASCII(TCHAR ch)
{
  return ch >= _T('a') && ch <= _T('z');
}

constexpr
static inline bool
IsAlphaASCII(TCHAR ch)
{
  return IsUpperAlphaASCII(ch) || IsLowerAlphaASCII(ch);
}

constexpr
static inline bool
IsAlphaNumericASCII(TCHAR ch)
{
  return IsAlphaASCII(ch) || IsDigitASCII(ch);
}

#endif
