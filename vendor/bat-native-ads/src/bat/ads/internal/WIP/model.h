/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_AD_EVENTS_AD_EVENT_H_
#define BAT_ADS_INTERNAL_AD_EVENTS_AD_EVENT_H_

namespace ads {

template <class T>
class AdEvent {
 public:
  virtual ~AdEvent() = default;

  virtual void FireEvent(
      const T&) = 0;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_AD_EVENTS_AD_EVENT_H_


ad_serving/ad_delivery
ad_serving/ad_pacing
ad_serving/eligible_ads

ad_serving/ad_targeting/processors:
purchase intent behavioral processor (write) < when: page load - what: url
page classification contextual processor (write) < when: page load - what: content
epsilon greedy bandit behavioral processor (write) < when: ad event - what: ad
ucb bandit behavioral processor (write) < when: ad event - what: ad
gradiant bandit behavioral processor (write) < when: ad event - what: ad
pre-search behavioral processor (write) < when: NOP - what: NOP

// PurchaseIntentBehavioralProcessor processor(user_model);
// processor.Process(url);

ad_serving/ad_targeting/models:
purchase intent behavioral model (read)
page classification contextual model (read)
epsilon greedy bandit behavioral model (read)
ucb bandit behavioral model (read)
gradiant bandit behavioral model (read)
pre-search behavioral model (read)

// PurchaseIntentBehavioralModel model;
// segments = model.GetSegments();
