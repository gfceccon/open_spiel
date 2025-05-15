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

#ifndef OPEN_SPIEL_GAMES_FRENCH_TAROT_SCORE_H_
#define OPEN_SPIEL_GAMES_FRENCH_TAROT_SCORE_H_

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "open_spiel/spiel.h"
#include "open_spiel/games/tarot/cards.h"
#include "open_spiel/games/tarot/bids.h"

inline constexpr std::array<int, 4>
    pointsNeededPerBouts = {56, 51, 41, 36};

namespace open_spiel
{
  namespace french_tarot
  {
    class Score
    {
    public:
      Score(int points, Bid bid, int taker, std::vector<int> team, bool slamCall, bool handfullCall);

      const Bid bid = Bid(BidName::kPass, 0);
      const int taker = -1;
      const std::vector<int> team;
      const bool slamCall = false;
      const bool handfullCall = false;

      const std::string ToString() const;

      int PartialPoints(
          const bool taker,
          const std::vector<Action> &actions,
          const std::array<Card, kDeckSize> &deck);

      int FinalPoints(
          const bool taker,
          const std::vector<Action> &actions,
          const std::array<Card, kDeckSize> &deck);
    };
  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_CARDS_H_
