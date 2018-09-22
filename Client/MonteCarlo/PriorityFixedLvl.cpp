//#define DBG_FILEOUT
#ifdef DBG_FILEOUT
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "PriorityFixedLvl.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

#ifdef DBG_FILEOUT
namespace {
	FILE* df = fopen("fixed.txt", "w");
}
#endif

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
#ifdef DBG_FILEOUT
	fprintf(df, "START:(%s)\n",GetPlayerName().c_str());
#endif
}

Reversi::Move PriorityFixedLvlMonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	VecMove rest = UpdateRest(moves);

	GameTreeNode node = GameTreeNode::BuildTree(b, rest, tree_depth);
	Reversi::Move move = node.PlayOutN(sample_num, CalcScore(64) - CalcScore(-64));
#ifdef DBG_FILEOUT
	node.DebugPrint(df);
#endif
	return move;
}
void PriorityFixedLvlMonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}
