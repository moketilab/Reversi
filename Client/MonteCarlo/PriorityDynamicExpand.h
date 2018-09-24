﻿#pragma once

#include "PriorityBase.h"



class PriorityDynamicExpandMonteCarlo
	:public PriorityMonteCarloBase
{
private:
	const int sample_num;
	const int expand_num;
public:
	PriorityDynamicExpandMonteCarlo(int sample = 100,int num_to_expand = 10);
public:
	virtual void GameStart(Reversi::Board b);
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves);
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves);
};
