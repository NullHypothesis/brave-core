/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_blank_page_ui.h"

#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "chrome/browser/profiles/profile.h"
#include "components/grit/brave_components_resources.h"
#include "content/public/browser/web_ui_data_source.h"

BraveBlankPageUI::BraveBlankPageUI(content::WebUI* web_ui,
                                   const std::string& host)
    : content::WebUIController(web_ui) {
  const bool is_dark_mode =
      dark_mode::GetActiveBraveDarkModeType() ==
      dark_mode::BraveDarkModeType::BRAVE_DARK_MODE_TYPE_DARK;

  Profile* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource* source =
      content::WebUIDataSource::Create(host);
  source->SetDefaultResource(is_dark_mode ? IDR_BRAVE_DARK_BLANK_PAGE_HTML
                                          : IDR_BRAVE_BLANK_PAGE_HTML);
  content::WebUIDataSource::Add(profile, source);
}

BraveBlankPageUI::~BraveBlankPageUI() = default;
