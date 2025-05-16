// Copyright 2019 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <cmath>
#include <ctime>

#include "open_spiel/abseil-cpp/absl/strings/str_split.h"
#include "open_spiel/abseil-cpp/absl/strings/str_cat.h"
#include "open_spiel/spiel.h"
#include "open_spiel/games/tarot/tarot.h"
#include "open_spiel/games/tarot/score.h"

namespace open_spiel
{
  namespace french_tarot
  {

    const GameType kGameType{"french_tarot",               // short_name
                             "French Tarot for 4 Players", // long_name
                             GameType::Dynamics::kSequential,
                             GameType::ChanceMode::kExplicitStochastic,
                             GameType::Information::kImperfectInformation,
                             GameType::Utility::kZeroSum,
                             GameType::RewardModel::kTerminal,
                             kNumPlayers, // max_num_players
                             kNumPlayers, // min_num_players
                             true,        // provides_information_state_string
                             false,       // provides_information_state_tensor
                             false,       // provides_observation_string
                             false,       // provides_observation_tensor
                             {{"players", GameParameter(kDefaultNumPLayers)},
                              {"dog_size", GameParameter(kDefaultDogSize)},
                              {"rng_seed", GameParameter(kDefaultSeed)}}}; // parameter_specification};

    std::shared_ptr<const Game> Factory(const GameParameters &params)
    {
      return std::shared_ptr<const Game>(new FrenchTarotGame(params));
    }

    REGISTER_SPIEL_GAME(kGameType, Factory);

    RegisterSingleTensorObserver single_tensor(kGameType.short_name);

    FrenchTarotGame::FrenchTarotGame(const GameParameters &params)
        : Game(kGameType, params),
          num_players_(ParameterValue<int>("players")),
          dog_size_(ParameterValue<int>("dog_size")),
          rng_(std::mt19937(
              ParameterValue<int>("rng_seed") == -1
                  ? std::time(0)
                  : ParameterValue<int>("rng_seed")))
    {
      if (num_players_ != 4)
        SpielFatalError(absl::StrCat("Current game implementation only supports 4 players "));

      if (dog_size_ != 6)
        SpielFatalError(absl::StrCat("Current game implementation only supports 4 players "));
    }

    std::unique_ptr<State> FrenchTarotGame::NewInitialState() const
    {
      return NewInitialTarotState();
    }

    std::unique_ptr<TarotState> FrenchTarotGame::NewInitialTarotState() const
    {
      return std::make_unique<TarotState>(shared_from_this());
    }

    TarotState::TarotState(std::shared_ptr<const Game> game)
        : State(game),
          tarot_parent_game_(std::static_pointer_cast<const FrenchTarotGame>(game))
    {
      players_bids_.reserve(tarot_parent_game_->num_players_);
      players_bids_.insert(players_bids_.end(), tarot_parent_game_->num_players_, kInvalidAction);
      players_cards_.reserve(tarot_parent_game_->num_players_);
      players_cards_.insert(players_cards_.end(), tarot_parent_game_->num_players_, std::vector<Action>());
      players_info_states_.reserve(tarot_parent_game_->num_players_);
      players_info_states_.insert(players_info_states_.end(), tarot_parent_game_->num_players_, "");
      mulligan_performed_.reserve(tarot_parent_game_->dog_size_);
    }

    Player TarotState::CurrentPlayer() const
    {
      switch (current_game_phase_)
      {
      case GamePhase::kCardDealing:
        return kChancePlayerId;
      case GamePhase::kFinished:
        return kTerminalPlayerId;
      case GamePhase::kMulligan:
        return taker_;
      case GamePhase::kBidding:
      case GamePhase::kTaker:
      case GamePhase::kTeam:
        return current_player_;
      default:
        return current_player_;
      }
    }

    std::vector<Action> TarotState::PlayerCards(Player player) const
    {
      if (current_game_phase_ == GamePhase::kCardDealing)
        return {};
      return players_cards_.at(player);
    }

    std::vector<Action> TarotState::TrickCards() const
    {
      return current_trick_;
    }

    std::vector<Action> TarotState::LegalActions() const
    {
      if (IsTerminal())
      {
        return {};
      }

      switch (current_game_phase_)
      {
      case GamePhase::kCardDealing:
        return {0};
      case GamePhase::kBidding:
        return LegalActionsInBidding();
      case GamePhase::kMulligan:
        return LegalActionsInMulligan();
      case GamePhase::kTaker:
      case GamePhase::kTeam:
        return LegalActionsInTricksPlaying();
      default:
        SpielFatalError("GamePhase not handled in LegalActions()");
        return {};
      }
    }

