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
#include <cmath>
#include <ctime>

#include "open_spiel/abseil-cpp/absl/strings/str_split.h"
#include "open_spiel/spiel.h"
#include "tarot.h"

namespace open_spiel
{
  namespace french_tarot
  {

    const GameType kGameType{"tarot",                      // short_name
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
                             // parameter_specification
                             {{"players", GameParameter(kNumPlayers)},
                              {"rng_seed", GameParameter(kDefaultSeed)}}};

    std::shared_ptr<const Game> Factory(const GameParameters &params)
    {
      return std::shared_ptr<const Game>(new FrenchTarotGame(params));
    }

    REGISTER_SPIEL_GAME(kGameType, Factory);

    RegisterSingleTensorObserver single_tensor(kGameType.short_name);

    FrenchTarotGame::FrenchTarotGame(const GameParameters &params)
        : Game(kGameType, params),
          num_players_(ParameterValue<int>("players")),
          bid_(Bid(BidName::kPass, 0))
    {
      rng_ = std::mt19937(
          ParameterValue<int>("rng_seed") == -1
              ? std::time(0)
              : ParameterValue<int>("rng_seed"));
    }

    int FrenchTarotGame::NumDistinctActions() const { return kDeckSize; }
    int FrenchTarotGame::MaxChanceOutcomes() const { return 1; }

    std::unique_ptr<State> FrenchTarotGame::NewInitialState() const
    {
      return NewInitialTarotState();
    }

    std::unique_ptr<TarotState> FrenchTarotGame::NewInitialTarotState() const
    {
      return std::make_unique<TarotState>(shared_from_this());
    }

    int FrenchTarotGame::NumPlayers() const { return num_players_; }

    double FrenchTarotGame::MinUtility() const { return -1.0; }

    double FrenchTarotGame::MaxUtility() const { return 1.0; }

    int FrenchTarotGame::MaxGameLength() const { return kMaxGameLength; }

    std::unique_ptr<State> FrenchTarotGame::DeserializeState(
        const std::string &str) const
    {
    }

    std::string FrenchTarotGame::GetRNGState() const
    {
    }

    void FrenchTarotGame::SetRNGState(const std::string &rng_state) const
    {
    }

    int FrenchTarotGame::RNG() const { return rng_(); }

    // state implementation
    TarotState::TarotState(std::shared_ptr<const Game> game)
        : State(game),
          tarot_parent_game_(std::static_pointer_cast<const FrenchTarotGame>(game))
    {
    }

    Player TarotState::CurrentPlayer() const
    {
    }

    bool TarotState::IsTerminal() const
    {
    }

    GamePhase TarotState::CurrentGamePhase() const { return current_game_phase_; }

    std::vector<Action> TarotState::PlayerCards(Player player) const
    {
    }

    Bid TarotState::SelectedBidName() const
    {
    }

    std::vector<Action> TarotState::TrickCards() const
    {
    }

    std::vector<Action> TarotState::LegalActions() const
    {
    }

    std::vector<Action> TarotState::LegalActionsInBidding() const
    {
    }

    std::vector<Action> TarotState::LegalActionsInTricksPlaying() const
    {
    }

    std::vector<Action> TarotState::LegalActionsInMulligan() const
    {
    }
    std::vector<Action> TarotState::LegalActionsInTricksPlayingFollowing() const
    {
    }

    std::tuple<bool, bool> TarotState::CanFollowSuitOrCantButHasTrump() const
    {
    }

    std::string TarotState::ActionToString(Player player, Action action_id) const
    {
    }

    std::string TarotState::CardActionToString(Action action_id) const
    {
    }

    ActionsAndProbs TarotState::ChanceOutcomes() const
    {
    }

    void TarotState::DoApplyAction(Action action_id)
    {
    }

    void TarotState::DoApplyActionInCardDealing()
    {
    }

    void TarotState::AddPrivateCardsToInfoStates()
    {
    }

    void TarotState::DoApplyActionInBidding(Action action_id)
    {
    }

    void TarotState::FinishBiddingPhase(Action action_id)
    {
    }

    void TarotState::StartTricksPlayingPhase()
    {
    }

    void TarotState::DoApplyActionInTricksPlaying(Action action_id)
    {
    }

    void TarotState::ResolveTrick()
    {
    }

    TrickWinnerAndAction TarotState::ResolveTrickWinnerAndWinningAction() const
    {
    }

    Player TarotState::TrickCardsIndexToPlayer(int index) const
    {
    }

    std::vector<double> TarotState::Returns() const
    {
    }

    std::string TarotState::InformationStateString(Player player) const
    {
    }

    std::string TarotState::ToString() const
    {
    }

    std::string TarotState::Serialize() const
    {
    }

    std::unique_ptr<State> TarotState::Clone() const
    {
      return std::unique_ptr<State>(new TarotState(*this));
    }

    void TarotState::NextPlayer()
    {
    }

    bool TarotState::ActionInActions(Action action_id,
                                     const std::vector<Action> &actions)
    {
    }

    void TarotState::MoveActionFromTo(Action action_id, std::vector<Action> *from,
                                      std::vector<Action> *to)
    {
    }

    const Card &TarotState::ActionToCard(Action action_id) const
    {
    }

    void TarotState::AppendToAllInformationStates(const std::string &appendix)
    {
    }

    void TarotState::AppendToInformationState(Player player,
                                              const std::string &appendix)
    {
      absl::StrAppend(&players_info_states_.at(player), appendix);
    }

    std::ostream &operator<<(std::ostream &os, const GamePhase &game_phase)
    {
    }

    std::string GamePhaseToString(const GamePhase &game_phase)
    {
    }

  } // namespace french_tarot
} // namespace open_spiel
