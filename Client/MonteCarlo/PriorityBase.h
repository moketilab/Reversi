#pragma once

#include "MonteCarloBase.h"



class PriorityMonteCarloBase
	:public MonteCarloBase
{
public:
	enum ScoreMethod {
		SCORE_METHOD_AVE_MAX,
		SCORE_METHOD_TOTAL_AVE,
		SCORE_METHOD_AVE_MAX_SD,
		SCORE_METHOD_AVE_MAX_GSD,
		SCORE_METHOD_END,
		SCORE_METHOD_DEFAULT = SCORE_METHOD_AVE_MAX
	};
protected:
	class GameTreeNode
	{
		const Reversi::Move       m;
		const Reversi::Board      b;
		const VecMove             rest;
		const bool                is_my_turn;
		bool                      is_leaf;
		int                       times;
		int                       score;
		int                       subtree_times;
		double                    subtree_score;
		int                       total_times;
		double                    total_score;
		ScoreMethod               score_method;
		std::vector<GameTreeNode> children;
	private:
		static void (PriorityMonteCarloBase::GameTreeNode::*socre_funcs[SCORE_METHOD_END])();
	public:
		// constructor
		GameTreeNode(Reversi::Move _m, Reversi::Board _b, VecMove _rest, bool _is_my_turn, ScoreMethod _score_method)
			:m(_m), b(_b), rest(_rest), is_my_turn(_is_my_turn)
			, times(0), score(0), subtree_times(0), subtree_score(0.0), total_times(0), total_score(0.0), score_method(_score_method)
		{}
	public:
		Reversi::Move PlayOutN(int n, double coeff);
		Reversi::Move PlayOutNWithExpansion(int n, double coeff, int num_to_expand);
		// build tree
		static GameTreeNode BuildTree(Reversi::Board board, VecMove rest, int depth, ScoreMethod score_method);
	private:
		// build tree helper
		static GameTreeNode BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn, ScoreMethod score_method);
		bool ExpandTree();
		void PlayOutAndUpdate(double coeff);
		void PlayOutAndUpdateWithExpansion(double coeff, int num_to_expand);
		void UpdateCurrentScore()
		{
			(this->*socre_funcs[score_method])();
		}
	private: //scoring method functions
		void UpdateCurrentScoreAuxAverageMax();
		void UpdateCurrentScoreAuxTotalAve();
		void UpdateCurrentScoreAuxAverageMaxSubSD();
		void UpdateCurrentScoreAuxAverageMaxSubGroupSD();

	public:
		void DebugPrint(FILE* f, int indent = 0);
	};
protected:
	PriorityMonteCarloBase(const std::string& name):MonteCarloBase(name) {}
};