    std::vector<Action> TarotState::LegalActionsInBidding() const
    {
      std::vector<Action> legal_actions;

      // Ação de passar sempre é válida
      legal_actions.push_back(kPass); // Pass

      // Adicionar todos os lances válidos
      // No Tarot Francês, só é possível fazer um lance maior que o lance atual
      Action highest_bid = kPass;
      for (auto bid : players_bids_)
      {
        if (bid != kInvalidAction && bid > highest_bid)
        {
          highest_bid = bid;
        }
      }

      // Adicionar todos os lances maiores que o mais alto lance atual
      for (int i = 1; i < kNumBids; i++)
      {
        if (i > highest_bid)
        {
          legal_actions.push_back(i);
        }
      }

      return legal_actions;
    }

    std::vector<Action> TarotState::LegalActionsInTricksPlaying() const
    {
      if (current_trick_.empty())
      {
        return players_cards_[current_player_];
      }
      else
      {
        return LegalActionsInTricksPlayingFollowing();
      }
    }

    std::vector<Action> TarotState::LegalActionsInMulligan() const
    {
      // No Mulligan, o jogador pode escolher qualquer carta de sua mão para descartar
      // mas não pode descartar trunfos ou cartas valiosas (reis)
      std::vector<Action> legal_actions;

      for (auto card_action : players_cards_[current_player_])
      {
        const Card &card = CardFromAction(card_action);
        // Não permitir descarte de trunfos (cartas da suit kTrumps) ou reis (rank 14)
        // Não permitir descarte de cartas que já foram descartadas
        if (card.suit != CardSuit::kTrumps &&
            card.rank != 14 &&
            !std::any_of(mulligan_performed_.begin(), mulligan_performed_.end(), card_action))
          legal_actions.push_back(card_action);
      }
      return legal_actions;
    }

    std::vector<Action> TarotState::LegalActionsInTricksPlayingFollowing() const
    {
      std::vector<Action> legal_actions;
      auto [can_follow_suit, cant_but_has_trump] = CanFollowSuitOrCantButHasTrump();

      // Se pode seguir o naipe, deve seguir
      if (can_follow_suit)
      {
        // Filtra cartas da mão para incluir apenas as do mesmo naipe
        Action first_card_action = current_trick_[0];
        const Card &first_card = CardFromAction(first_card_action);

        for (auto card_action : players_cards_[current_player_])
        {
          const Card &card = CardFromAction(card_action);
          if (card.suit == first_card.suit)
          {
            legal_actions.push_back(card_action);
          }
        }
      }
      // Se não pode seguir naipe mas tem trunfo, deve jogar trunfo
      else if (cant_but_has_trump)
      {
        // Filtra cartas da mão para incluir apenas os trunfos
        for (auto card_action : players_cards_[current_player_])
        {
          const Card &card = CardFromAction(card_action);
          if (card.suit == CardSuit::kTrumps)
          {
            legal_actions.push_back(card_action);
          }
        }
      }
      // Se não pode seguir naipe e não tem trunfo, pode jogar qualquer carta
      else
      {
        legal_actions = players_cards_[current_player_];
      }

      return legal_actions;
    }

    std::tuple<bool, bool> TarotState::CanFollowSuitOrCantButHasTrump() const
    {
      if (current_trick_.empty())
      {
        SpielFatalError("Cannot check if player can follow suit in an empty trick");
      }

      Action first_card_action = current_trick_[0];
      const Card &first_card = CardFromAction(first_card_action);
      CardSuit lead_suit = first_card.suit;

      bool can_follow_suit = false;
      bool has_trump = false;

      // Verificar se o jogador tem cartas do naipe que está sendo jogado
      for (auto card_action : players_cards_[current_player_])
      {
        const Card &card = CardFromAction(card_action);
        if (card.suit == lead_suit)
        {
          can_follow_suit = true;
        }
        if (card.suit == CardSuit::kTrumps)
        {
          has_trump = true;
        }
      }

      // Retorna se pode seguir o naipe e, se não puder, se tem trunfo
      return std::make_tuple(can_follow_suit, !can_follow_suit && has_trump);
    }

