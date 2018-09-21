#pragma once

#include "..\ReversiBase\reversi.h"
#include "..\ReversiBase\player.h"

#include <string>

class MonteCarloBase
	:public PlayerBase
{
protected:
	typedef std::vector<Reversi::Move> VecMove;
private:
	std::string name;
//protected:
private:
	VecMove rest;
protected:
	MonteCarloBase(std::string _name) :name(_name) {}
private:
	virtual std::string GetPlayerName() { return name; }
protected:
	const VecMove& ResetRest(const Reversi::Board& b);
	const VecMove& UpdateRest(const VecMove& moves);
	static VecMove FindMoveList(Reversi::Board b, const VecMove& moves);
	static VecMove FindMoveList(Reversi::Board b, const VecMove& moves, bool is_my_move);
	static int PlayOut(Reversi::Board b, VecMove rest, bool playerNext = false);
};