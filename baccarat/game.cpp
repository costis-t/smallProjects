#include "game.h"
#include "story.h"

#include <vector>
#include <iostream>

//////////////////////
///// Player //////////
//////////////////////

int Player::total_hand_score() {
  int hand_score = 0;
  for (Card c : hand) {
    hand_score += c.card_value();
  }
  return hand_score % 10;
}

//////////////////////
///// Game //////////
//////////////////////

template <typename T>
std::ostream &operator<<(std::ostream &stream,
                         const std::vector<T> &string_vector) {
  std::cout << "+++++++++++++++\n";
  for (auto sv : string_vector) {
    stream << sv;
  }
  std::cout << "+++++++++++++++\n";
  std::cout << "\n";
  return stream;
}

std::string Game::result(size_t player_score, size_t banker_score) {
  size_t _res = player_score - banker_score +
                100; // +100 because negative values possible; otherwise int +0;
                     // size_t vs int performance differences to be profiled
  std::string result =
      (_res == 100) ? "Tie" : ((_res > 100) ? "Player wins" : "Banker wins");
  return result;
}

std::string Game::ugly_result(size_t player_score, size_t banker_score) {
  size_t _res = player_score - banker_score + 100;
  std::string result =
      (_res == 100) ? "TIE" : ((_res > 100) ? "PLAYER" : "BANKER");
  return result;
}

void Game::add_players() {
  for (size_t player = 0; player < num_players; player++) {
    Player new_player;
    players.push_back(new_player);
  }
}

void Game::draw_card(Shoe &shoe, int player_index) {
  players[player_index].hand.push_back(shoe.cards[0]);
  shoe.cards.erase(shoe.cards.begin());
}

void Game::play(std::string mode) 
{
  std::vector<int> natural = {8, 9};

  Story story;
  
  Shoe shoe;
  shoe.initialize_shoe();
  
  bool with_story = false;
  if (mode == "with story") {
  bool with_story = true;
  story.preamble();
  // shoe.print_shoe();
  shoe.print_shoe_beginning();
  story.after_shoe();
  story.delay(config::SECONDS_DELAY);
  story.before_shuffling();
  }
  
  shoe.shuffle_shoe();

  if (mode == "with story") {
  // shoe.print_shoe();
  shoe.print_shoe_beginning();
  story.after_shoe();
  story.delay(config::SECONDS_DELAY);
  story.after_shuffling();
  }

  // shoe.burn_cards(false);
  shoe.burn_cards(with_story);
  // shoe.print_shoe();
  if (mode == "with story") {
  shoe.print_shoe_beginning();
  story.after_shoe();
  story.delay(config::SECONDS_DELAY);
  }

  add_players();

  bool game_over = false;
  size_t player = 0; // index for player
  players[player].name = "Player";
  players[player + 1].name = "Banker";

  while (!game_over) {
    draw_card(shoe, 0);
    draw_card(shoe, 1);
    draw_card(shoe, 0);
    draw_card(shoe, 1);

    players[player].initial_score = players[player].score =
        players[player].total_hand_score();
    players[player + 1].initial_score = players[player + 1].score =
        players[player + 1].total_hand_score();

    
    // shoe.print_shoe();
    if (mode == "with story") {
    story.deal_cards();
    shoe.print_shoe_beginning();
    story.after_shoe();
    }
    // story.show_hands(players);

    if (std::binary_search(natural.begin(), natural.end(),
                           players[player].initial_score) ||
        std::binary_search(natural.begin(), natural.end(),
                           players[player + 1].initial_score)) {
      if (mode == "with story") {
      std::cout << story.natural();
      }
      game_over = true;
      break;
    }

    if (players[player].initial_score <= 5) {
      players[player].state = "drew";

      draw_card(shoe, 0);
      players[player].score = players[player].total_hand_score();
      if (mode == "with story") {
      std::cout << story.player_draws();
      // story.show_hands(players);
      }
    } else {
      players[player].state = "pat";
      if (mode == "with story") {
      std::cout << story.player_stands();
      }
    }

    if (players[player].state == "pat") {
      if (players[player + 1].initial_score <= 5) {
        players[player + 1].state = "drew";
        draw_card(shoe, 1);
        players[player + 1].score = players[player + 1].total_hand_score();
        if (mode == "with story") {
        std::cout << story.banker_draws();
        }
      } else {
        players[player + 1].state = "pat";
        if (mode == "with story") {
        std::cout << story.banker_stands();
        }
      }
    } else {
      Card player_third_card = players[player].hand.back();
      size_t player_third_card_value =
          player_third_card.card_value();
      size_t banker_draws = config::tableau[players[player + 1].initial_score]
                                           [player_third_card_value];
      players[player + 1].state = (banker_draws == 1) ? "drew" : "stood";
      if (players[player + 1].state == "drew") {
        draw_card(shoe, 1);
        players[player + 1].score = players[player + 1].total_hand_score();
        if (mode == "with story") {
        std::cout << story.banker_draws_tableau();
        }
      } else {
        if (mode == "with story") {
        std::cout << story.banker_stands_tableau();
        }
      }
    }
    game_over = true;
  }
  if (mode == "with story") {
  std::cout << story.game_over();
  story.delay(config::SECONDS_DELAY);
  story.print_hands_and_outcome(
      Story::string_hand_assignment_form(players[player].hand),
      Story::string_hand_assignment_form(players[player + 1].hand),
      result(players[player].score, players[player + 1].score));
  story.game_over_titles();
  } else if (mode == "without story") {
      story.print_hands_and_outcome(
      Story::string_hand_assignment_form(players[player].hand),
      Story::string_hand_assignment_form(players[player + 1].hand),
      result(players[player].score, players[player + 1].score));
  } else {
      story.print_ugly_hands_and_outcome(
      Story::string_hand_ugly_form(players[player].hand),
      Story::string_hand_ugly_form(players[player + 1].hand),
      ugly_result(players[player].score, players[player + 1].score));
  }
}