    std::string TarotState::ActionToString(Player player, Action action_id) const
    {
      if (current_game_phase_ == GamePhase::kCardDealing)
      {
        return "Deal cards";
      }
      else if (current_game_phase_ == GamePhase::kBidding)
      {
        // Na fase de leilão, retorne o nome do lance
        if (action_id >= 0 && action_id < kNumBids)
          return BidActionToString(action_id);
      }
      else
      {
        // Nas outras fases, retorne o nome da carta
        return CardActionToString(action_id);
      }

      return absl::StrCat("Invalid action: ", action_id);
    }

    std::string TarotState::CardActionToString(Action action_id) const
    {
      if (action_id >= 0 && action_id < kDeckSize)
      {
        return CardFromAction(action_id).long_name;
      }
      return absl::StrCat("Invalid card action: ", action_id);
    }

    ActionsAndProbs TarotState::ChanceOutcomes() const
    {
      if (current_game_phase_ == GamePhase::kCardDealing)
      {
        return {{0, 1.0}}; // Apenas uma ação possível, com probabilidade 1.0
      }
      return {};
    }

    void TarotState::DoApplyAction(Action action_id)
    {
      switch (current_game_phase_)
      {
      case GamePhase::kCardDealing:
        DoApplyActionInCardDealing();
        break;
      case GamePhase::kBidding:
        DoApplyActionInBidding(action_id);
        break;
      case GamePhase::kMulligan:
        mulligan_performed_.push_back(action_id);
        LegalActionsInMulligan();
        break;
      case GamePhase::kTaker:
        // Implementação a ser adicionada
        break;
      case GamePhase::kTeam:
        // Implementação a ser adicionada
        break;
      case GamePhase::kFinished:
        SpielFatalError("Game is already finished, cannot apply action.");
        break;
      }
    }

    void TarotState::DoApplyActionInCardDealing()
    {
      // Distribuição implícita das cartas através do RNG
      do
      {
        card_dealing_seed_ = tarot_parent_game_->RNG();
        std::tie(dog_, players_cards_) =
            DealCards(tarot_parent_game_->num_players_, card_dealing_seed_);
      } while (AnyPlayerWithoutTrump());
      current_game_phase_ = GamePhase::kBidding;
      // Índices de jogador menores correspondem a maior prioridade de leilão,
      // ou seja, 0 é o jogador da frente, num_players - 1 é o distribuidor
      current_player_ = 1;
      AddPrivateCardsToInfoStates();
    }

    void TarotState::AddPrivateCardsToInfoStates()
    {
      std::string dog_append = "Chien: ";
      for (auto card_action : dog_)
      {
        const Card &card = tarot_parent_game_->card_deck_.at(card_action);
        absl::StrAppend(&dog_append, card.long_name, ", ");
      }
      AppendToAllInformationStates(dog_append);

      for (int player = 0; player < tarot_parent_game_->num_players_; player++)
      {
        std::string player_append = "\nPlayer ";
        absl::StrAppend(&player_append, player, " cards: ");
        for (auto card_action : players_cards_[player])
        {
          const Card &card = tarot_parent_game_->card_deck_.at(card_action);
          absl::StrAppend(&player_append, card.long_name, ", ");
        }
        AppendToInformationState(player, player_append);
      }
    }

    void TarotState::DoApplyActionInBidding(Action action_id)
    {
      players_bids_[current_player_] = action_id;

      std::string bid_str = "\nPlayer ";
      absl::StrAppend(&bid_str, current_player_, " bids: ");
      absl::StrAppend(&bid_str, BidActionToString(action_id));
      AppendToAllInformationStates(bid_str);

      // Verificar se todos os jogadores exceto o atual passaram
      // ou se o jogador atual fez um lance
      if (action_id != kPass)
      { // Não passou
        FinishBiddingPhase(action_id);
      }
      else
      {
        NextPlayer();
        // Se todos passaram, o jogo termina
        bool all_passed = true;
        for (int i = 0; i < tarot_parent_game_->num_players_; i++)
        {
          if (players_bids_[i] != kPass && players_bids_[i] != kInvalidAction)
          {
            all_passed = false;
            break;
          }
        }

        if (all_passed)
        {
          current_game_phase_ = GamePhase::kFinished;
        }
      }
    }

