#include "sue.h"
#include "story.h"

//////////////////////
///// Cards //////////
//////////////////////

enum Rank : unsigned int {
  Ace,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King
};

enum Suit : unsigned int { Clubs, Diamonds, Hearts, Spades };

void Card::print_card() {
  std::cout << "Rank: " << config::Rank_faces[rank]
            << ", Suit: " << config::Suit_faces[suit]
            << ", Value: " << card_value() << "\n";
}

void Card::print_card_for_shoe() {
  std::cout << "[Rank: " << config::Rank_faces[rank]
            << ", Suit: " << config::Suit_faces[suit]
            << ", Value: " << card_value() << "]  ===  ";
}

size_t Card::card_value() {
  if (rank > 8) {
    return 0;
  } else {
    return rank + 1;
  }
}

///////////////////
//// Shoe /////////
///////////////////

void Shoe::initialize_shoe() {
  for (size_t count = 0; count < max_shoe_size; count++) {
    Card card;
    for (size_t suit = 0; suit < card.num_suits; suit++) {
      for (size_t rank = 0; rank < card.num_ranks; rank++) {
        card.suit = Suit(suit);
        card.rank = Rank(rank);
        cards.push_back(card);
      }
    }
  }
}

void Shoe::print_shoe() {
  for (Card c : cards) {
    c.print_card_for_shoe();
  }
  std::cout << "\n";
}

void Shoe::print_shoe_beginning() {
  for (int i = 0; i < 15; i++) {
    cards[i].print_card_for_shoe();
  }
  std::cout << "...\n";
}



void Shoe::shuffle_shoe() {
  // Need to explore randommess more;
  // Check:
  // https://codereview.stackexchange.com/questions/109260/seed-stdmt19937-from-stdrandom-device,
  // https://stackoverflow.com/questions/45069219/how-to-succinctly-portably-and-thoroughly-seed-the-mt19937-prng,
  // and others...
  size_t seconds_since_epoch =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  size_t microseconds_since_epoch =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now().time_since_epoch())
          .count();
  std::random_device rdev;
  std::mt19937::result_type seed =
      rdev() ^ (seconds_since_epoch + microseconds_since_epoch);
  std::mt19937 gen(seed);
  // Check space complexity https://www.youtube.com/watch?v=4zx5bM2OcvA&t=194s
  // Fisher-Yates modern algorithm
  Shoe shuffled;
  
  // empty() is a std::vector function which indicates if anything is left
  while (!cards.empty()) {
    std::uniform_int_distribution<unsigned> distrib(0, (cards.size()) - 1);
    size_t rand_index = distrib(gen);
    shuffled.cards.push_back(cards[rand_index]);
    cards.erase(cards.begin() + rand_index);
  }
  *this = shuffled;
}

bool Shoe::burn_cards(bool with_story) {
  if (cards.size() < 25) { // Boccorat game needs far less than 25 cards drawn,
    // skipping error checking in general
    return false;
  }
  // First burn according to wikipedia https://en.wikipedia.org/wiki/Baccarat#Punto_banco
  std::vector<Card> hand_burned_cards;
  hand_burned_cards.push_back(cards[0]);
  cards.erase(cards.begin());
  size_t burned_face_up_card_value = hand_burned_cards[0].rank;
  size_t cards_to_burn =
      (burned_face_up_card_value > 8) ? 10 : burned_face_up_card_value + 1;
  if (with_story) {
    Story::first_burned_card(hand_burned_cards, cards_to_burn);
  }
  for (size_t c = 0; c < cards_to_burn; c++) {
    cards.erase(cards.begin());
  }
  return true;
}
