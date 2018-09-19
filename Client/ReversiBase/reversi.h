#pragma once

#include <stdio.h>
#include <string>
#include <algorithm>

namespace Reversi {
	struct Move {
		unsigned char mv;
		Move(int x, int y) { Set(x, y); }
		Move(const Move& m) :mv(m.mv) {}
		void Set(int x, int y) { mv = (x & 7) | ((y & 7) << 3); }
		int  GetX() { return mv & 7; }
		int  GetY() { return (mv >> 3) & 7; }
		int  GetXY() { return mv & 63; }
		bool operator == (const Move& m)const { return mv == m.mv; }
	};
	struct Board {
		uint64_t stone;  // 1: set      0: unset
		uint64_t player; // 1: opponent 0: mine
		static const uint64_t PID_MINE = 0;
		static const uint64_t PID_OPPONENT = ~0ULL;
	public:
		void Reset();
		void SetPlayer(Move m);
		void SetOpponent(Move m);
		bool MoveMine(Move m);
		bool MoveOpponent(Move m);
		bool MovePlayer(Move m, bool is_my_move)
		{
			return is_my_move ? MoveMine(m) : MoveOpponent(m);
		}
		bool CanSet(Move m) const;
		bool CanMoveMine(Move m) const;
		bool CanMoveOpponent(Move m) const;
		bool CanMovePlayer(Move m, bool is_my_move) const
		{
			return is_my_move ? CanMoveMine(m) : CanMoveOpponent(m);
		}
		int  Score() const;
	private:
		bool MoveBoard(uint64_t base, uint64_t pid, int x, int y);
		bool CanMoveBoard(uint64_t base, uint64_t pid, int x, int y) const;
		uint64_t MoveLine(uint64_t base, uint64_t pid, int shift, int loop) const;
	};

	//DebugFunctions
	void PrintBoard(Board b,FILE* f=stdout);

}