    void TarotState::FinishBiddingPhase(Action action_id)
    {
      taker_ = current_player_;
      bid_ = BidFromAction(action_id);

      std::string bid_str = "\nBidding phase finished. Player ";
      absl::StrAppend(&bid_str, taker_, " is the taker with bid: ");
      absl::StrAppend(&bid_str, BidToString(bid_));
      AppendToAllInformationStates(bid_str);

      // No Tarot Francês, após o leilão, pode haver uma fase de mulligan
      // onde o jogador pode descartar cartas
      current_game_phase_ = GamePhase::kMulligan;
      current_player_ = taker_;
    }

    void TarotState::StartTricksPlayingPhase()
    {
      current_game_phase_ = GamePhase::kTaker;
      current_player_ = taker_;

      std::string phase_str = "\nStarting tricks playing phase. Player ";
      absl::StrAppend(&phase_str, taker_, " starts the game as taker.");
      AppendToAllInformationStates(phase_str);
    }

    void TarotState::DoApplyActionInTricksPlaying(Action action_id)
    {
      // Adicionar a carta jogada à lista de cartas da jogada atual
      current_trick_.push_back(action_id);

      // Remover a carta da mão do jogador
      auto it = std::find(players_cards_[current_player_].begin(),
                          players_cards_[current_player_].end(), action_id);
      if (it != players_cards_[current_player_].end())
      {
        players_cards_[current_player_].erase(it);
      }
      else
      {
        SpielFatalError(absl::StrCat("Card action ", action_id,
                                     " not found in player ", current_player_, "'s hand."));
      }

      std::string play_str = "\nPlayer ";
      absl::StrAppend(&play_str, current_player_, " plays: ");
      absl::StrAppend(&play_str, CardFromAction(action_id).long_name);
      AppendToAllInformationStates(play_str);

      // Se todos os jogadores já jogaram neste truque
      if (current_trick_.size() == tarot_parent_game_->num_players_)
      {
        ResolveTrick();
      }
      else
      {
        NextPlayer();
      }

      // Verificar se o jogo acabou (todos os jogadores não têm mais cartas)
      bool game_over = true;
      for (int i = 0; i < tarot_parent_game_->num_players_; i++)
      {
        if (!players_cards_[i].empty())
        {
          game_over = false;
          break;
        }
      }

      if (game_over)
      {
        current_game_phase_ = GamePhase::kFinished;
      }
    }

    void TarotState::ResolveTrick()
    {
      // Encontrar o vencedor do truque
      TrickWinnerAndAction winner_info = ResolveTrickWinnerAndWinningAction();
      Player winner = std::get<0>(winner_info);
      Action winning_action = std::get<1>(winner_info);

      // Adicionar as cartas do truque à pilha de cartas conquistadas pelo vencedor
      for (auto card_action : current_trick_)
      {
        players_tricks_[winner].push_back(card_action);
      }

      std::string trick_str = "\nPlayer ";
      absl::StrAppend(&trick_str, winner, " wins the trick with card: ");
      absl::StrAppend(&trick_str, CardFromAction(winning_action).long_name);
      AppendToAllInformationStates(trick_str);

      // Limpar as cartas do truque atual
      current_trick_.clear();

      // O vencedor inicia o próximo truque
      current_player_ = winner;
    }

    TrickWinnerAndAction TarotState::ResolveTrickWinnerAndWinningAction() const
    {
      if (current_trick_.empty())
      {
        SpielFatalError("Cannot resolve winner of an empty trick.");
      }

      Action first_card_action = current_trick_[0];
      const Card &first_card = CardFromAction(first_card_action);
      CardSuit lead_suit = first_card.suit;

      Action winning_action = first_card_action;
      Player winning_player = TrickCardsIndexToPlayer(0);
      int winning_card_index = 0;

      // Verificar cada carta jogada
      for (int i = 1; i < current_trick_.size(); i++)
      {
        Action current_action = current_trick_[i];
        const Card &current_card = CardFromAction(current_action);
        const Card &winning_card = CardFromAction(current_trick_[winning_card_index]);

        bool current_wins = false;
        if (current_card.suit == CardSuit::kTrumps)
        {
          if (winning_card.suit != CardSuit::kTrumps ||
              (winning_card.suit == CardSuit::kTrumps &&
               current_card.rank > winning_card.rank))
          {
            current_wins = true;
          }
        }
        else if (current_card.suit == lead_suit &&
                 current_card.rank > winning_card.rank)
        {
          current_wins = true;
        }

        if (current_wins)
        {
          winning_action = current_action;
          winning_player = TrickCardsIndexToPlayer(i);
          winning_card_index = i;
        }
      }

      return std::make_tuple(winning_player, winning_action);
    }

