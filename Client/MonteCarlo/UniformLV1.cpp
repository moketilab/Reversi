#include "UniformLv1.h"

#include <stdio.h>
#include <string>
#include <algorithm>


UniformLv1MonteCarlo::UniformLv1MonteCarlo(int sample)
	: MonteCarloBase("C++ Simple Monte Carlo(" + std::to_string(sample) + ")")
	, sample_num(sample)
{
}
void UniformLv1MonteCarlo ::GameStart(Reversi::Board b)
{
	ResetRest(b);
}
Reversi::Move UniformLv1MonteCarlo::GetMove(Reversi::Board b, std::vector<Reversi::Move> moves)
{
	//Reversi::PrintBoard(b, stderr);
	VecMove rest = UpdateRest(moves);
	VecMove mv = FindMoveList(b, rest);
	if (mv.size() < 1)
		return Reversi::Move(0, 0);
	else if (mv.size() == 1)
		return mv[0];
	int each_sample = sample_num / mv.size() + 1;
	Reversi::Move best_move = mv[0];
	float best_score = PlayOutN(b, mv[0], each_sample, rest);
	//fprintf(stderr, "%c%c: %f\n", mv[0].GetX() + 'A', mv[0].GetY() + '1', best_score);
	for (auto m = mv.begin() + 1; m != mv.end(); m++)
	{
		float score = PlayOutN(b, *m, each_sample, rest);
		//fprintf(stderr, "%c%c: %f\n", m->GetX() + 'A', m->GetY() + '1', score);
		if (best_score < score)
		{
			best_score = score;
			best_move = *m;
		}
	}
	//fprintf(stderr, " BestMove %c%c: %f\n\n", best_move.GetX() + 'A', best_move.GetY() + '1', best_score);
	return best_move;
}
void UniformLv1MonteCarlo::GameSet(Reversi::Board b, std::vector<Reversi::Move> moves)
{
}

float UniformLv1MonteCarlo::PlayOutN(Reversi::Board b, Reversi::Move m, int trial, VecMove rest)
{
	b.MoveMine(m);
	rest.erase(std::find(rest.begin(), rest.end(), m));
	int score = 0;
	for (int i = 0; i < trial; i++)
		score += CalcScore(PlayOut(b, rest));
	return score / (float)trial;
}

