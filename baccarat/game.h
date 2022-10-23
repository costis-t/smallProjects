#ifndef _Game
#define _Game

// struct Card;
// struct Shoe;

//#include "config.h"
#include "sue.h"


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

struct Player {
  std::vector<Card> hand;
  std::string name = "";
  size_t initial_score = 0;
  size_t score = 0;
  std::string state;
  std::string player_state;
  std::string banker_state = "stood";
  int total_hand_score();
  void print_hand(std::string hand_name);
  void print_hand_assignment_form();
  int hand_score(Card card_1, Card card_2);
  std::string string_hand_assignment_form();
  void check_for_score();
};


struct Game {
  std::vector<Player>
      players; // std:array better than std::vector for embedded systems and
               // other reasons (small fixed length, etc...).
  Shoe shoe;
  size_t num_players = 2;
  size_t initial_cards = 2;
  bool deal_cards(Shoe &); 
  void initialize();
  void add_players();
  void print_game(); 
  void display_state(size_t);
  void play(std::string);
  void play_without_story();
  void play_with_ugly_output();
  std::string result(size_t, size_t);
  std::string ugly_result(size_t, size_t);
  void draw_card(Shoe &, int);
};

#endif // _Game
