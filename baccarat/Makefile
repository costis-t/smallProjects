# g++ main.o game.o story.o sue.o config.o -o ./baccarat
baccarat: main.o game.o story.o sue.o
	g++ -o baccarat main.o game.o story.o sue.o

main.o: main.cpp
	g++ -c main.cpp

#config.o: config.cpp config.h
#	g++ -c config.cpp

game.o: game.cpp game.h config.h
	g++ -c game.cpp

story.o: story.cpp story.h sue.h
	g++ -c story.cpp

sue.o: sue.cpp sue.h story.h config.h
	g++ -c sue.cpp

clean:
	rm *.o baccarat simulation_million.csv simulation_million.output
