#include "ScoreTestPriorityBase.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

Reversi::Move ScoreTestPriorityMonteCarloBase::GameTreeNode::PlayOutN(int n, double coeff, ScoreMethod method)
{
	while (variety_score[method].total_times < n)
	{
		PlayOutAndUpdate(coeff,method);
	}
	auto node = children.begin();
	double best = node->variety_score[method].total_score;
	auto bestnode = node;
	for (; node != children.end(); node++)
	{
		if (node->variety_score[method].total_score > best)
		{
			best = node->variety_score[method].total_score;
			bestnode = node;
		}
	}
	return bestnode->m;
}

Reversi::Move ScoreTestPriorityMonteCarloBase::GameTreeNode::PlayOutNWithExpansion(int n, double coeff, int num_to_expand, ScoreMethod method, FILE* fd)
{
	if (fd)
	{
		fprintf(fd, "START (%d)\n", 60-rest.size());
		Reversi::PrintBoard(b, fd);
		for (auto i = children.begin(); i != children.end(); i++)
			for (int m = 0; m < SCORE_METHOD_END; m++)
				fprintf(fd, "%c%c %5s,", i->m.GetX()+'A', i->m.GetY() + '1', GetMethodName(ScoreMethod(m)));
		fprintf(fd, "\n");
		for (auto i = children.begin(); i != children.end(); i++)
			for (int m = 0; m < SCORE_METHOD_END; m++)
				fprintf(fd, "%8.5f,", i->variety_score[m].total_score);
		fprintf(fd, "\n");
	}
	while (variety_score[method].total_times < n)
	{
		for (int m = 0; m < SCORE_METHOD_END; m++)
			PlayOutAndUpdateWithExpansion(coeff, num_to_expand, ScoreMethod(m));
		if (fd)
		{
			for(auto i=children.begin(); i != children.end();i++ )
				for(int m=0; m < SCORE_METHOD_END; m++)
					fprintf(fd, "%8.5f,", i->variety_score[m].total_score);
			fprintf(fd, "\n");
		}
	}
	auto node = children.begin();
	double best = node->variety_score[method].total_score;
	auto bestnode = node;
	for (; node != children.end(); node++)
	{
		if (node->variety_score[method].total_score > best)
		{
			best = node->variety_score[method].total_score;
			bestnode = node;
		}
	}
	return bestnode->m;
}

ScoreTestPriorityMonteCarloBase::GameTreeNode ScoreTestPriorityMonteCarloBase::GameTreeNode::BuildTree(Reversi::Board board, VecMove rest, int depth)
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
	for (int i = 0; i < SCORE_METHOD_END; i++)
	{
		rv.UpdateCurrentScore(ScoreMethod(i));
	}
	return rv;
}

ScoreTestPriorityMonteCarloBase::GameTreeNode ScoreTestPriorityMonteCarloBase::GameTreeNode::BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn)
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
			PlayOutResult result;
			result.score = CalcScore(b.Score());
			result.score_total = result.score;
			rv.playout_results.push_back(result);
			for (int i = 0; i < SCORE_METHOD_END; i++)
			{
				rv.variety_score[i].is_leaf     = true;
				rv.variety_score[i].times       = 1;
				rv.variety_score[i].total_score = result.score;
				rv.variety_score[i].total_times = 1;
			}
			return rv;
		}
	}
	if (depth <= 0)
	{
		GameTreeNode rv(m, b, rest, my_turn);
		rv.is_leaf = true;
		PlayOutResult result;
		result.score = CalcScore(PlayOut(b, rest, my_turn));
		result.score_total = result.score;
		rv.playout_results.push_back(result);
		for (int i = 0; i < SCORE_METHOD_END; i++)
		{
			rv.variety_score[i].is_leaf     = true;
			rv.variety_score[i].times       = 1;
			rv.variety_score[i].total_score = result.score;
			rv.variety_score[i].total_times = 1;
		}
		return rv;
	}
	GameTreeNode rv(m, b, rest, my_turn);
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = BuildTree(*i, b, rest, depth - 1, my_turn);
		rv.children.push_back(node);
	}
	rv.is_leaf = false;
	for (int i = 0; i < SCORE_METHOD_END; i++)
	{
		rv.UpdateCurrentScore(ScoreMethod(i));
	}
	return rv;
}

