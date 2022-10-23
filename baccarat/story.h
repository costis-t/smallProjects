#ifndef _Story
#define _Story

#include <thread>
#include <iostream>
#include <vector>

struct Player; // forward declaration

struct Story {
  static void pluses();
  static void empty_line();
  void preamble();
  void after_shoe();
  void before_shuffling();
  void after_shuffling();
  static void first_burned_card(std::vector<Card> &, size_t);
  void deal_cards();
  std::vector<std::string> natural();
  void show_hands(const std::vector<Player> &);
  std::vector<std::string> game_over();
  void game_over_titles();
  void print_hands_and_outcome(std::string, std::string, std::string);
  void print_ugly_hands_and_outcome(std::string, std::string, std::string);
  static std::string string_hand_assignment_form(const std::vector<Card> &);
  static std::string string_hand_ugly_form(const std::vector<Card> &);
  std::vector<std::string> player_draws();
  std::vector<std::string> player_stands();
  std::vector<std::string> banker_draws();
  std::vector<std::string> banker_stands();
  std::vector<std::string> banker_draws_tableau();
  std::vector<std::string> banker_stands_tableau();
  void delay(size_t);
};

#endif // _Story
