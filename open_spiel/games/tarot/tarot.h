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

#ifndef OPEN_SPIEL_GAMES_FRENCH_TAROT_H_
#define OPEN_SPIEL_GAMES_FRENCH_TAROT_H_

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "open_spiel/abseil-cpp/absl/types/optional.h"
#include "open_spiel/games/tarot/cards.h"
#include "open_spiel/games/tarot/bid.h"
#include "open_spiel/spiel.h"

namespace open_spiel
{
  namespace french_tarot
  {

    inline constexpr int kNumPlayers = 4;
    inline constexpr int kDefaultSeed = -1;
    inline constexpr int kSlamCall = 1;
    inline constexpr int kHandfullCall = 1;
    inline constexpr int kDefaultNumPLayers = 4;
    inline constexpr int kDefaultDogSize = 6;

    enum class GamePhase
    {
      kCardDealing,
      kBidding,
      kMulligan,
      kTaker,
      kTeam,
      kFinished,
    };

    class TarotState;

    class FrenchTarotGame : public Game
    {
    public:
      explicit FrenchTarotGame(const GameParameters &params);

      int NumDistinctActions() const override { return kDeckSize; };
      std::unique_ptr<State> NewInitialState() const override;
      std::unique_ptr<TarotState> NewInitialTarotState() const;
      int MaxChanceOutcomes() const override { return 1; };
      int NumPlayers() const override { return num_players_; };
      double MinUtility() const override { return -1.0; };
      double MaxUtility() const override { return 1.0; };
      int MaxGameLength() const override { return num_players_ * (kDeckSize - dog_size_); };

      std::unique_ptr<State> DeserializeState(
          const std::string &str) const override;
      std::string GetRNGState() const override
      {
        std::ostringstream rng_stream;
        rng_stream << rng_;
        return rng_stream.str();
      };
      void SetRNGState(const std::string &rng_state) const override
      {
        std::istringstream rng_stream(rng_state);
        rng_stream >> rng_;
      };

    private:
      friend class TarotState;
      
      static inline const std::array<Card, kDeckSize> card_deck_ = InitializeCardDeck();
      
      const int dog_size_;
      const int num_players_;

      mutable std::mt19937 rng_;
      int RNG() const { return rng_(); }
    };

    using TrickWinnerAndAction = std::tuple<Player, Action>;
    using CollectedCardsPerTeam =
        std::tuple<std::vector<Action>, std::vector<Action>>;

    class TarotState : public State
    {
    public:
      explicit TarotState(std::shared_ptr<const Game> game);

      bool IsTerminal() const override { return current_game_phase_ == GamePhase::kFinished; };
      Player CurrentPlayer() const override;
      GamePhase CurrentGamePhase() const;
      std::vector<Action> PlayerCards(Player player) const;
      std::vector<Action> TrickCards() const;

      std::vector<Action> LegalActions() const override;
      std::string ActionToString(Player player, Action action_id) const override;
      std::string CardActionToString(Action action_id) const;
      ActionsAndProbs ChanceOutcomes() const override;

      std::vector<double> Returns() const override;
      std::string InformationStateString(Player player) const override;

      std::string ToString() const override;
      std::string Serialize() const override;
      std::unique_ptr<State> Clone() const override;

    protected:
      void DoApplyAction(Action action_id) override;

    private:
      int round_ = 0;
      std::vector<Action> dog_;
      std::vector<Action> players_bids_;
      std::vector<Action> current_trick_;
      std::vector<std::vector<Action>> players_tricks_;
      std::vector<std::vector<Action>> players_cards_;
      std::vector<Action> mulligan_performed_;

      Bid bid_ = Bid::kPass;
      bool slam_call_ = false;
      bool handful_call_ = false;
      Player taker_ = kInvalidPlayer;
      Player current_player_ = kInvalidPlayer;
      std::vector<int> public_information_;

      GamePhase current_game_phase_ = GamePhase::kCardDealing;
      int card_dealing_seed_ = kDefaultSeed;

      std::shared_ptr<const FrenchTarotGame> tarot_parent_game_;
      friend class FrenchTarotGame;

      std::vector<std::string> players_info_states_;

      void NextPlayer();

      std::vector<Action> LegalActionsInBidding() const;
      std::vector<Action> LegalActionsInMulligan() const;
      std::vector<Action> LegalActionsInTricksPlaying() const;
      std::vector<Action> LegalActionsInTricksPlayingFollowing() const;
      std::tuple<bool, bool> CanFollowSuitOrCantButHasTrump() const;

      void DoApplyActionInCardDealing();
      bool AnyPlayerWithoutTrump() const;
      void AddPrivateCardsToInfoStates();
      void DoApplyActionInBidding(Action action_id);
      void FinishBiddingPhase(Action action_id);

      void StartTricksPlayingPhase();
      void DoApplyActionInTricksPlaying(Action action_id);
      TrickWinnerAndAction ResolveTrickWinnerAndWinningAction() const;
      Player TrickCardsIndexToPlayer(int index) const;
      void ResolveTrick();

      static bool ActionInActions(Action action_id,
                                  const std::vector<Action> &actions);
      static void MoveActionFromTo(Action action_id, std::vector<Action> *from,
                                   std::vector<Action> *to);
      const Card &ActionToCard(Action action_id) const;
      void AppendToAllInformationStates(const std::string &appendix);
      void AppendToInformationState(Player player, const std::string &appendix);
    };

    std::ostream &operator<<(std::ostream &os, const GamePhase &game_phase);
    std::string GamePhaseToString(const GamePhase &game_phase);

  } // namespace french_tarot
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_FRENCH_TAROT_H_
