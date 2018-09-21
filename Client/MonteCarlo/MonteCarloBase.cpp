
#include "MonteCarloBase.h"


const MonteCarloBase::VecMove& MonteCarloBase::ResetRest(const Reversi::Board& b)
{
	rest.clear();
	for (int y = 0; y<8; y++)
		for (int x = 0; x < 8; x++)
		{
			Reversi::Move m(x, y);
			if (b.CanSet(m))
				rest.push_back(m);
		}
	return rest;
}
const MonteCarloBase::VecMove& MonteCarloBase::UpdateRest(const VecMove& moves)
{
	for (auto m = moves.begin(); m != moves.end(); m++)
		rest.erase(std::find(rest.begin(), rest.end(), *m));
	return rest;
}
MonteCarloBase::VecMove MonteCarloBase::FindMoveList(Reversi::Board b, const VecMove& moves)
{
	VecMove ret;
	for (auto m = moves.begin(); m != moves.end(); m++)
		if (b.CanMoveMine(*m))
			ret.push_back(*m);
	return ret;
}
MonteCarloBase::VecMove MonteCarloBase::FindMoveList(Reversi::Board b, const VecMove& moves, bool is_my_move)
{
	VecMove ret;
	for (auto m = moves.begin(); m != moves.end(); m++)
		if (b.CanMovePlayer(*m, is_my_move))
			ret.push_back(*m);
	return ret;
}

int MonteCarloBase::PlayOut(Reversi::Board b, VecMove rest, bool playerNext)
{
	bool pass = false;
	auto start = rest.begin();
	auto end = rest.end();
	auto now = start;
	if (playerNext)
		goto PlayerTurn;
	while (1)
	{
		//OpponentTurn:
		now = start;
		while (now != end)
		{
			int idx = Random() % (now - end);
			std::swap(now[0], now[idx]);
			if (b.MoveOpponent(now[0]))
			{
				std::swap(start[0], now[0]);
				start++;
				pass = false;
				break;
			}
			now++;
		}
		if (now == end)
		{
			if (pass)
				return b.Score();
			pass = true;
		}
		PlayerTurn:
		now = start;
		while (now != end)
		{
			int idx = Random() % (now - end);
			std::swap(now[0], now[idx]);
			if (b.MoveMine(now[0]))
			{
				std::swap(start[0], now[0]);
				start++;
				pass = false;
				break;
			}
			now++;
		}
		if (now == end)
		{
			if (pass)
				return b.Score();
			pass = true;
		}
	}
}