ScoreTestPriorityMonteCarloBase::GameTreeNode ScoreTestPriorityMonteCarloBase::GameTreeNode::CreateNode(Reversi::Move m, Reversi::Board b, VecMove rest, bool my_turn)
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
			rest.clear();
	}
	GameTreeNode rv(m, b, rest, my_turn);
	rv.is_leaf = true;
	rv.playout_results.clear();
	for (int i = 0; i < SCORE_METHOD_END; i++)
	{
		rv.variety_score[i].is_leaf = true;
		rv.variety_score[i].times = 0;
		rv.variety_score[i].total_score = 0;
		rv.variety_score[i].total_times = 0;
	}
	return rv;
}

bool ScoreTestPriorityMonteCarloBase::GameTreeNode::ExpandTree()
{
	VecMove legal_moves = FindMoveList(b, rest, is_my_turn);
	assert(legal_moves.size() > 0);
	for (auto i = legal_moves.begin(); i != legal_moves.end(); i++)
	{
		GameTreeNode node = CreateNode(*i, b, rest, is_my_turn);
		children.push_back(node);
	}
	is_leaf = false;
	return true;
}

bool ScoreTestPriorityMonteCarloBase::GameTreeNode::ExpandTree(double coeff, ScoreMethod method)
{
	if (is_leaf)
		ExpandTree();
	variety_score[method].is_leaf = false;
	for (auto node= children.begin(); node != children.end(); node++)
	{
		node->PlayOutAndUpdate(coeff, method);
	}
	UpdateCurrentScore(method);
	return true;
}

void ScoreTestPriorityMonteCarloBase::GameTreeNode::PlayOutAndUpdate(double coeff, ScoreMethod method)
{
	if (variety_score[method].is_leaf)
	{
		variety_score[method].times++;
		if (variety_score[method].times > playout_results.size())
		{
			PlayOutResult result;
			result.score = CalcScore(PlayOut(b, rest, is_my_turn));
			result.score_total = result.score + 
				(playout_results.size() == 0 ? 0 : playout_results.back().score_total);
			playout_results.push_back(result);
		}
		variety_score[method].total_times = variety_score[method].times;
		variety_score[method].total_score = playout_results[variety_score[method].times - 1].score_total / (double)variety_score[method].times;
		return;
	}
	// not leaf
	double coeff_score = coeff * sqrt(2 * log(variety_score[method].total_times));
	double sign_score = is_my_turn ? 1.0 : -1.0;
	auto node = children.begin();

	double best = sign_score * node->variety_score[method].total_score + coeff_score / sqrt(node->variety_score[method].total_times);
	GameTreeNode* bestnode = &node[0];
	for (node++; node != children.end(); node++)
	{
		double score = sign_score * node->variety_score[method].total_score + coeff_score / sqrt(node->variety_score[method].total_times);
		if(score > best)
		{
			best = score;
			bestnode = &node[0];
		}
	}
	bestnode->PlayOutAndUpdate(coeff, method);
	UpdateCurrentScore(method);
}

void ScoreTestPriorityMonteCarloBase::GameTreeNode::PlayOutAndUpdateWithExpansion(double coeff, int num_to_expand, ScoreMethod method)
{
	if (variety_score[method].is_leaf)
	{
		variety_score[method].times++;
		if (variety_score[method].times > playout_results.size())
		{
			PlayOutResult result;
			result.score = CalcScore(PlayOut(b, rest, is_my_turn));
			result.score_total = result.score +
				(playout_results.size() == 0 ? 0 : playout_results.back().score_total);
			playout_results.push_back(result);
		}
		if ((variety_score[method].times >= num_to_expand)&& !rest.empty())
		{
			if (ExpandTree(coeff, method))
				return;
		}
		variety_score[method].total_times = variety_score[method].times;
		variety_score[method].total_score = playout_results[variety_score[method].times -1 ].score_total / (double)variety_score[method].times;
		return;
	}
	// not leaf
	double coeff_score = coeff * sqrt(2 * log(variety_score[method].total_times));
	double sign_score = is_my_turn ? 1.0 : -1.0;
	auto node = children.begin();

	double best = sign_score * node->variety_score[method].total_score + coeff_score / sqrt(node->variety_score[method].total_times);
	GameTreeNode* bestnode = &node[0];
	for (node++; node != children.end(); node++)
	{
		double score = sign_score * node->variety_score[method].total_score + coeff_score / sqrt(node->variety_score[method].total_times);
		if (score > best)
		{
			best = score;
			bestnode = &node[0];
		}
	}
	bestnode->PlayOutAndUpdateWithExpansion(coeff, num_to_expand, method);
	UpdateCurrentScore(method);
}

void ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScore(ScoreMethod method)
{
	typedef void (ScoreTestPriorityMonteCarloBase::GameTreeNode::*MemFuncs)();
	static MemFuncs funcs[] =
	{
		&ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMax,
		&ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxTotalAve,
		&ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMaxSubSD,
		&ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMaxSubGroupSD,
		0
	};
	(this->*funcs[method])();
}

void ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMax()
{
	static const ScoreMethod method = SCORE_METHOD_AVE_MAX;
	int local_times = variety_score[method].times;
	int local_score_total = (local_times == 0 ? 0 : playout_results[local_times - 1].score_total);

	variety_score[method].subtree_times = 0;
	variety_score[method].subtree_score = is_my_turn ? CalcScore(-64) : CalcScore(64);
	for (auto node = children.begin(); node != children.end(); node++)
	{
		variety_score[method].subtree_times += node->variety_score[method].total_times;
		if (is_my_turn)
			variety_score[method].subtree_score = std::max(variety_score[method].subtree_score, node->variety_score[method].total_score);
		else
			variety_score[method].subtree_score = std::min(variety_score[method].subtree_score, node->variety_score[method].total_score);
	}
	variety_score[method].total_times = variety_score[method].subtree_times + local_times;
	variety_score[method].total_score = local_score_total / (double)variety_score[method].total_times + variety_score[method].subtree_score * variety_score[method].subtree_times / (double)variety_score[method].total_times;
}
void ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxTotalAve()
{
	static const ScoreMethod method = SCORE_METHOD_TOTAL_AVE;
	int local_times = variety_score[method].times;
	int local_score_total = (local_times == 0 ? 0 : playout_results[local_times - 1].score_total);

	variety_score[method].subtree_times = 0;
	double subtree_total = 0;
	for (auto node = children.begin(); node != children.end(); node++)
	{
		variety_score[method].subtree_times += node->variety_score[method].total_times;
		subtree_total += node->variety_score[method].total_score * node->variety_score[method].total_times;
	}
	variety_score[method].subtree_score = subtree_total/ variety_score[method].subtree_times;
	variety_score[method].total_times = variety_score[method].subtree_times + local_times;
	variety_score[method].total_score = (local_score_total + subtree_total) / (double)variety_score[method].total_times;
}
void ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMaxSubSD()
{
	static const ScoreMethod method = SCORE_METHOD_AVE_MAX_SD;
	int local_times = variety_score[method].times;
	int local_score_total = (local_times == 0 ? 0 : playout_results[local_times - 1].score_total);

	variety_score[method].subtree_times = 0;
	variety_score[method].subtree_score = is_my_turn ? CalcScore(-64) : -CalcScore(64);
	double coeff = is_my_turn ? 1.0 : -1.0;
	for (auto node = children.begin(); node != children.end(); node++)
	{
		variety_score[method].subtree_times += node->variety_score[method].total_times;
		variety_score[method].subtree_score = std::max(
			variety_score[method].subtree_score
			, coeff * node->variety_score[method].total_score
			- 1.0 / sqrt(node->variety_score[method].total_times)
		);
	}
	variety_score[method].subtree_score *= coeff;
	variety_score[method].total_times = variety_score[method].subtree_times + local_times;
	variety_score[method].total_score = local_score_total / (double)variety_score[method].total_times + variety_score[method].subtree_score * variety_score[method].subtree_times / (double)variety_score[method].total_times;
}
void ScoreTestPriorityMonteCarloBase::GameTreeNode::UpdateCurrentScoreAuxAverageMaxSubGroupSD()
{
	static const ScoreMethod method = SCORE_METHOD_AVE_MAX_GSD;
	int local_times = variety_score[method].times;
	int local_score_total = (local_times == 0 ? 0 : playout_results[local_times - 1].score_total);

	variety_score[method].subtree_times = 0;
	variety_score[method].subtree_score = is_my_turn ? CalcScore(-64) : -CalcScore(64);
	double coeff = is_my_turn ? 1.0 : -1.0;

	std::vector<int> idx;
	for (int i = 0; i < children.size(); i++)
		idx.push_back(i);
	std::sort(idx.begin(), idx.end(), [=](int a, int b) {return coeff * children[a].variety_score[method].total_score > coeff * children[b].variety_score[method].total_score; });
	
	double total = 0.0;
	for (auto i= idx.begin();i!= idx.end(); i++)
	{
		variety_score[method].subtree_times += children[*i].variety_score[method].total_times;
		total += children[*i].variety_score[method].total_score * children[*i].variety_score[method].total_times;
		variety_score[method].subtree_score = std::max(
			variety_score[method].subtree_score
			, coeff * total / variety_score[method].subtree_times
			- 1.0 / sqrt(variety_score[method].subtree_times)
		);
	}
	variety_score[method].subtree_score *= coeff;
	variety_score[method].total_times = variety_score[method].subtree_times + local_times;
	variety_score[method].total_score = local_score_total / (double)variety_score[method].total_times + variety_score[method].subtree_score * variety_score[method].subtree_times / (double)variety_score[method].total_times;
}


