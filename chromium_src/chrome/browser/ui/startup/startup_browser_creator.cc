/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/tor/tor_profile_manager.h"
#include "brave/browser/tor/tor_profile_service_factory.h"
#include "brave/components/tor/tor_profile_service.h"
#include "brave/common/brave_switches.h"

#define GetPrivateProfileIfRequested GetPrivateProfileIfRequested_ChromiumImpl

#include "../../../../../../chrome/browser/ui/startup/startup_browser_creator.cc"  // NOLINT

#undef GetPrivateProfileIfRequested

Profile* StartupBrowserCreator::GetPrivateProfileIfRequested(
    const base::CommandLine& command_line,
    Profile* profile) {
  if command_line.HasSwitch(switches::kTor) {
    LOG(INFO) << "Selecting Tor profile";
    profile = TorProfileManager::GetInstance().GetTorProfile(profile);
    tor::TorProfileService* service =
        TorProfileServiceFactory::GetForContext(profile);
    service->RegisterTorClientUpdater();
    return profile;
  }
  return GetPrivateProfileIfRequested_ChromiumImpl(command_line, profile);
}

/*
#define BRAVE_STARTUPBROWSERCREATOR_ADDTORPROFILE \
  if (command_line.HasSwitch(switches::kTor)) {   \
      LOG(INFO) << "Selecting Tor profile";       \
      profile = TorProfileManager::GetInstance().GetTorProfile(profile); \
      tor::TorProfileService* service =     \
          TorProfileServiceFactory::GetForContext(profile); \
      service->RegisterTorClientUpdater(); \
  };


#undef BRAVE_STARTUPBROWSERCREATOR_ADDTORPROFILE
*/
