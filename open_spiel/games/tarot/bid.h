// Copyright 2019 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef OPEN_SPIEL_GAMES_FRENCH_TAROT_BIDS_H_
#define OPEN_SPIEL_GAMES_FRENCH_TAROT_BIDS_H_

#include <iostream>
#include <string>
#include <vector>

#include "open_spiel/spiel.h"

namespace open_spiel
{
  namespace french_tarot
  {
    inline constexpr int kInvalidBidAction = -1;
    inline constexpr int kNumBids = 5;

    enum Bid
    {
      kPass = 0,       // Pass
      kPetit = 1,      // Small
      kGarde = 2,      // Guard
      kGardeSans = 4,  // Guard without dog
      kGardeContre = 6 // Guard against
    };

    Bid BidFromAction(int action_id);

    int BidToMultiplier(Bid bid);

    bool BidCanMulligan(Bid bid);

    std::string BidToString(Bid bid);
    std::string BidActionToString(int action_id);

    std::ostream &operator<<(std::ostream &os, Bid bid);

  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_BIDS_H_
