#include "PriorityFixedLvl.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

PriorityFixedLvlMonteCarlo::PriorityFixedLvlMonteCarlo(int sample, int depth)
	: PriorityMonteCarloBase("C++ Monte Carlo with search depth " + std::to_string(depth)
		+ " (" + std::to_string(sample) + " times)")
	, sample_num(sample)
	, tree_depth(depth)
{
}
void PriorityFixedLvlMonteCarlo::GameStart(Reversi::Board b)
{
	ResetRest(b);
}

Reversi::Move PriorityFixedLvlMonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	VecMove rest = UpdateRest(moves);

	GameTreeNode node = GameTreeNode::BuildTree(b, rest, tree_depth);
	Reversi::Move move = node.PlayOutN(sample_num, CalcScore(64) - CalcScore(-64));
	return move;
}
void PriorityFixedLvlMonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}
