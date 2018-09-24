#include "PriorityBase.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

Reversi::Move PriorityMonteCarloBase::GameTreeNode::PlayOutN(int n, double coeff)
{
	while (total_times < n)
	{
		PlayOutAndUpdate(coeff);
	}
	auto node = children.begin();
	double best = node->total_score;
	auto bestnode = node;
	for (; node != children.end(); node++)
	{
		if (node->total_score > best)
		{
			best = node->total_score;
			bestnode = node;
		}
	}
	return bestnode->m;
}

Reversi::Move PriorityMonteCarloBase::GameTreeNode::PlayOutNWithExpansion(int n, double coeff, int num_to_expand)
{
	while (total_times < n)
	{
		PlayOutAndUpdateWithExpansion(coeff, num_to_expand);
	}
	auto node = children.begin();
	double best = node->total_score;
	auto bestnode = node;
	for (; node != children.end(); node++)
	{
		if (node->total_score > best)
		{
			best = node->total_score;
			bestnode = node;
		}
	}
	return bestnode->m;
}

PriorityMonteCarloBase::GameTreeNode PriorityMonteCarloBase::GameTreeNode::BuildTree(Reversi::Board board, VecMove rest, int depth)
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
	}
	rv.is_leaf = false;
	rv.UpdateCurrentScore();
	return rv;
}

PriorityMonteCarloBase::GameTreeNode PriorityMonteCarloBase::GameTreeNode::BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn)
{
	b.MovePlayer(m, my_turn);
	rest.erase(std::find(rest.begin(), rest.end(), m));
	my_turn = !my_turn;
	VecMove legal_moves = FindMoveList(b, rest, my_turn);
	if (legal_moves.size() == 0)
	{
		my_turn = !my_turn;
		legal_moves = FindMoveList(b, rest, my_turn);
		if (legal_moves.size() == 0)
		{
			rest.clear();
			GameTreeNode rv(m, b, rest, my_turn);
			rv.is_leaf = true;
			rv.times = 1;
			rv.score = CalcScore(b.Score());
			rv.total_score = rv.score;
			rv.total_times = rv.times;
			return rv;
		}
	}
	if (depth <= 0)
	{
		GameTreeNode rv(m, b, rest, my_turn);
		rv.is_leaf = true;
		rv.times = 1;
		rv.score = CalcScore(PlayOut(b,rest,my_turn));
		rv.total_score = rv.score;
		rv.total_times = rv.times;
		return rv;
	}
	GameTreeNode rv(m, b, rest, my_turn);
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = BuildTree(*i, b, rest, depth - 1, my_turn);
		rv.children.push_back(node);
	}
	rv.is_leaf = false;
	rv.UpdateCurrentScore();
	return rv;
}

bool PriorityMonteCarloBase::GameTreeNode::ExpandTree()
{
	VecMove legal_moves = FindMoveList(b, rest, is_my_turn);
	assert(legal_moves.size() > 0);
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = BuildTree(*i, b, rest, 0, is_my_turn);
		children.push_back(node);
	}
	is_leaf = false;
	UpdateCurrentScore();
	return true;
}

void PriorityMonteCarloBase::GameTreeNode::PlayOutAndUpdate(double coeff)
{
	if (is_leaf)
	{
		times++;
		score += CalcScore(PlayOut(b, rest, is_my_turn));
		total_score = score / (double)times;
		total_times = times;
		return;
	}
	// not leaf
	double coeff_score = coeff * sqrt(2 * log(total_times));
	double sign_score = is_my_turn ? 1.0 : -1.0;
	auto node = children.begin();

	double best = sign_score * node->total_score + coeff_score / sqrt(node->total_times);
	GameTreeNode* bestnode = &node[0];
	for (node++; node != children.end(); node++)
	{
		double score = sign_score * node->total_score + coeff_score / sqrt(node->total_times);
		if(score > best)
		{
			best = score;
			bestnode = &node[0];
		}
	}
	bestnode->PlayOutAndUpdate(coeff);
	UpdateCurrentScore();
}

void PriorityMonteCarloBase::GameTreeNode::PlayOutAndUpdateWithExpansion(double coeff, int num_to_expand)
{
	if (is_leaf)
	{
		times++;
		score += CalcScore(PlayOut(b, rest, is_my_turn));
		if ((times >= num_to_expand)&& !rest.empty())
		{
			if (ExpandTree())
				return;
		}
		total_score = score / (double)times;
		total_times = times;
		return;
	}
	// not leaf
	double coeff_score = coeff * sqrt(2 * log(total_times));
	double sign_score = is_my_turn ? 1.0 : -1.0;
	auto node = children.begin();

	double best = sign_score * node->total_score + coeff_score / sqrt(node->total_times);
	GameTreeNode* bestnode = &node[0];
	for (node++; node != children.end(); node++)
	{
		double score = sign_score * node->total_score + coeff_score / sqrt(node->total_times);
		if (score > best)
		{
			best = score;
			bestnode = &node[0];
		}
	}
	bestnode->PlayOutAndUpdateWithExpansion(coeff, num_to_expand);
	UpdateCurrentScore();
}

void PriorityMonteCarloBase::GameTreeNode::UpdateCurrentScore()
{
	subtree_times = 0;
	subtree_score = is_my_turn ? CalcScore(-64) : CalcScore(64);
	for (auto node = children.begin(); node != children.end(); node++)
	{
		subtree_times += node->total_times;
		if (is_my_turn)
			subtree_score = std::max(subtree_score, node->total_score);
		else
			subtree_score = std::min(subtree_score, node->total_score);
	}
	total_times = subtree_times + times;
	total_score = score / (double)total_times + subtree_score * subtree_times / (double)total_times;
}

void PriorityMonteCarloBase::GameTreeNode::DebugPrint(FILE* f, int indent)
{
	static const char space[] = "                                              ";
	static const int  s_len = sizeof(space) - 1;
	fprintf(f, "%s%c%d: %c%c :", space + s_len - indent, is_my_turn ? 'M' : 'O', indent,
		'a' + m.GetX(), '1' + m.GetY());
	fprintf(f, "%7.4f (%5d times, node [%4d / %4d] subtree [ ave %7.4f (%d)])\n"
		, total_score,total_times, score, times, subtree_score , subtree_times);
	PrintBoard(b, f, indent);
	for (auto i = children.begin(); i != children.end(); i++)
	{
		i->DebugPrint(f, indent + 1);
	}
}

