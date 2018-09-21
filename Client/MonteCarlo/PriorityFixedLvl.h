#pragma once

#include "PriorityBase.h"



class PriorityFixedLvlMonteCarlo
	:public PriorityMonteCarloBase
{
private:
	const int sample_num;
	const int tree_depth;
public:
	PriorityFixedLvlMonteCarlo(int sample = 100,int depth = 1);
public:
	virtual void GameStart(Reversi::Board b);
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves);
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves);
};
