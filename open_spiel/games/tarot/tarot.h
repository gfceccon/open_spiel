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
#include "open_spiel/games/tarot/bids.h"
#include "open_spiel/spiel.h"

namespace open_spiel
{
  namespace french_tarot
  {

    inline constexpr int kNumPlayers = 4;
    inline constexpr int kDefaultSeed = -1;
    inline constexpr int kSlamCall = 1;
    inline constexpr int kHandfullCall = 1;
    inline constexpr int kMaxGameLength = kNumPlayers * (kDeckSize - kDogSize);

    enum class GamePhase
    {
      kCardDealing,
      kBidding,
      kMulligan,
      kTaker, // Le Preneur
      kTeam,  // Le Defenseur
      kFinished,
    };

    class TarotState;

    class FrenchTarotGame : public Game
    {
    public:
      explicit FrenchTarotGame(const GameParameters &params);

      int NumDistinctActions() const override;
      std::unique_ptr<State> NewInitialState() const override;
      std::unique_ptr<TarotState> NewInitialTarotState() const;
      int MaxChanceOutcomes() const override;
      int NumPlayers() const override;
      double MinUtility() const override;
      double MaxUtility() const override;
      int MaxGameLength() const override;

      std::unique_ptr<State> DeserializeState(
          const std::string &str) const override;
      std::string GetRNGState() const override;
      void SetRNGState(const std::string &rng_state) const override;

    private:
      friend class TarotState;
      int RNG() const;

      static inline const std::array<Card, kDeckSize> card_deck_ = InitializeCardDeck();
      static inline const std::array<Bid, kNumBids> bids_ = InitializeBids();

      const Bid bid_;
      const int num_players_;
      mutable std::mt19937 rng_;
    };

    using TrickWinnerAndAction = std::tuple<Player, Action>;
    using CollectedCardsPerTeam =
        std::tuple<std::vector<Action>, std::vector<Action>>;

    class TarotState : public State
    {
    public:
      explicit TarotState(std::shared_ptr<const Game> game);

      bool IsTerminal() const override;
      Player CurrentPlayer() const override;
      GamePhase CurrentGamePhase() const;
      std::vector<Action> PlayerCards(Player player) const;
      Bid SelectedBidName() const;
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
      std::vector<Action> dog_;
      std::vector<Action> players_bids_;
      std::vector<Action> players_trick_cards_;
      std::vector<std::vector<Action>> players_cards_;

      Player taker_ = kInvalidPlayer;
      Player current_player_ = kInvalidPlayer;
      std::vector<Player> players_collected_cards_;
      std::vector<std::string> players_info_states_;

      GamePhase current_game_phase_ = GamePhase::kCardDealing;
      int card_dealing_seed_ = kDefaultSeed;

      std::shared_ptr<const FrenchTarotGame> tarot_parent_game_;
      friend class FrenchTarotGame;

      void NextPlayer();

      std::vector<Action> LegalActionsInBidding() const;
      std::vector<Action> LegalActionsInMulligan() const;
      std::vector<Action> LegalActionsInTricksPlaying() const;
      std::vector<Action> LegalActionsInTricksPlayingFollowing() const;
      std::tuple<bool, bool> CanFollowSuitOrCantButHasTrump() const;
      
      
      void DoApplyActionInCardDealing();
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
