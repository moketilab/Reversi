#pragma once

#include "MonteCarloBase.h"

class UniformLv1MonteCarlo
	:public MonteCarloBase
{
	const int sample_num;
public:
	UniformLv1MonteCarlo(int sample = 100);
public:
	virtual void GameStart(Reversi::Board b);
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves);
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves);
private:
	static float PlayOutN(Reversi::Board b, Reversi::Move m, int trial, VecMove rest);
};
