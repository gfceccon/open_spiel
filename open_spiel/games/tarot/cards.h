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
    inline constexpr int kDeckSize = 78;
    inline constexpr int kNumSuits = 4;
    inline constexpr int kNumRanks = 14;
    inline constexpr int kNumTrumps = 22;

    enum CardSuit
    {
      kHearts = 0,
      kDiamonds = 1,
      kSpades = 2,
      kClubs = 3,
      kTrumps = 4
    };

    struct Card
    {
      Card(CardSuit suit, int rank, int points, std::string short_name,
           std::string long_name, bool is_bout, bool is_fool = false);

      std::string ToString();

      int rank;
      int points;
      bool is_bout;
      bool is_fool;
      CardSuit suit;
      std::string short_name;
      std::string long_name;

      std::string Card::ToString() { return long_name; }
    };

    inline constexpr int kOffset = kNumTrumps - 1;
    const std::map<CardSuit, int> kOrderOfSuits = {
        {CardSuit::kTrumps, 0},
        {CardSuit::kHearts, CardSuit::kHearts *kNumRanks + kOffset},
        {CardSuit::kDiamonds, CardSuit::kDiamonds *kNumRanks + kOffset},
        {CardSuit::kSpades, 2 * CardSuit::kSpades *kNumRanks + kOffset},
        {CardSuit::kClubs, 3 * CardSuit::kClubs *kNumRanks + kOffset}};

    std::array<Card, kDeckSize> InitializeCardDeck();

    std::vector<std::vector<Action>> DealCards(int num_players, int seed);

    int CardToAction(const Card &card)
    {
      return kOrderOfSuits.at(card.suit) + card.rank;
    }

    Card CardFromAction(Action action_id)
    {
      return deck[action_id];
    }

    void Shuffle(std::vector<Action> *actions, std::mt19937 &&rng);

    const std::array<Card, kDeckSize> deck = {
        Card(CardSuit::kHearts, 1, 0.5, "H1", "1 of Hearts", false),
        Card(CardSuit::kHearts, 2, 0.5, "H2", "2 of Hearts", false),
        Card(CardSuit::kHearts, 3, 0.5, "H3", "3 of Hearts", false),
        Card(CardSuit::kHearts, 4, 0.5, "H4", "4 of Hearts", false),
        Card(CardSuit::kHearts, 5, 0.5, "H5", "5 of Hearts", false),
        Card(CardSuit::kHearts, 6, 0.5, "H6", "6 of Hearts", false),
        Card(CardSuit::kHearts, 7, 0.5, "H7", "7 of Hearts", false),
        Card(CardSuit::kHearts, 8, 0.5, "H8", "8 of Hearts", false),
        Card(CardSuit::kHearts, 9, 0.5, "H9", "9 of Hearts", false),
        Card(CardSuit::kHearts, 10, 0.5, "H10", "10 of Hearts", false),
        Card(CardSuit::kHearts, 11, 1.5, "H11", "Jack of Hearts", false),
        Card(CardSuit::kHearts, 12, 2.5, "H12", "Knight of Hearts", false),
        Card(CardSuit::kHearts, 13, 3.5, "H13", "Queen of Hearts", false),
        Card(CardSuit::kHearts, 14, 4.5, "H14", "King of Hearts", false),
        Card(CardSuit::kDiamonds, 1, 0.5, "D1", "1 of Diamonds", false),
        Card(CardSuit::kDiamonds, 2, 0.5, "D2", "2 of Diamonds", false),
        Card(CardSuit::kDiamonds, 3, 0.5, "D3", "3 of Diamonds", false),
        Card(CardSuit::kDiamonds, 4, 0.5, "D4", "4 of Diamonds", false),
        Card(CardSuit::kDiamonds, 5, 0.5, "D5", "5 of Diamonds", false),
        Card(CardSuit::kDiamonds, 6, 0.5, "D6", "6 of Diamonds", false),
        Card(CardSuit::kDiamonds, 7, 0.5, "D7", "7 of Diamonds", false),
        Card(CardSuit::kDiamonds, 8, 0.5, "D8", "8 of Diamonds", false),
        Card(CardSuit::kDiamonds, 9, 0.5, "D9", "9 of Diamonds", false),
        Card(CardSuit::kDiamonds, 10, 0.5, "D10", "10 of Diamonds", false),
        Card(CardSuit::kDiamonds, 11, 1.5, "D11", "Jack of Diamonds", false),
        Card(CardSuit::kDiamonds, 12, 2.5, "D12", "Knight of Diamonds", false),
        Card(CardSuit::kDiamonds, 13, 3.5, "D13", "Queen of Diamonds", false),
        Card(CardSuit::kDiamonds, 14, 4.5, "D14", "King of Diamonds", false),
        Card(CardSuit::kSpades, 1, 0.5, "S1", "1 of Spades", false),
        Card(CardSuit::kSpades, 2, 0.5, "S2", "2 of Spades", false),
        Card(CardSuit::kSpades, 3, 0.5, "S3", "3 of Spades", false),
        Card(CardSuit::kSpades, 4, 0.5, "S4", "4 of Spades", false),
        Card(CardSuit::kSpades, 5, 0.5, "S5", "5 of Spades", false),
        Card(CardSuit::kSpades, 6, 0.5, "S6", "6 of Spades", false),
        Card(CardSuit::kSpades, 7, 0.5, "S7", "7 of Spades", false),
        Card(CardSuit::kSpades, 8, 0.5, "S8", "8 of Spades", false),
        Card(CardSuit::kSpades, 9, 0.5, "S9", "9 of Spades", false),
        Card(CardSuit::kSpades, 10, 0.5, "S10", "10 of Spades", false),
        Card(CardSuit::kSpades, 11, 1.5, "S11", "Jack of Spades", false),
        Card(CardSuit::kSpades, 12, 2.5, "S12", "Knight of Spades", false),
        Card(CardSuit::kSpades, 13, 3.5, "S13", "Queen of Spades", false),
        Card(CardSuit::kSpades, 14, 4.5, "S14", "King of Spades", false),
        Card(CardSuit::kClubs, 1, 0.5, "C1", "1 of Clubs", false),
        Card(CardSuit::kClubs, 2, 0.5, "C2", "2 of Clubs", false),
        Card(CardSuit::kClubs, 3, 0.5, "C3", "3 of Clubs", false),
        Card(CardSuit::kClubs, 4, 0.5, "C4", "4 of Clubs", false),
        Card(CardSuit::kClubs, 5, 0.5, "C5", "5 of Clubs", false),
        Card(CardSuit::kClubs, 6, 0.5, "C6", "6 of Clubs", false),
        Card(CardSuit::kClubs, 7, 0.5, "C7", "7 of Clubs", false),
        Card(CardSuit::kClubs, 8, 0.5, "C8", "8 of Clubs", false),
        Card(CardSuit::kClubs, 9, 0.5, "C9", "9 of Clubs", false),
        Card(CardSuit::kClubs, 10, 0.5, "C10", "10 of Clubs", false),
        Card(CardSuit::kClubs, 11, 1.5, "C11", "Jack of Clubs", false),
        Card(CardSuit::kClubs, 12, 2.5, "C12", "Knight of Clubs", false),
        Card(CardSuit::kClubs, 13, 3.5, "C13", "Queen of Clubs", false),
        Card(CardSuit::kClubs, 14, 4.5, "C14", "King of Clubs", false),
        Card(CardSuit::kTrumps, 0, 4.5, "T0", "Le Excuse", true, true),
        Card(CardSuit::kTrumps, 1, 4.5, "T1", "Le Petit", true),
        Card(CardSuit::kTrumps, 2, 0.5, "T2", "2 of Trumps", false),
        Card(CardSuit::kTrumps, 3, 0.5, "T3", "3 of Trumps", false),
        Card(CardSuit::kTrumps, 4, 0.5, "T4", "4 of Trumps", false),
        Card(CardSuit::kTrumps, 5, 0.5, "T5", "5 of Trumps", false),
        Card(CardSuit::kTrumps, 6, 0.5, "T6", "6 of Trumps", false),
        Card(CardSuit::kTrumps, 7, 0.5, "T7", "7 of Trumps", false),
        Card(CardSuit::kTrumps, 8, 0.5, "T8", "8 of Trumps", false),
        Card(CardSuit::kTrumps, 9, 0.5, "T9", "9 of Trumps", false),
        Card(CardSuit::kTrumps, 10, 0.5, "T10", "10 of Trumps", false),
        Card(CardSuit::kTrumps, 11, 0.5, "T11", "11 of Trumps", false),
        Card(CardSuit::kTrumps, 12, 0.5, "T12", "12 of Trumps", false),
        Card(CardSuit::kTrumps, 13, 0.5, "T13", "13 of Trumps", false),
        Card(CardSuit::kTrumps, 14, 0.5, "T14", "14 of Trumps", false),
        Card(CardSuit::kTrumps, 15, 0.5, "T15", "15 of Trumps", false),
        Card(CardSuit::kTrumps, 16, 0.5, "T16", "16 of Trumps", false),
        Card(CardSuit::kTrumps, 17, 0.5, "T17", "17 of Trumps", false),
        Card(CardSuit::kTrumps, 18, 0.5, "T18", "18 of Trumps", false),
        Card(CardSuit::kTrumps, 19, 0.5, "T19", "19 of Trumps", false),
        Card(CardSuit::kTrumps, 20, 0.5, "T20", "20 of Trumps", false),
        Card(CardSuit::kTrumps, 21, 4.5, "T21", "Le Monde", true),
    };
  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_CARDS_H_
