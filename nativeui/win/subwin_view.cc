// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/win/subwin_view.h"

#include "base/win/scoped_hdc.h"
#include "nativeui/gfx/font.h"
#include "nativeui/state.h"

namespace nu {

SubwinView::SubwinView(base::StringPiece16 class_name,
                       DWORD window_style, DWORD window_ex_style)
    : WindowImpl(class_name, State::current()->GetSubwinHolder(),
                 window_style, window_ex_style),
      BaseView(ControlType::Subwin) {
  // Create HFONT from default system font.
  base::win::ScopedCreateDC mem_dc(CreateCompatibleDC(NULL));
  Gdiplus::Graphics context(mem_dc.Get());
  LOGFONTW logfont;
  Font().GetNativeFont()->GetLogFontW(&context, &logfont);
  font_.reset(CreateFontIndirect(&logfont));
  // Use it as control's default font.
  SendMessage(hwnd(), WM_SETFONT, reinterpret_cast<WPARAM>(font_.get()), TRUE);
}

SubwinView::~SubwinView() {
}

void SubwinView::SizeAllocate(const Rect& size_allocation) {
  BaseView::SizeAllocate(size_allocation);

  SetWindowPos(hwnd(), NULL,
               size_allocation.x(), size_allocation.y(),
               size_allocation.width(), size_allocation.height(),
               SWP_NOACTIVATE | SWP_NOZORDER);
  RedrawWindow(hwnd(), NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

void SubwinView::SetParent(BaseView* parent) {
  BaseView::SetParent(parent);
  ::SetParent(hwnd(),
              parent && parent->window() ? parent->window()->hwnd()
                                         : State::current()->GetSubwinHolder());
}

void SubwinView::BecomeContentView(WindowImpl* parent) {
  BaseView::BecomeContentView(parent);
  ::SetParent(hwnd(), parent ? parent->hwnd()
                             : State::current()->GetSubwinHolder());
}

void SubwinView::Invalidate(const Rect& dirty) {
  // There is no need to invalidate a child window.
}

void SubwinView::SetFocus(bool focus) {
  ::SetFocus(focus ? hwnd() : ::GetParent(hwnd()));
  if (!focus)
    ::SendMessage(hwnd(), WM_KILLFOCUS, NULL, NULL);
}

bool SubwinView::IsFocused() const {
  return ::GetFocus() == hwnd();
}

}  // namespace nu
