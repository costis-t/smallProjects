#ifndef CONFIG
#define CONFIG

#include <string>

namespace config {
const int NUM_DECKS = 8;
const size_t SECONDS_DELAY= 5;
const std::string Rank_faces[13] = {"Ace",  "2",     "3",   "4", "5",
                                    "6",    "7",     "8",   "9", "10",
                                    "Jack", "Queen", "King"}; // NONE = 0
const std::string Rank_ugly_faces[13] = {
    "A", "2", "3",  "4", "5", "6", "7",
    "8", "9", "10", "J", "Q", "K"}; // NONE = 0
const std::string Suit_faces[4] = {"Clubs", "Diamonds", "Hearts", "Spades"};
const size_t tableau[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

} // namespace config

#endif // CONFIG