    Player TarotState::TrickCardsIndexToPlayer(int index) const
    {
      Player starting_player = current_player_;
      Player player = (starting_player + index) % tarot_parent_game_->num_players_;
      return player;
    }

    std::vector<double> TarotState::Returns() const
    {
      if (IsTerminal())
      {
        std::vector<double> points(tarot_parent_game_->num_players_, 0.0);
        double score = ScoreFinalScore(players_tricks_[taker_], bid_, slam_call_, handful_call_);
        for (int i = 0; i < tarot_parent_game_->num_players_; i++)
        {
          if (i == taker_)
            points[i] = score * (tarot_parent_game_->num_players_ - 1.0);
          else
            points[i] = -score;
        }
        return points;
      }

      // Se o jogo está em bidding, retorna 0 para todos os jogadores
      if (current_game_phase_ == GamePhase::kBidding)
      {
        return std::vector<double>(tarot_parent_game_->num_players_, 0.0);
      }

      // Se o jogo está em tricks playing, retorna 0 para todos os jogadores
      if (current_game_phase_ == GamePhase::kTaker ||
          current_game_phase_ == GamePhase::kTeam)
      {
        std::vector<double> points(tarot_parent_game_->num_players_, 0.0);
        for (int i = 0; i < tarot_parent_game_->num_players_; i++)
        {
          int player_points = 0;
          for (auto card_action : players_tricks_[i])
          {
            const Card &card = tarot_parent_game_->card_deck_.at(card_action);
            player_points += card.points;
          }
          points[i] = player_points;
        }
        return points;
      }

      // Se o jogo está em finished, retorna os pontos finais

      return std::vector<double>(tarot_parent_game_->num_players_, 0.0);
      auto hand = players_cards_[current_player_];
    }

    std::string TarotState::InformationStateString(Player player) const
    {
      if (player < 0 || player >= tarot_parent_game_->num_players_)
      {
        SpielFatalError(absl::StrCat("Invalid player id: ", player));
      }
      return players_info_states_.at(player);
    }

    std::string TarotState::ToString() const
    {
      std::string result = "Game phase: ";
      absl::StrAppend(&result, GamePhaseToString(current_game_phase_));
      absl::StrAppend(&result, "\nCurrent player: ", current_player_);

      if (taker_ != kInvalidPlayer)
      {
        absl::StrAppend(&result, "\nTaker: ", taker_);
      }

      absl::StrAppend(&result, "\nDog: ");
      for (auto card_action : dog_)
      {
        const Card &card = CardFromAction(card_action);
        absl::StrAppend(&result, card.long_name, ", ");
      }

      for (int i = 0; i < tarot_parent_game_->num_players_; i++)
      {
        absl::StrAppend(&result, "\nPlayer ", i, " cards: ");
        for (auto card_action : players_cards_[i])
        {
          const Card &card = CardFromAction(card_action);
          absl::StrAppend(&result, card.long_name, ", ");
        }
      }

      if (!current_trick_.empty())
      {
        absl::StrAppend(&result, "\nCurrent trick: ");
        for (auto card_action : current_trick_)
        {
          const Card &card = CardFromAction(card_action);
          absl::StrAppend(&result, card.long_name, ", ");
        }
      }

      return result;
    }

    std::unique_ptr<State> TarotState::Clone() const
    {
      return std::unique_ptr<State>(new TarotState(*this));
    }

    void TarotState::NextPlayer()
    {
      current_player_ = (current_player_ + 1) % tarot_parent_game_->num_players_;
    }

    bool TarotState::AnyPlayerWithoutTrump() const
    {
      for (int i = 0; i < tarot_parent_game_->num_players_; i++)
      {
        bool has_trump = false;
        for (auto card_action : players_cards_[i])
        {
          const Card &card = tarot_parent_game_->card_deck_.at(card_action);
          if (card.suit == CardSuit::kTrumps)
          {
            has_trump = true;
            break;
          }
        }
        if (!has_trump)
          return true;
      }
      return false;
    }

    void TarotState::MoveActionFromTo(Action action_id, std::vector<Action> *from, std::vector<Action> *to)
    {
      auto it = std::find(from->begin(), from->end(), action_id);
      if (it != from->end())
      {
        to->push_back(action_id);
        from->erase(it);
      }
      else
      {
        SpielFatalError(absl::StrCat("Action ", action_id, " not found in source collection."));
      }
    }

