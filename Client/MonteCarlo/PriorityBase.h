#pragma once

#include "MonteCarloBase.h"



class PriorityMonteCarloBase
	:public MonteCarloBase
{
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
		double                    current_score;
		std::vector<GameTreeNode> children;
	public:
		// constructor
		GameTreeNode(Reversi::Move _m, Reversi::Board _b, VecMove _rest, bool _is_my_turn) :m(_m), b(_b), rest(_rest), is_my_turn(_is_my_turn), times(0), current_score(0) {}
	public:
		Reversi::Move PlayOutN(int n, double coeff);
		// build tree
		static GameTreeNode BuildTree(Reversi::Board board, VecMove rest, int depth);
	private:
		// build tree helper
		static GameTreeNode BuildTree(Reversi::Move m, Reversi::Board b, VecMove rest, int depth, bool my_turn);
		void PlayOutAndUpdate(double coeff);
		void UpdateCurrentScore();
	public:
		void DebugPrint(FILE* f, int indent = 0);
	};
protected:
	PriorityMonteCarloBase(const std::string& name):MonteCarloBase(name) {}
};