const char* ScoreTestPriorityMonteCarloBase::GameTreeNode::GetMethodName(ScoreMethod m)
{
	static const char name[][5]=
	{
		"MAX",
		"AVE",
		"SMAX",
		"GSMX",
		0
	};
	return name[m];
}

void ScoreTestPriorityMonteCarloBase::GameTreeNode::DebugPrint(FILE* f, int indent)
{
	static const char space[] = "                                              ";
	static const int  s_len = sizeof(space) - 1;
	fprintf(f, "%s%c%d: %c%c :\n", space + s_len - indent, is_my_turn ? 'M' : 'O', indent,
		'a' + m.GetX(), '1' + m.GetY());
	for (int m = 0; m < SCORE_METHOD_END; m++)
	{
		Reversi::Move m_best(0,0);
		if (!variety_score[m].is_leaf)
		{
			auto best = children.begin();
			auto i = best;
			for (i++; i != children.end(); i++)
			{
				if (is_my_turn)
				{
					if (best->variety_score[m].total_score < i->variety_score[m].total_score)
						best = i;
				}
				else
					if (best->variety_score[m].total_score > i->variety_score[m].total_score)
						best = i;
			}
			m_best = best->m;
		}
		int local_times = variety_score[m].times;
		int local_score = (local_times <= 0 ? 0 : playout_results[local_times - 1].score_total);
		fprintf(f, "%s %5s[%c]:best[%c%c]:%7.4f (%5d times, node [%4d / %4d] subtree [ ave %7.4f (%d)]) \n"
			, space + s_len - indent, GetMethodName(ScoreMethod(m)), variety_score[m].is_leaf ? 'L' : 'N'
			, variety_score[m].is_leaf ? '-' : ('a' + m_best.GetX())
			, variety_score[m].is_leaf ? '-' : ('1' + m_best.GetY())
			, variety_score[m].total_score, variety_score[m].total_times, local_score, local_times, variety_score[m].subtree_score, variety_score[m].subtree_times);
		if (!variety_score[m].is_leaf)
		{
			
			for (auto i = children.begin(); i != children.end(); i++)
			{
				fprintf(f, "%s:    [%c%c]:%7.4f (%5d times)\n"
					, space + s_len - indent - 9
					, 'a' + i->m.GetX(), '1' + i->m.GetY()
					, i->variety_score[m].total_score, i->variety_score[m].total_times);
			}
		}
	}
	PrintBoard(b, f, indent);
	for (auto i = children.begin(); i != children.end(); i++)
	{
		i->DebugPrint(f, indent + 1);
	}
}

