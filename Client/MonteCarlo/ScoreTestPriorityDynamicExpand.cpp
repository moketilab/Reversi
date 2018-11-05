//#define DBG_FILEOUT
#ifdef DBG_FILEOUT
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ScoreTestPriorityDynamicExpand.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

#ifdef DBG_FILEOUT
namespace {
	FILE* df = fopen("log/dynamic.txt", "w");
	FILE* sf = fopen("log/score.txt", "w");
}
#endif

ScoreTestPriorityDynamicExpandMonteCarlo::ScoreTestPriorityDynamicExpandMonteCarlo(int sample, int num_to_expand)
	: ScoreTestPriorityMonteCarloBase("C++ Monte Carlo with dynamic tree expand over(Score Test)" + std::to_string(num_to_expand)
		+ " times (" + std::to_string(sample) + " samples)")
	, sample_num(sample)
	, expand_num(num_to_expand)
{
}
void ScoreTestPriorityDynamicExpandMonteCarlo::GameStart(Reversi::Board b)
{
	ResetRest(b);
#ifdef DBG_FILEOUT
	fprintf(df, "START:(%s)\n", GetPlayerName().c_str());
#endif
}


Reversi::Move ScoreTestPriorityDynamicExpandMonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	VecMove rest = UpdateRest(moves);

	GameTreeNode node = GameTreeNode::BuildTree(b, rest, 1);
#ifdef DBG_FILEOUT
	Reversi::Move move = node.PlayOutNWithExpansion(sample_num, CalcScore(64) - CalcScore(-64), expand_num, SCORE_METHOD_AVE_MAX_GSD, sf);
	node.DebugPrint(df);
#else
	Reversi::Move move = node.PlayOutNWithExpansion(sample_num, CalcScore(64) - CalcScore(-64), expand_num, SCORE_METHOD_AVE_MAX_GSD);
#endif
	UpdateRest(move);
	return move;
}
void ScoreTestPriorityDynamicExpandMonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}
