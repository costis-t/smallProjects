#ifndef _Sue
#define _Sue

struct Story;

#include "config.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

enum Rank : unsigned int;
enum Suit : unsigned int;

struct Card {
  Rank rank;
  Suit suit;
  size_t num_suits = 4;
  size_t num_ranks = 13;
  size_t value;
  void print_card();
  size_t card_value();
  void print_card_assignment_form();
  void print_card_for_shoe();
};

struct Shoe {
  std::vector<Card> cards;
  size_t max_shoe_size = config::NUM_DECKS;
  void initialize_shoe();
  void print_shoe();
  void print_shoe_beginning();
  // std::string print_shoe_string();
  void shuffle_shoe();
  bool burn_cards(bool);
};

#endif // _Sue
