#include "PriorityFixedLvl.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

PriorityFixedLvlMonteCarlo::PriorityFixedLvlMonteCarlo(int sample, int depth)
	: MonteCarloBase("C++ Monte Carlo with search depth " + std::to_string(depth) 
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

int PriorityFixedLvlMonteCarlo::CalcScore(int x)
{
	if (x > 0)
		return 3;
	else if (x == 0)
		return 1;
	return 0;
}

Reversi::Move PriorityFixedLvlMonteCarlo::GameTreeNode::PlayOutN(int n, double coeff)
{
	while (times < n)
	{
		PlayOutAndUpdate(coeff);
	}
	auto node = children.begin();
	double best = node->current_score;
	auto bestnode = node;
	for (; node != children.end(); node++)
	{
		if (node->current_score > best)
		{
			best = node->current_score;
			bestnode = node;
		}
	}
	return bestnode->m;
}

PriorityFixedLvlMonteCarlo::GameTreeNode PriorityFixedLvlMonteCarlo::GameTreeNode::BuildTree(Reversi::Board board, VecMove rest, int depth)
{
	GameTreeNode rv(Reversi::Move(0,0), board, rest, true);
	VecMove legal_moves = FindMoveList(board, rest);
	depth = depth - 1;
	if (legal_moves.size() == 1)
		depth = 0;
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = BuildTree(*i, board, rest, depth, true);
		rv.children.push_back(node);
		rv.times += node.times;
		rv.current_score = std::max(rv.current_score, node.current_score);
	}
	rv.is_leaf = false;
	return rv;
}

PriorityFixedLvlMonteCarlo::GameTreeNode PriorityFixedLvlMonteCarlo::GameTreeNode::BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn)
{
	b.MovePlayer(m, my_turn);
	rest.erase(std::find(rest.begin(), rest.end(), m));
	my_turn = !my_turn;
	if (depth <= 0)
	{
		GameTreeNode rv(m, b, rest, my_turn);
		rv.is_leaf = true;
		rv.times = 1;
		rv.score = CalcScore(PlayOut(b,rest,my_turn));
		rv.current_score = rv.score;
		return rv;
	}
	VecMove legal_moves = FindMoveList(b, rest, my_turn);
	if (legal_moves.size() == 0)
	{
		my_turn = !my_turn;
		legal_moves = FindMoveList(b, rest, my_turn);
		if (legal_moves.size() == 0)
		{
			GameTreeNode rv(m, b, rest, my_turn);
			rv.is_leaf = true;
			rv.times = 1;
			rv.score = CalcScore(b.Score());
			rv.current_score = rv.score;
			return rv;
		}
	}
	GameTreeNode rv(m, b, rest, my_turn);
	rv.current_score = my_turn ? CalcScore(-64) : CalcScore(64);
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = BuildTree(*i, b, rest, depth - 1, my_turn);
		rv.children.push_back(node);
		rv.times += node.times;
		if (my_turn)
			rv.current_score = std::max(rv.current_score, node.current_score);
		else
			rv.current_score = std::min(rv.current_score, node.current_score);
	}
	rv.is_leaf = false;
	return rv;
}

void PriorityFixedLvlMonteCarlo::GameTreeNode::PlayOutAndUpdate(double coeff)
{
	if (is_leaf)
	{
		score += CalcScore(PlayOut(b, rest, is_my_turn));
		times++;
		current_score = score / (double)times;
		return;
	}
	// not leaf
	double coeff_score = coeff * sqrt(2 * log(times));
	double sign_score = is_my_turn ? 1.0 : -1.0;
	auto node = children.begin();

	double best = sign_score * node->current_score + coeff_score / sqrt(node->times);
	GameTreeNode* bestnode = &node[0];
	for (node++; node != children.end(); node++)
	{
		double score = sign_score * node->current_score + coeff_score / sqrt(node->times);
		if(score > best)
		{
			best = score;
			bestnode = &node[0];
		}
	}
	bestnode->PlayOutAndUpdate(coeff);
	UpdateCurrentScore();
}

void PriorityFixedLvlMonteCarlo::GameTreeNode::UpdateCurrentScore()
{
	times = 0;
	current_score = is_my_turn ? CalcScore(-64) : CalcScore(64);
	for (auto node = children.begin(); node != children.end(); node++)
	{
		times += node->times;
		if (is_my_turn)
			current_score = std::max(current_score, node->current_score);
		else
			current_score = std::min(current_score, node->current_score);
	}
}

void PriorityFixedLvlMonteCarlo::GameTreeNode::DebugPrint(FILE* f, int indent)
{
	static const char space[] = "                                              ";
	static const int  s_len = sizeof(space) - 1;
	fprintf(f, "%s%c%d: %c%c :", space + s_len - indent, is_my_turn ? 'M' : 'O', indent,
		'a' + m.GetX(), '1' + m.GetY());
	if (is_leaf)
		fprintf(f, "%7.4f (%4d / %4d)\n", current_score, score, times);
	else 
		fprintf(f, "%7.4f (/%4d)\n", current_score, times);
	PrintBoard(b, f, indent);
	for (auto i = children.begin(); i != children.end(); i++)
	{
		i->DebugPrint(f, indent + 1);
	}
}

