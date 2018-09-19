#pragma once

#include "MonteCarloBase.h"

class PriorityLv1MonteCarlo
	:public MonteCarloBase
{
	struct MoveInfo
	{
		const Reversi::Move m;
		const Reversi::Board b;
		const VecMove        rest;
		int            times;
		int            score;
		MoveInfo(Reversi::Move _m,Reversi::Board _b, VecMove _rest):m(_m),b(_b),rest(_rest),times(0),score(0){}
	};
	typedef std::vector<MoveInfo> MoveInfoList;
	const int sample_num;
public:
	PriorityLv1MonteCarlo(int sample = 100);
public:
	virtual void GameStart(Reversi::Board b);
	virtual Reversi::Move GetMove(Reversi::Board b, std::vector<Reversi::Move> moves);
	virtual void GameSet(Reversi::Board b, std::vector<Reversi::Move> moves);
private:
	static int CalcScore(int x);
	static void AddMoveInfoList(MoveInfoList& list, Reversi::Board b, Reversi::Move m, VecMove rest);
	static MoveInfo* GetBestMove(MoveInfoList& list, float coeff);
};
