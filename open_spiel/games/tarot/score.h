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
#include "open_spiel/games/tarot/bid.h"

// Pontos necessários para ganhar baseado no número de bouts (0-3)
inline constexpr std::array<int, 4> pointsNeededPerBouts = {56, 51, 41, 36};

namespace open_spiel
{
  namespace french_tarot
  {
    int ScorePointsNeeded(std::vector<Action> hand);

    bool ScoreBidSuccess(std::vector<Action> hand, Bid bid);

    double ScoreEstimated(std::vector<Action> hand, Bid bid);

    double ScorePartialScore(std::vector<Action> hand, Bid bid);

    double ScoreFinalScore(std::vector<Action> hand, Bid bid, bool slam_success, bool poingee_success);
    
  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_SCORE_H_
