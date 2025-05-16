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

#include "open_spiel/games/tarot/cards.h"

#include <algorithm>
#include <random>
#include <utility>
#include <numeric>

namespace open_spiel
{
  namespace french_tarot
  {
    Card::Card(CardSuit suit, int rank, int points, std::string short_name,
               std::string long_name, bool is_bout, bool is_fool)
        : suit(suit),
          rank(rank),
          points(points),
          short_name(short_name),
          long_name(long_name),
          is_bout(is_bout),
          is_fool(is_fool) {}

    std::tuple<std::vector<Action>, std::vector<std::vector<Action>>> DealCards(int num_players, int cards_per_player, int dog_size, int seed)
    {
      // deck_ agora é um vetor de índices (Action), não de Card
      auto deck_ = std::vector<Action>(deck.size());
      std::iota(deck_.begin(), deck_.end(), 0); // Preenche com 0, 1, ..., deck.size()-1
      Shuffle(&deck_, std::mt19937(seed));

      std::vector<std::vector<Action>> players_cards(num_players); // +1 para o dog (será o último elemento)
      std::vector<Action> dog(dog_size);                           // cartas do dog
      // Distribuição tradicional: 3 cartas por vez para cada jogador
      int cards_dealt = 0;
      int cards_per_round = 3;   // padrão de distribuição é 3 cartas por vez
      int dog_idx = num_players; // índice do dog (último jogador)

      // Distribui as cartas para os jogadores
      while (cards_dealt < num_players * cards_per_player)
      {
        for (int player = 0; player < num_players && cards_dealt < num_players * cards_per_player; ++player)
        {
          int cards_to_deal = std::min(cards_per_round, cards_per_player - static_cast<int>(players_cards[player].size()));
          for (int i = 0; i < cards_to_deal; ++i)
            players_cards[player].push_back(deck_[cards_dealt++]);
          dog.push_back(deck_[cards_dealt++]);
        }
      }

      return {dog, players_cards};
    }

    void Shuffle(std::vector<Action> *deck, std::mt19937 &&rng)
    {
      for (int i = deck->size() - 1; i > 0; i--)
      {
        std::swap(deck->at(i), deck->at(rng() % (i + 1)));
      }
    }

  } // namespace french_tarot
} // namespace open_spiel
