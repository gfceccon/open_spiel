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

#include "open_spiel/games/tarot/bid.h"

namespace open_spiel
{
  namespace french_tarot
  {

    Bid BidFromAction(int action_id)
    {
      // Mapping from action IDs to bid names
      static const Bid bids[] = {
          Bid::kPass,
          Bid::kPetit,
          Bid::kGarde,
          Bid::kGardeSans,
          Bid::kGardeContre,
      };

      // Return default value for invalid action IDs
      if (action_id < 0 || action_id >= kNumBids)
      {
        return Bid::kPass;
      }

      return bids[action_id];
    }

    Action BidToAction(Bid bid)
    {
      // Mapping from bid names to action IDs
      static const Action actions[] = {
          0, // Pass
          1, // kPetit
          2, // kGarde
          3, // kGardeSans
          4, // kGardeContre
      };

      return actions[static_cast<int>(bid)];
    }

    int BidToMultiplier(Bid bid)
    {
      return static_cast<int>(bid);
    }

    bool BidCanMulligan(Bid bid)
    {
      return (bid == Bid::kPetit || bid == Bid::kGarde);
    }

    std::string BidToString(Bid bid)
    {
      switch (bid)
      {
      case Bid::kPetit:
        return "Small";
      case Bid::kGarde:
        return "Guard";
      case Bid::kGardeSans:
        return "Guard without dog";
      case Bid::kGardeContre:
        return "Guard against";
      case Bid::kPass:
      default:
        return "Pass";
      }
    }

    std::ostream &operator<<(std::ostream &os, Bid bid)
    {
      os << BidToString(bid);
      return os;
    }

  } // namespace french_tarot
} // namespace open_spiel