    void TarotState::AppendToAllInformationStates(const std::string &appendix)
    {
      for (int i = 0; i < tarot_parent_game_->num_players_; i++)
      {
        AppendToInformationState(i, appendix);
      }
    }

    void TarotState::AppendToInformationState(Player player,
                                              const std::string &appendix)
    {
      absl::StrAppend(&players_info_states_.at(player), appendix);
    }

    std::string TarotState::Serialize() const
    {
      std::string result;

      // Serializar fase do jogo
      absl::StrAppend(&result, static_cast<int>(current_game_phase_), ",");

      // Serializar jogador atual
      absl::StrAppend(&result, current_player_, ",");

      // Serializar tomador
      absl::StrAppend(&result, taker_, ",");

      // Serializar seed de distribuição de cartas
      absl::StrAppend(&result, card_dealing_seed_, ",");

      // Serializar lances dos jogadores
      for (auto bid : players_bids_)
      {
        absl::StrAppend(&result, bid, ",");
      }
      absl::StrAppend(&result, ";");

      // Serializar cartas dos jogadores
      for (int i = 0; i < tarot_parent_game_->num_players_; i++)
      {
        for (auto card_action : players_cards_[i])
        {
          absl::StrAppend(&result, card_action, ",");
        }
        absl::StrAppend(&result, ";");
      }

      // Serializar cachorro
      for (auto card_action : dog_)
      {
        absl::StrAppend(&result, card_action, ",");
      }
      absl::StrAppend(&result, ";");

      return result;
    }

    std::unique_ptr<State> FrenchTarotGame::DeserializeState(
        const std::string &str) const
    {
      std::unique_ptr<TarotState> state = NewInitialTarotState();

      std::vector<std::string> parts = absl::StrSplit(str, ';');
      if (parts.empty())
      {
        SpielFatalError("Expected at least 1 part in serialized state.");
      }

      // Decomposição da primeira parte (informações gerais)
      std::vector<std::string> general_parts = absl::StrSplit(parts[0], ',');
      if (general_parts.size() < 4)
      {
        SpielFatalError("Expected at least 4 elements in the first part of the serialized state.");
      }

      // Primeiro vem a fase do jogo, jogador atual, tomador e seed
      state->current_game_phase_ = static_cast<GamePhase>(std::stoi(general_parts[0]));
      state->current_player_ = std::stoi(general_parts[1]);
      state->taker_ = std::stoi(general_parts[2]);
      state->card_dealing_seed_ = std::stoi(general_parts[3]);

      // O resto são as cartas do dog
      for (int i = dog_size_; i < general_parts.size() - 1; ++i)
      {
        state->dog_.push_back(std::stoi(general_parts[i]));
      }

      // Processar lances, se houver uma segunda parte
      if (parts.size() > 1)
      {
        std::vector<std::string> bids_parts = absl::StrSplit(parts[1], ',');
        for (int i = 0; i < bids_parts.size() - 1 && i < state->players_bids_.size(); ++i)
        {
          state->players_bids_[i] = std::stoi(bids_parts[i]);
        }
      }

      // Processar cartas dos jogadores, se houver partes suficientes
      for (int player = 0; player < num_players_ && player < parts.size(); ++player)
      {
        std::vector<std::string> cards_parts = absl::StrSplit(parts[player], ',');
        for (int i = 0; i < cards_parts.size() - 1; ++i)
        {
          state->players_cards_[player].push_back(std::stoi(cards_parts[i]));
        }

        std::vector<std::string> dog_parts = absl::StrSplit(parts[num_players_ + 1], ',');
        for (int i = 0; i < dog_parts.size() - 1; ++i)
        {
          state->dog_.push_back(std::stoi(dog_parts[i]));
        }
      }

      return state;
    }

    std::ostream &operator<<(std::ostream &os, const GamePhase &game_phase)
    {
      os << GamePhaseToString(game_phase);
      return os;
    }

    std::string GamePhaseToString(const GamePhase &game_phase)
    {
      switch (game_phase)
      {
      case GamePhase::kCardDealing:
        return "Card dealing";
      case GamePhase::kBidding:
        return "Bidding";
      case GamePhase::kMulligan:
        return "Mulligan";
      case GamePhase::kTaker:
        return "Taker phase";
      case GamePhase::kTeam:
        return "Team phase";
      case GamePhase::kFinished:
        return "Finished";
      }
      return "Unknown phase";
    }

  } // namespace french_tarot
} // namespace open_spiel
