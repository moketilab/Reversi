#pragma once

#include "MonteCarloBase.h"



class ScoreTestPriorityMonteCarloBase
	:public MonteCarloBase
{
public:
	enum ScoreMethod{
		SCORE_METHOD_AVE_MAX,
		SCORE_METHOD_TOTAL_AVE,
		SCORE_METHOD_AVE_MAX_SD,
		SCORE_METHOD_AVE_MAX_GSD,
		SCORE_METHOD_END
	};
protected:
	class GameTreeNode
	{
		struct PlayOutResult
		{
			int score;
			int score_total;
		};
		const Reversi::Move       m;
		const Reversi::Board      b;
		const VecMove             rest;
		const bool                is_my_turn;
		bool                      is_leaf;
		std::vector<PlayOutResult> playout_results;
//		int                       times;
//		int                       score;
		struct Score
		{
			bool                  is_leaf;
			int                   times;
			int                   subtree_times;
			double                subtree_score;
			int                   total_times;
			double                total_score;
		}variety_score[SCORE_METHOD_END];
		std::vector<GameTreeNode> children;
	public:
		// constructor
		GameTreeNode(Reversi::Move _m, Reversi::Board _b, VecMove _rest, bool _is_my_turn)
			:m(_m), b(_b), rest(_rest), is_my_turn(_is_my_turn)
		{}
	public:
		Reversi::Move PlayOutN(int n, double coeff, ScoreMethod method = SCORE_METHOD_AVE_MAX);
		Reversi::Move PlayOutNWithExpansion(int n, double coeff, int num_to_expand, ScoreMethod method = SCORE_METHOD_AVE_MAX, FILE* fd=0);
		// build tree
		static GameTreeNode BuildTree(Reversi::Board board, VecMove rest, int depth);
	private:
		// build tree helper
		static GameTreeNode BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn);
		static GameTreeNode CreateNode(Reversi::Move m, Reversi::Board b, VecMove rest, bool my_turn);
		bool ExpandTree();
		bool ExpandTree(double coeff, ScoreMethod method);
		void PlayOutAndUpdate(double coeff, ScoreMethod method);
		void PlayOutAndUpdateWithExpansion(double coeff, int num_to_expand, ScoreMethod method);
		void UpdateCurrentScore(ScoreMethod method);
		void UpdateCurrentScoreAuxAverageMax();
		void UpdateCurrentScoreAuxTotalAve();
		void UpdateCurrentScoreAuxAverageMaxSubSD();
		void UpdateCurrentScoreAuxAverageMaxSubGroupSD();
		const char* GetMethodName(ScoreMethod m);
	public:
		void DebugPrint(FILE* f, int indent = 0);
	};
protected:
	ScoreTestPriorityMonteCarloBase(const std::string& name):MonteCarloBase(name) {}
};
