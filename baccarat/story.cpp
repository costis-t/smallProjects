#include "sue.h"
#include "story.h"

//////////////////////
///// Story //////////
//////////////////////

void Story::empty_line() { std::cout << "\n"; }

void Story::pluses() { std::cout << "+++++++++++++++\n"; }

std::vector<std::string> Story::natural() {
  std::vector<std::string> sv;
  sv.push_back("From the first deal, we have a \n");
  sv.push_back("natural hand and, hence, a winner.\n");
  return sv;
}

void Story::game_over_titles() {
  std::cout << "`--._,--'\"`--._,--'\"`--._,--'\"`--._,--'\n";
  std::cout << "`--._,--'\"`--. GAME OVER --'\"`--._,--'\"\n";
  std::cout << "`--._,--'\"`--._,--'\"`--._,--'\"`--._,--'\n";
}

std::vector<std::string> Story::banker_stands() {
  std::vector<std::string> sv;
  sv.push_back("The player stood pat.\n");
  sv.push_back("Banker's hand has a total \n");
  sv.push_back("value of 6 or 7 so (s)he stands.\n");
  return sv;
}

std::vector<std::string> Story::banker_draws_tableau() {
  std::vector<std::string> sv;
  sv.push_back("The player drew a third card.\n");
  sv.push_back("The banker follows the \n");
  sv.push_back("tableau and draws, too.\n");
  return sv;
}

std::vector<std::string> Story::banker_stands_tableau() {
  std::vector<std::string> sv;
  sv.push_back("The player drew a third card.\n");
  sv.push_back("The banker follows the tableau\n");
  sv.push_back("and does not draw.\n");
  return sv;
}

std::vector<std::string> Story::banker_draws() {
  std::vector<std::string> sv;
  sv.push_back("The player stood pat.\n");
  sv.push_back("Banker's hand has a total value \n");
  sv.push_back("of less or equal to 5 so (s)he draws.\n");
  return sv;
}

std::vector<std::string> Story::player_draws() {
  std::vector<std::string> sv;
  sv.push_back("Player's hand total is 5 or less so (s)he draws.\n");
  return sv;
}

std::vector<std::string> Story::player_stands() {
  std::vector<std::string> sv;
  sv.push_back("Player's hand has a total value \n");
  sv.push_back("of 6 or 7 so (s)hee player stands pat.\n");
  return sv;
}

std::vector<std::string> Story::game_over() {
  std::vector<std::string> sv;
  sv.push_back("Our game ends with the following hands and result.\n\n");
  sv.push_back("Can you guess it?\n");
  return sv;
}

void Story::after_shoe() {
  pluses();
  empty_line();
  // pluses();
}

void Story::before_shuffling() {
  pluses();
  std::cout << "Then, we asked an amazing team of cardists, \n";
  std::cout << "The Virts (https://go.thevirts.com/), to shuffle \n";
  std::cout << "our very long shoe as best as they could. This is\n";
  std::cout << "what the shoe looked like after their divine touch!:\n";
  pluses();
  empty_line();
  pluses();
}

void Story::after_shuffling() {
  pluses();
  std::cout << "They had to leave for a performance, so not \n";
  std::cout << "knowing what to do with the shoe, we decide to\n";
  std::cout << " name it ---what else?--- Sue and play Baccarat.\n";
  pluses();
  empty_line();
  pluses();
}

void Story::deal_cards() {
  pluses();
  std::cout << "After dealing alternatively 2 cards to the Player \n";
  std::cout << "and 2 to the Banker, this is what the shoe looks like:\n";
  pluses();
  empty_line();
  pluses();
}

void Story::preamble() {
  pluses();
  std::cout << "Initially, our artisan " << config::NUM_DECKS << " deck(s)\n";
  std::cout << "of cards were never used for mesmerizing cardistry.\n";
  std::cout << "Fresh out of the box, they came in good order.\n";
  std::cout << "They were combined to a very long ordered shoe (always first "
               "cards only): \n";
  pluses();
  empty_line();
  pluses();
}

void Story::first_burned_card(std::vector<Card> &hand_burned_cards,
                              size_t cards_to_burn) {
  std::cout << "The first card of the freshly shuffled shoe is:\n";
  hand_burned_cards[0].print_card();
  std::cout << "Id est: " << config::Rank_faces[hand_burned_cards[0].rank]
            << ", " << config::Suit_faces[hand_burned_cards[0].suit] << ".\n";
  std::cout << "Because its rank, starting from 0, is "
            << hand_burned_cards[0].rank << ", " << cards_to_burn
            << " more card(s) are burned face down.\n";
  pluses();
  empty_line();
  pluses();
  std::cout << "Now that the first cards were burned, the shoe looks like:\n";
  pluses();
  empty_line();
  pluses();
}

std::string Story::string_hand_assignment_form(const std::vector<Card> &hand) {
  auto delimiter = ", ";
  auto no_delimiter = "";
  std::string string_to_print = "";
  for (Card c : hand) {
    string_to_print.append(no_delimiter);
    string_to_print.append(config::Rank_faces[c.rank]);
    no_delimiter = delimiter;
  }
  return string_to_print;
}

std::string Story::string_hand_ugly_form(const std::vector<Card> &hand) {
  auto delimiter = ",";
  auto no_delimiter = "";
  std::string string_to_print = "";
  for (Card c : hand) {
    string_to_print.append(no_delimiter);
    string_to_print.append(config::Rank_ugly_faces[c.rank]);
    no_delimiter = delimiter;
  }
  return string_to_print;
}

void Story::print_hands_and_outcome(std::string player_hand_string,
                                    std::string banker_hand_string,
                                    std::string outcome) {
  pluses();
  auto phand = "Player Hand";
  auto bhand = "Banker Hand";
  auto _outcome = "Outcome";

  size_t phand_strlen =
      std::max(std::strlen(phand) + 7, player_hand_string.length());
  size_t bhand_strlen =
      std::max(std::strlen(bhand) + 7, banker_hand_string.length());
  size_t _outcome_strlen =
      std::max(std::strlen(_outcome) + 7, outcome.length());
  std::cout << std::left << std::setw(phand_strlen) << phand
            << std::setw(bhand_strlen) << bhand << std::setw(_outcome_strlen)
            << _outcome << "\n";
  std::cout << std::left << std::setw(phand_strlen) << player_hand_string
            << std::setw(bhand_strlen) << banker_hand_string
            << std::setw(_outcome_strlen) << outcome << "\n";
  pluses();
  empty_line();
}

void Story::print_ugly_hands_and_outcome(std::string player_hand_string,
                                         std::string banker_hand_string,
                                         std::string outcome) {
  std::cout << "PHand - BHand - Outcome" << std::endl;
  std::cout << player_hand_string << " - " << banker_hand_string << " - "
            << outcome << std::endl;
}

void Story::delay(size_t seconds) {
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(seconds));
}
