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
#include <cmath>

#include "open_spiel/games/tarot/score.h"
#include "open_spiel/games/tarot/cards.h"
#include "open_spiel/games/tarot/bid.h"
#include "score.h"

namespace open_spiel
{
  namespace french_tarot
  {
    int ScoreCountBouts(std::vector<Action> hand)
    {
      int bouts = 0;
      for (int i = 0; i < hand.size(); i++)
        if (CardFromAction(hand[i]).is_bout)
          bouts++;

      return bouts;
    }

    int ScorePointsNeeded(std::vector<Action> hand)
    {
      int bouts = ScoreCountBouts(hand);
      return pointsNeededPerBouts[bouts];
    }

    int ScoreCountTrumps(std::vector<Action> hand)
    {
      int trumps = 0;
      for (int i = 0; i < hand.size(); i++)
        if (CardFromAction(hand[i]).suit == kTrumps)
          trumps++;

      return trumps;
    }

    bool ScoreBidSuccess(std::vector<Action> hand, Bid bid)
    {
      int points_needed = ScorePointsNeeded(hand);
      int points = ScorePartialScore(hand, bid);
      return points >= points_needed;
    }

    double ScoreEstimated(std::vector<std::vector<Action>> tricks, Bid bid)
    {
      int points = 0;
      for (int i = 0; i < tricks.size(); i++)
      {
        auto trick = tricks[i];
        for (int j = 0; j < trick.size(); j++)
        {
          points += ScorePartialScore(trick, bid);
        }
      }

      return points;
    }

    double ScorePartialScore(std::vector<Action> hand)
    {
      int points = 0;
      for (int i = 0; i < hand.size(); i++)
      {
        Card card = CardFromAction(hand[i]);
        points += card.points;
      }

      return points;
    }

    double ScoreFinalScore(std::vector<Action> hand, Bid bid, bool slam_called, int poingee_called)
    {
      int points = ScorePartialScore(hand, bid);
      int points_needed = ScorePointsNeeded(hand);
      bool bid_successful = ScoreBidSuccess(hand, bid);

      int multiplier = BidToMultiplier(bid);
      double base_score = std::abs(points - points_needed);

      int slam = 0;
      if (hand.size() == kDeckSize)
      {
        if (slam_called)
          slam += 400;
        else
          slam += 200;
      }
      else if (slam_called)
      {
        slam = -200;
      }

      int poingee = 0;
      if (poingee_called)
      {
        int trumps = ScoreCountTrumps(hand);
        if (trumps >= 15 && poingee_called == 15)
          poingee = 40; // Bônus por poingee
        else if (trumps >= 13 && poingee_called == 13)
          poingee = 30; // Bônus por poingee
        else if (trumps >= 10 && poingee_called == 10)
          poingee = 20; // Bônus por poingee
      }

      int petit_au_bout = 0;
      int last_cards_to_check = std::min(4, static_cast<int>(hand.size()));
      for (int i = 0; i < last_cards_to_check; i++)
      {
        int card_index = hand.size() - 1 - i;
        Card card = CardFromAction(hand[card_index]);
        if (card.rank == 1 && card.suit == kTrumps)
        {
          petit_au_bout = 10;
          break;
        }
      }

      // Aplicar multiplicador e sinal
      return (bid_successful ? 1.0 : -1.0) * (base_score * multiplier + poingee + slam + petit_au_bout);
    }

  } // namespace french_tarot
}
// namespace open_spiel
