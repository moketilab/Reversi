

#include "..\ReversiBase\reversi.h"
#include "..\ReversiBase\player.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>

class GameRandom
	:public PlayerBase
{
public:
	GameRandom()
	{
	}
public:
	virtual std::string GetPlayerName() { return "C++ Random Player";}
	virtual void GameStart(Reversi::Board b)
	{

	}
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
	{
		Reversi::Move m(0, 0);
		do
		{
			m= Reversi::Move(Random()&7,Random()&7);
		} while (!b.MoveMine(m));
		return m;
	}
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
	{

	}

};




int main(int argc, char** argv)
{
	GameRandom game;
	return ReversiPlayerStdioClient(&game).StartClient();
}

