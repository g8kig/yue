// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/gfx/win/font_win.h"

#include <algorithm>

#include "base/strings/utf_string_conversions.h"
#include "base/win/scoped_gdi_object.h"
#include "base/win/scoped_hdc.h"
#include "base/win/scoped_select_object.h"
#include "nativeui/gfx/win/scoped_set_map_mode.h"

namespace nu {

FontWin::FontWin(const base::string16& font_name,
                                 int font_size)
    : font_family_(font_name.c_str()),
      font_(&font_family_, font_size,
            Gdiplus::FontStyleRegular, Gdiplus::UnitPixel) {
}

FontWin::~FontWin() {
}

std::string FontWin::GetFontName() const {
  WCHAR buffer[LF_FACESIZE] = {0};
  font_family_.GetFamilyName(buffer);
  return base::UTF16ToUTF8(buffer);
}

int FontWin::GetFontSize() const {
  return font_.GetSize();
}

NativeFont FontWin::GetNativeFont() const {
  return const_cast<NativeFont>(&font_);
}

// static
Font* Font::CreateDefault() {
  NONCLIENTMETRICS metrics = {0};
  metrics.cbSize = sizeof(metrics);
  SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);

  TEXTMETRIC font_metrics;
  base::win::ScopedHFONT font(CreateFontIndirectW(&(metrics.lfMessageFont)));
  base::win::ScopedGetDC screen_dc(NULL);
  ScopedSetMapMode mode(screen_dc, MM_TEXT);
  base::win::ScopedSelectObject scoped_font(screen_dc, font.get());
  ::GetTextMetrics(screen_dc, &font_metrics);
  int font_size =
      std::max<int>(1, font_metrics.tmHeight - font_metrics.tmInternalLeading);
  return new FontWin(metrics.lfMessageFont.lfFaceName, font_size);
}

// static
Font* Font::CreateFromNameAndSize(const std::string& font_name,
                                                  int font_size) {
  return new FontWin(base::UTF8ToUTF16(font_name), font_size);
}

}  // namespace nu