//#define DBG_FILEOUT
#ifdef DBG_FILEOUT
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "PriorityDynamicExpand.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

#ifdef DBG_FILEOUT
namespace {
	FILE* df = fopen("dynamic.txt", "w");
}
#endif

PriorityDynamicExpandMonteCarlo::PriorityDynamicExpandMonteCarlo(int sample, int num_to_expand)
	: PriorityMonteCarloBase("C++ Monte Carlo with dynamic tree expand over " + std::to_string(num_to_expand)
		+ " times (" + std::to_string(sample) + " samples)")
	, sample_num(sample)
	, expand_num(num_to_expand)
{
}
void PriorityDynamicExpandMonteCarlo::GameStart(Reversi::Board b)
{
	ResetRest(b);
#ifdef DBG_FILEOUT
	fprintf(df, "START:(%s)\n", GetPlayerName().c_str());
#endif
}


Reversi::Move PriorityDynamicExpandMonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	VecMove rest = UpdateRest(moves);

	GameTreeNode node = GameTreeNode::BuildTree(b, rest, 1);
	Reversi::Move move = node.PlayOutNWithExpansion(sample_num, CalcScore(64) - CalcScore(-64), expand_num);
#ifdef DBG_FILEOUT
	node.DebugPrint(df);
#endif
	UpdateRest(move);
	return move;
}
void PriorityDynamicExpandMonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}
