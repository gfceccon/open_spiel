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

#ifndef OPEN_SPIEL_GAMES_FRENCH_TAROT_CARDS_H_
#define OPEN_SPIEL_GAMES_FRENCH_TAROT_CARDS_H_

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "open_spiel/spiel.h"

namespace open_spiel
{
  namespace french_tarot
  {

    enum class CardSuit
    {
      kHearts,
      kDiamonds,
      kSpades,
      kClubs,
      kTrumps
    };
    inline constexpr int kDeckSize = 78;
    inline constexpr int kNumSuits = 4;
    inline constexpr int kNumRanks = 14;
    inline constexpr int kNumTrumps = 22;
    inline constexpr int kDogSize = 6;            // Le chien
    inline constexpr int kDealCardsPerPlayer = 3; // Number of cards dealt to each player
    inline const std::map<CardSuit, int> kOrderOfSuits = {
        {CardSuit::kHearts, 0},
        {CardSuit::kDiamonds, kNumTrumps - 1},
        {CardSuit::kSpades, kNumTrumps + kNumRanks - 1},
        {CardSuit::kClubs, kNumTrumps + kNumRanks * 2 - 1},
        {CardSuit::kTrumps, kNumTrumps + kNumRanks * 3 - 1}};

    struct Card
    {
      Card(CardSuit suit, int rank, int points, std::string short_name,
           std::string long_name, bool is_bout, bool is_fool = false);

      const std::string ToString() const;

      int rank;
      int points;
      bool is_bout;
      bool is_fool;
      CardSuit suit;
      std::string short_name;
      std::string long_name;
    };

    const std::array<Card, kDeckSize> InitializeCardDeck();

    // a type for a pair holding talon and players' private cards
    using DealtCards =
        std::tuple<std::vector<Action>, std::vector<std::vector<Action>>>;
    DealtCards DealCards(int num_players, int seed);

    void Shuffle(std::vector<Action> *actions, std::mt19937 &&rng);

    int CardPoints(const std::vector<Action> &actions,
                   const std::array<Card, kDeckSize> &deck);

  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_CARDS_H_
