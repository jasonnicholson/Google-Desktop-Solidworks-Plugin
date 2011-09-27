// Copyright 2005-2006 Google Inc. All Rights Reserved.
//
// utils.h : Common utility functions.
//

#ifndef COMMON_UTILS_H__
#define COMMON_UTILS_H__

inline int GetFontHeightInPixels(HDC dc, HFONT font) {
  HGDIOBJ prev_font;
  ATLVERIFY(prev_font = ::SelectObject(dc, font));
  TEXTMETRIC tm;
  ATLVERIFY(::GetTextMetrics(dc, &tm));
  POINT font_size = { 0, tm.tmHeight };
  ATLVERIFY(::LPtoDP(dc, &font_size, 1));
  ::SelectObject(dc, prev_font);
  return font_size.y;
}

// Load the given image resource and return a picture object
HRESULT LoadPictureFromResource(HMODULE module, const TCHAR *res_name,
                                const TCHAR *res_type, OUT IPicture **pict);

void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);

#endif  // COMMON_UTILS_H__
