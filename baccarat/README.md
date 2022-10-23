# License

Distributed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) License. See `LICENSE.txt` for more information.


# About The Game
I made this game to refresh my simple C++ skills.
This project was made with personal fun optimization as its focus; it's not optimized for embedded systems, compilation time, etc...
It is my own work, but I made it after acquiring a lot of insights and knowledge from variuos sources.

Special thanks to the authors of these books:
* Scott Meyers, 2014, [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/), O'Reilly Media
* Brian Overland, 2016, [C++ Without Fear](https://www.pearson.com/en-us/subject-catalog/p/c-without-fear-a-beginners-guide-that-makes-you-feel-smart/P200000000526/9780134314303), Pearson Education
* Divya Sachdeva, Natalya Ustukpayeva,  Sufyan bin Uzayr (editor) (2022), [Mastering C++ Programming Language](https://www.taylorfrancis.com/books/mono/10.1201/9781003214762/mastering-programming-language-sufyan-bin-uzayr-sufyan-bin-uzayr?context=ubx&refId=bdf15248-bf46-46e6-9231-fb3d3df0a35a), CRC Press, Taylor & Francis Group
* George S. Tselikis, 2023,  [Introduction to C++](https://www.taylorfrancis.com/books/mono/10.1201/9781003230076/introduction-george-tselikis), CRC Press, Taylor & Francis Group

and to the community members and content creators of:
* [C++ reference](https://en.cppreference.com/w/cpp)
* [Skypjack](https://stackoverflow.com/users/4987285/skypjack)
* [Snowhawk](https://codereview.stackexchange.com/users/38656/snowhawk)
* [Jason Turner](https://www.youtube.com/c/lefticus1/videos)
* [John Seiffertt](https://www.youtube.com/channel/UCPGpqVNc83RwMZlWSqnBY_A/videos)
* [Yan Chernikov](https://www.youtube.com/c/TheChernoProject/videos)


# Getting Started
## Prerequisites
Standard C++ libraries and compilation environment.

## Compilation
```sh
make clean
make baccarat
```

or

```sh
rm baccarat; g++ main.cpp game.cpp story.cpp sue.cpp -o baccarat
```

## Usage

The game has three modes:
* **Story mode (default).** This is alternative to verbose output. It builds suspence (!), explains what the game does and urges the player to think of the outcome before it appears on the screen.
```sh 
./baccarat
```
* **Without Story.** This mode outputs the result in a breathtakingly beautiful format.
```sh 
./baccarat --without-story
```

* **Ugly format.** This mode outputs the result, without good looking formatting. It is ideal for simulations.
```sh 
./baccarat --ugly-output
```

# After running the game
## Simulation
`simulation.py` includes some elementary testing of the results of the game.
Its commented lines include output after running the game 1.000.000 times.
The game's resulting probabilities are consistent with various online sources, like the [Wizard of Odds: Baccarat Analysis](https://wizardofodds.com/games/baccarat/appendix/1/).

## Output
### Without story
```
$ ./baccarat --without-story
+++++++++++++++
Player Hand       Banker Hand       Outcome       
3, Queen, 3       9, 4, Queen       Player wins   
+++++++++++++++

```

### Ugly output format
```
$ ./baccarat --ugly-format
PHand - BHand - Outcome
3,3 - 7,Q - BANKER
```

### With story
```
$ ./baccarat
+++++++++++++++
Initially, our artisan 8 deck(s)
of cards were never used for mesmerizing cardistry.
Fresh out of the box, they came in good order.
They were combined to a very long ordered shoe (always first cards only): 
+++++++++++++++

+++++++++++++++
[Rank: Ace, Suit: Clubs, Value: 1]  ===  [Rank: 2, Suit: Clubs, Value: 2]  ===  
[Rank: 3, Suit: Clubs, Value: 3]  ===  [Rank: 4, Suit: Clubs, Value: 4]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  [Rank: 6, Suit: Clubs, Value: 6]  === 
 [Rank: 7, Suit: Clubs, Value: 7]  ===  [Rank: 8, Suit: Clubs, Value: 8]  ===  
 [Rank: 9, Suit: Clubs, Value: 9]  ===  [Rank: 10, Suit: Clubs, Value: 0]  ===  
 [Rank: Jack, Suit: Clubs, Value: 0]  ===  [Rank: Queen, Suit: Clubs, Value: 0]  ===  
 [Rank: King, Suit: Clubs, Value: 0]  ===  [Rank: Ace, Suit: Diamonds, Value: 1]  ===  
 [Rank: 2, Suit: Diamonds, Value: 2]  ===  ...
+++++++++++++++

+++++++++++++++
Then, we asked an amazing team of cardists, 
The Virts (https://go.thevirts.com/), to shuffle 
our very long shoe as best as they could. This is
what the shoe looked like after their divine touch!:
+++++++++++++++

+++++++++++++++
[Rank: 7, Suit: Clubs, Value: 7]  ===  [Rank: 7, Suit: Clubs, Value: 7]  ===  
[Rank: King, Suit: Diamonds, Value: 0]  ===  [Rank: Queen, Suit: Clubs, Value: 0]  ===  
[Rank: 4, Suit: Hearts, Value: 4]  ===  [Rank: Jack, Suit: Clubs, Value: 0]  ===  
[Rank: 4, Suit: Diamonds, Value: 4]  ===  [Rank: 6, Suit: Clubs, Value: 6]  ===  
[Rank: 3, Suit: Spades, Value: 3]  ===  [Rank: 10, Suit: Spades, Value: 0]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  [Rank: 3, Suit: Spades, Value: 3]  ===  
[Rank: 2, Suit: Clubs, Value: 2]  ===  [Rank: 3, Suit: Clubs, Value: 3]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  ...
+++++++++++++++

+++++++++++++++
They had to leave for a performance, so not 
knowing what to do with the shoe, we decide to
 name it ---what else?--- Sue and play Baccarat.
+++++++++++++++

+++++++++++++++
[Rank: 3, Suit: Spades, Value: 3]  ===  [Rank: 10, Suit: Spades, Value: 0]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  [Rank: 3, Suit: Spades, Value: 3]  ===  
[Rank: 2, Suit: Clubs, Value: 2]  ===  [Rank: 3, Suit: Clubs, Value: 3]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  [Rank: 10, Suit: Diamonds, Value: 0]  ===  
[Rank: 9, Suit: Hearts, Value: 9]  ===  [Rank: 4, Suit: Diamonds, Value: 4]  ===  
[Rank: 3, Suit: Spades, Value: 3]  ===  [Rank: 3, Suit: Hearts, Value: 3]  ===  
[Rank: 2, Suit: Spades, Value: 2]  ===  [Rank: King, Suit: Clubs, Value: 0]  ===  
[Rank: 7, Suit: Diamonds, Value: 7]  ===  ...
+++++++++++++++

+++++++++++++++
After dealing alternatively 2 cards to the Player 
and 2 to the Banker, this is what the shoe looks like:
+++++++++++++++

+++++++++++++++
[Rank: 2, Suit: Clubs, Value: 2]  ===  [Rank: 3, Suit: Clubs, Value: 3]  ===  
[Rank: 5, Suit: Clubs, Value: 5]  ===  [Rank: 10, Suit: Diamonds, Value: 0]  ===  
[Rank: 9, Suit: Hearts, Value: 9]  ===  [Rank: 4, Suit: Diamonds, Value: 4]  ===  
[Rank: 3, Suit: Spades, Value: 3]  ===  [Rank: 3, Suit: Hearts, Value: 3]  ===  
[Rank: 2, Suit: Spades, Value: 2]  ===  [Rank: King, Suit: Clubs, Value: 0]  ===  
[Rank: 7, Suit: Diamonds, Value: 7]  ===  [Rank: 7, Suit: Spades, Value: 7]  ===  
[Rank: 2, Suit: Spades, Value: 2]  ===  [Rank: Jack, Suit: Clubs, Value: 0]  ===  
[Rank: Queen, Suit: Spades, Value: 0]  ===  ...
+++++++++++++++

+++++++++++++++
From the first deal, we have a 
natural hand and, hence, a winner.
+++++++++++++++

+++++++++++++++
Our game ends with the following hands and result.

Can you guess it?
+++++++++++++++

+++++++++++++++
Player Hand       Banker Hand       Outcome       
3, 5              10, 3             Player wins   
+++++++++++++++

`--._,--'"`--._,--'"`--._,--'"`--._,--'
`--._,--'"`--. GAME OVER --'"`--._,--'"
`--._,--'"`--._,--'"`--._,--'"`--._,--'

```

# Future
If I had more time I would do the following:
- [ ] Application of template metaprogramming and design patterns, like decorating for story output
- [ ] Optimization for embedded systems (size/speed?)
- [ ] Optimization for simulation
- [ ] Graphical User Interface
- [ ] Add inline documentation
