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

#include "open_spiel/games/tarot/bids.h"

namespace open_spiel
{
  namespace french_tarot
  {

    Bid::Bid(BidName name, int multiplier)
        : name(name),
          multiplier(multiplier) {}

    const std::array<Bid, kNumBids> InitializeBids()
    {
      return {
          Bid(BidName::kPass, 0),
          Bid(BidName::kPetit, 1),
          Bid(BidName::kGarde, 2),
          Bid(BidName::kGardeSans, 4),
          Bid(BidName::kGardeContre, 6),
      };
    }

    std::ostream &operator<<(std::ostream &os, const BidName &contract_name)
    {
      os << BidToString(contract_name);
      return os;
    }

    std::string BidToString(const BidName &contract_name)
    {
      switch (contract_name)
      {
      case BidName::kPetit:
        return "Small";
      case BidName::kGarde:
        return "Guard";
      case BidName::kGardeSans:
        return "Guard without dog";
      case BidName::kGardeContre:
        return "Guard against";
      case BidName::kPass:
      default:
        return "Pass";
      }
    }

  } // namespace french_tarot
} // namespace open_spiel
