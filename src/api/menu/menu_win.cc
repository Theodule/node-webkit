// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/api/menu/menu.h"

#include "base/values.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/browser/native_window_win.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "ui/views/controls/menu/menu_2.h"
#include "ui/views/widget/widget.h"

namespace api {

void Menu::Create(const base::DictionaryValue& option) {
  is_menu_modified_ = true;
  menu_delegate_.reset(new MenuDelegate(dispatcher_host()));
  menu_model_.reset(new ui::SimpleMenuModel(menu_delegate_.get()));
  menu_.reset(new views::NativeMenuWin(menu_model_.get(), NULL));

  std::string type;
  if (option.GetString("type", &type) && type == "menubar")
    menu_->set_is_popup_menu(false);
}

void Menu::Destroy() {
}

void Menu::Append(MenuItem* menu_item) {
  if (menu_item->submenu_)
    menu_model_->AddSubMenu(menu_item->id(), menu_item->label_,
                            menu_item->submenu_->menu_model_.get());
  else if (menu_item->type_ == "normal")
    menu_model_->AddItem(menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "checkbox")
    menu_model_->AddCheckItem(menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "separator")
    menu_model_->AddSeparator(ui::NORMAL_SEPARATOR);

  is_menu_modified_ = true;
}

void Menu::Insert(MenuItem* menu_item, int pos) {
  if (menu_item->submenu_)
    menu_model_->InsertSubMenuAt(pos, menu_item->id(), menu_item->label_,
                                 menu_item->submenu_->menu_model_.get());
  else if (menu_item->type_ == "normal")
    menu_model_->InsertItemAt(pos, menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "checkbox")
    menu_model_->InsertCheckItemAt(pos, menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "separator")
    menu_model_->InsertSeparatorAt(pos, ui::NORMAL_SEPARATOR);

  is_menu_modified_ = true;
}

void Menu::Remove(MenuItem* menu_item, int pos) {
  menu_model_->RemoveAt(pos);
  is_menu_modified_ = true;
}

void Menu::Popup(int x, int y, content::Shell* shell) {
  Rebuild();

  // Map point from document to screen.
  POINT screen_point = { x, y };
  ClientToScreen(shell->web_contents()->GetView()->GetNativeView(),
                 &screen_point);

  menu_->RunMenuAt(gfx::Point(screen_point.x, screen_point.y),
                   views::Menu2::ALIGN_TOPLEFT);
}

void Menu::Rebuild() {
  if (is_menu_modified_) {
    // Refresh menu before show.
    menu_->Rebuild();
    is_menu_modified_ = false;
  }
}

}  // namespace api
