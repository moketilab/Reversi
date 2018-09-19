#pragma once

#include "reversi.h"
#include <iostream>
#include <string>
#include <vector>

class PlayerBase
{
public:
	PlayerBase(){}
	virtual ~PlayerBase() {}
public:
	virtual std::string GetPlayerName()=0;
	virtual void GameStart(Reversi::Board b)=0;
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves) = 0;
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)=0;
protected:
	static unsigned int Random();
};

class ReversiPlayerStdioClient
{
	PlayerBase* player;
public:
	ReversiPlayerStdioClient(PlayerBase* p) :player(p) {}
	~ReversiPlayerStdioClient() {}
public:
	int StartClient();
};