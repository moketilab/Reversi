#include "PriorityLV1.h"

#include <stdio.h>
#include <string>
#include <algorithm>

PriorityLv1MonteCarlo::PriorityLv1MonteCarlo(int sample)
	: MonteCarloBase("C++ Priority Monte Carlo(" + std::to_string(sample) + ")")
	, sample_num(sample)
{
}
void PriorityLv1MonteCarlo::GameStart(Reversi::Board b)
{
	ResetRest(b);
}
Reversi::Move PriorityLv1MonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	VecMove rest = UpdateRest(moves);
	VecMove mv = FindMoveList(b, rest);
	if (mv.size() < 1)
		return Reversi::Move(0, 0);
	else if (mv.size() == 1)
		return mv[0];
	MoveInfoList list;
	for (auto m = mv.begin(); m != mv.end(); m++)
		AddMoveInfoList(list, b, *m, rest);
	int trial = list.size();

	while (trial < sample_num)
	{
		MoveInfo* next = GetBestMove(list, (CalcScore(64) - CalcScore(-64))* sqrt(2 * log(trial)));
		next->times++;
		next->score += CalcScore(PlayOut(next->b, next->rest));
		trial++;
	}

	return GetBestMove(list, 0)->m;
}
void PriorityLv1MonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}

int PriorityLv1MonteCarlo::CalcScore(int x)
{
	if (x > 0)
		return 3;
	else if (x == 0)
		return 1;
	return 0;
}
void PriorityLv1MonteCarlo::AddMoveInfoList(MoveInfoList& list, Reversi::Board b, Reversi::Move m, VecMove rest)
{
	b.MoveMine(m);
	rest.erase(std::find(rest.begin(), rest.end(), m));
	MoveInfo mi(m, b, rest);
	mi.score = CalcScore(PlayOut(b, rest));
	mi.times = 1;
	list.push_back(mi);
}

PriorityLv1MonteCarlo::MoveInfo* PriorityLv1MonteCarlo::GetBestMove(MoveInfoList& list, float coeff)
{
	auto m = list.begin();
	float best = m->score / (float)m->times + coeff / sqrt(m->times);
	MoveInfo* ret = &m[0];
	for (m++; m != list.end(); m++)
	{
		float score = m->score / (float)m->times + coeff / sqrt(m->times);
		if (score > best)
		{
			best = score;
			ret = &m[0];
		}
	}
	return ret;
}


