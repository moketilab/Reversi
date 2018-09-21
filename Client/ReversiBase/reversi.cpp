
#include "reversi.h"

#if (_MSC_VER >= 1900)
#define CAN_USE_INTRINSICS
#include <intrin.h>
#endif


namespace {
	// private support functions
inline uint64_t GetBit(Reversi::Move m)
{
	return uint64_t(1) << m.GetXY();
}

#ifdef CAN_USE_INTRINSICS
inline int BitCount(uint64_t x)
{
	return int(__popcnt64(x));
}
#else
inline int BitCount(uint64_t x)
{
	const uint64_t const3 = 0x3333333333333333ull;
	const uint64_t const0F = 0x0f0f0f0f0f0f0f0full;
	uint64_t a = (x - (x >> 1));
	uint64_t b = (a & const3) + ((a >> 2)& const3);
	uint64_t c = b + (b >> 4);
	uint64_t d = (c & const0F) + ((c >> 4) & const0F);
	int ret = d % 0xFFu;
	return ret;
}
#endif
}

namespace Reversi{
	void Board::Reset()
	{
		stone = player = 0;
	}
	void Board::SetPlayer(Move m)
	{
		uint64_t x = GetBit(m);
		stone |= x;
		player &= ~x;
	}
	void Board::SetOpponent(Move m)
	{
		uint64_t x = GetBit(m);
		stone |= x;
		player |= x;
	}
	bool Board::MoveMine(Move m)
	{
		return MoveBoard(GetBit(m), PID_MINE, m.GetX(), m.GetY());
	}
	bool Board::MoveOpponent(Move m)
	{
		return MoveBoard(GetBit(m), PID_OPPONENT, m.GetX(), m.GetY());
	}
	bool Board::CanSet(Move m) const
	{
		return (stone & GetBit(m)) == 0;
	}
	bool Board::CanMoveMine(Move m) const
	{
		return CanMoveBoard(GetBit(m), PID_MINE, m.GetX(), m.GetY());
	}
	bool Board::CanMoveOpponent(Move m) const
	{
		return CanMoveBoard(GetBit(m), PID_OPPONENT, m.GetX(), m.GetY());
	}
	bool Board::MoveBoard(uint64_t base, uint64_t pid, int x, int y)
	{
		if (base & stone)
			return false;
		uint64_t changes = 0;
		changes |= MoveLine(base, pid, 8, 6 - y);
		changes |= MoveLine(base, pid, 1, 6 - x);
		changes |= MoveLine(base, pid, -1, x - 1);
		changes |= MoveLine(base, pid, -8, y - 1);
		changes |= MoveLine(base, pid, 9, std::min(6 - x, 6 - y));
		changes |= MoveLine(base, pid, 7, std::min(x - 1, 6 - y));
		changes |= MoveLine(base, pid, -7, std::min(6 - x, y - 1));
		changes |= MoveLine(base, pid, -9, std::min(x - 1, y - 1));
		if (changes)
		{
			player ^= changes;
			stone |= base;
			player |= (base & pid);
			return true;
		}
		return false;
	}
	bool Board::CanMoveBoard(uint64_t base, uint64_t pid, int x, int y) const
	{
		if (base & stone)
			return false;
		return MoveLine(base, pid, 8, 6 - y)
			|| MoveLine(base, pid, 1, 6 - x)
			|| MoveLine(base, pid, -1, x - 1)
			|| MoveLine(base, pid, -8, y - 1)
			|| MoveLine(base, pid, 9, std::min(6 - x, 6 - y))
			|| MoveLine(base, pid, 7, std::min(x - 1, 6 - y))
			|| MoveLine(base, pid, -7, std::min(6 - x, y - 1))
			|| MoveLine(base, pid, -9, std::min(x - 1, y - 1));
	}
	int  Board::Score() const
	{
		int ps = BitCount(stone & ~player);
		int os = BitCount(stone & player);
		return ps - os;
	}

	uint64_t Board::MoveLine(uint64_t base, uint64_t pid, int shift, int loop) const
	{
		if (loop < 1)
			return 0;
		uint64_t check = (shift > 0) ? (base << shift) : (base >> -shift);
		if ((stone&check) == 0 || (player&check) == (pid & check))
			return 0;
		uint64_t changes = check;
		for (int i = 0; i < loop; i++)
		{
			check = (shift > 0) ? (check << shift) : (check >> -shift);
			if ((stone&check) == 0)
				return 0;
			if ((player&check) == (pid & check))
			{
				return changes;
			}
			changes |= check;
		}
		return 0;
	}

	//DebugFunctions
	void PrintBoard(Board b, FILE* f, int indent)
	{
		char space[] = "                                                        ";
		int  s_len = sizeof(space) - 1;
		uint64_t tgt = 1;
		fprintf(f, "%s  :ABCDEFGH\n", space + s_len - indent);
		for (int y = 0; y < 8; y++)
		{
			std::string line;
			for (int x = 0; x < 8; x++)
			{
				if (tgt & b.stone)
					if (tgt & b.player)
						line += "x";
					else
						line += "o";
				else
					if (tgt & b.player)
						line += "&";
					else
						line += ".";
				tgt <<= 1;
			}
			fprintf(f, "%s %d:%s\n", space + s_len - indent, y + 1, line.c_str());
		}
	}
}