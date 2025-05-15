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

#ifndef OPEN_SPIEL_GAMES_FRENCH_TAROT_BID_H_
#define OPEN_SPIEL_GAMES_FRENCH_TAROT_BID_H_

#include <array>
#include <iostream>
#include <string>

#include "open_spiel/spiel.h"

namespace open_spiel {
namespace french_tarot {

  
inline constexpr int kInvalidBidAction = -1;
inline constexpr int kPass = 0;
inline constexpr int kPetit = 1;
inline constexpr int kGarde = 2;
inline constexpr int kGardeSans = 5;
inline constexpr int kGardeContre = 7;
inline constexpr int kNumBids = 5;

enum class BidName {
  kPass,
  kPetit,
  kGarde,
  kGardeSans,
  kGardeContre,
};

struct Bid {
  Bid(BidName name, int multiplier);

  bool NeedsTalonExchange() const;

  const BidName name;
  const int multiplier;
};

const std::array<Bid, kNumBids> InitializeBids();

std::ostream& operator<<(std::ostream& os, const BidName& contract_name);

std::string BidToString(const BidName& contract_name);

}  // namespace french_tarot
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_FRENCH_TAROT_BID_H_
