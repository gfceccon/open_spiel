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

#include <algorithm>
#include <random>
#include <utility>

#include "open_spiel/games/tarot/score.h"
#include "open_spiel/games/tarot/cards.h"

namespace open_spiel
{
  namespace french_tarot
  {
    class Score
    {
    public:
      Score(Bid bid, int taker, std::vector<int> team, bool slamCall, bool handfullCall):
          bid(bid),
          taker(taker),
          team(team),
          slamCall(slamCall),
          handfullCall(handfullCall) {}
      const std::string ToString() const;

      int PartialPoints(
          const bool taker,
          const std::vector<Action> &actions,
          const std::array<Card, kDeckSize> &deck)
      {
        float points = 0;
        int bouts = 0;
        for (auto const &action : actions)
        {
          auto card = deck.at(action);
          points += card.points;
          if (card.is_bout)
            bouts++;
        }

        int minimum = (points > pointsNeededPerBouts.at(bouts)) ? 1 : -1;

        return static_cast<int>(minimum * round(points));
      }

      int FinalPoints(
          const bool taker,
          const std::vector<Action> &actions,
          const std::array<Card, kDeckSize> &deck)
      {
        float points = 0;
        int handfull = 0;
        int bouts = 0;
        for (auto const &action : actions)
        {
          auto card = deck.at(action);
          points += card.points;
          if (card.suit == CardSuit::kTrumps)
            handfull++;
          if (card.is_bout)
            bouts++;
        }

        return static_cast<int>(round(points));
      }

    private:
      Bid bid;
      int taker;
      std::vector<int> team;
      bool slamCall;
      bool handfullCall;
    };

  } // namespace french_tarot
} // namespace open_spiel
