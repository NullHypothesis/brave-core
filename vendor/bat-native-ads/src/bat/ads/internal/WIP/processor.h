/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

<<<<<<< Updated upstream
#ifndef BAT_ADS_INTERNAL_READER_H_
#define BAT_ADS_INTERNAL_READER_H_
=======
#ifndef BAT_ADS_INTERNAL_PROCESSOR_H_
#define BAT_ADS_INTERNAL_PROCESSOR_H_
>>>>>>> Stashed changes

namespace ads {

class Processor {
 public:
  virtual ~Processor() = default;

  virtual bool Process() = 0;
};

}  // namespace ads

<<<<<<< Updated upstream
#endif  // BAT_ADS_INTERNAL_READER_H_
=======
#endif  // BAT_ADS_INTERNAL_PROCESSOR_H_
>>>>>>> Stashed changes
