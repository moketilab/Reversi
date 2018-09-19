

#include "player.h"
#include <random>

namespace {
	typedef unsigned int CommandID;
	constexpr CommandID GenCommandID(const char* s)
	{
		unsigned int rv = 0;
		if (s[0] == '\0' || s[0] == ':')
			return rv;
		rv = (unsigned char)s[0];
		if (s[1] == '\0' || s[1] == ':')
			return rv << 24;
		rv = (rv << 8) + (unsigned char)s[1];
		if (s[2] == '\0' || s[2] == ':')
			return rv << 16;
		rv = (rv << 8) + (unsigned char)s[2];
		if (s[3] == '\0' || s[3] == ':')
			return rv << 8;
		rv = (rv << 8) + (unsigned char)s[3];
		return rv;
	}
	Reversi::Move GetMoveFromCommand(const std::string& cmd)
	{
		int x, y;
		size_t i = cmd.find(':');
		for (i++; i < cmd.length(); i++)
		{
			char c = cmd[i];
			if (('a' <= c) && (c <= 'h'))
				x = c - 'a';
			else if (('A' <= c) && (c <= 'H'))
				x = c - 'A';
			else
				continue;
			c = cmd[i + 1];
			y = c - '1';
			//			printf("DBPT:Move = [%d,%d]\n", x,y);
			//			fflush(stdout);
			return Reversi::Move(x, y);
		}
		printf("DBPT:Can't find move\n");
		fflush(stdout);
		return Reversi::Move(0, 0);
	}
	void PirntBoard(Reversi::Board board)
	{
		uint64_t s = board.stone;
		uint64_t p = board.player;

		printf("DBPT:   01234567\n");
		printf("DBPT:   abcdefgh\n");
		for (int y = 0; y < 8; y++)
		{
			char c[10];
			for (int x = 0; x < 8; x++)
			{
				if (s &(1ull << x))
					if (p&(1ull << x))
						c[x + 1] = 'x';
					else
						c[x + 1] = 'o';
				else
					c[x + 1] = '.';
			}
			c[0] = '0' + y;
			c[9] = 0;
			printf("DBPT:  %s\n", c);
			fflush(stdout);
			s >>= 8;
			p >>= 8;
		}
	}
}

unsigned int PlayerBase::Random()
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt();
}

int ReversiPlayerStdioClient::StartClient()
{
	Reversi::Board board;
	board.Reset();
	std::vector<Reversi::Move> moves;
	while (1)
	{  //DBPT
		std::string buffer;
		std::getline(std::cin, buffer);
		int id = GenCommandID(buffer.c_str());
		bool b;
		switch (id)
		{
			// Meta Command
		case GenCommandID("MTWN"): // What your name
								   //				printf("DBPT:MTWN Recieved\n");
								   //				fflush(stdout);
			printf("MTNM:%s\n", player->GetPlayerName().c_str());
			fflush(stdout);
			break;
		case GenCommandID("MTTM"): // Terminate
								   //				fprintf(stderr, "Terminate called ...\n");
								   //				fflush(stderr);
			return 0;
			// Game Command
		case GenCommandID("GMRT"): // game reset
								   //				printf("DBPT:GMST Recieved\n");
								   //				fflush(stdout);
			printf("GMOK:\n");
			fflush(stdout);
			board.Reset();
			moves.clear();
			break;
		case GenCommandID("GMST"): // game start
								   //				printf("DBPT:GMST Recieved\n");
								   //				fflush(stdout);
			player->GameStart(board);
			printf("GMOK:\n");
			fflush(stdout);
			break;
		case GenCommandID("GMSP"): // set player stone (not move)
								   //				printf("DBPT:GMSP Recieved\n");
								   //				fflush(stdout);
			board.SetPlayer(GetMoveFromCommand(buffer));
			break;
		case GenCommandID("GMSE"): // set enemy stone (not move)
								   //				printf("DBPT:GMSE Recieved\n");
								   //				fflush(stdout);
			board.SetOpponent(GetMoveFromCommand(buffer));
			break;
		case GenCommandID("GMME"): // Move Enemy
								   //				printf("DBPT:GMME Recieved\n");
								   //				fflush(stdout);
		{
			Reversi::Move m = GetMoveFromCommand(buffer);
			b = board.MoveOpponent(m);
			moves.push_back(m);
			if (!b)
			{
				printf("DBPT:Move Failed\n");
				fflush(stdout);
			}
		}
			break;
		case GenCommandID("GMRM"): // Move Request
								   //				printf("DBPT:GMRM Recieved\n");
								   //				fflush(stdout);
		{
			Reversi::Move m(0, 0);
			do
			{
				m = player->GetMove(board, moves);
				b = board.MovePlayer(m);
			} while (!b);
			printf("GMMV:%c%c\n", 'a' + m.GetX(), '1' + m.GetY());
			fflush(stdout);
			moves.clear();
		}
			break;
		case GenCommandID("GMGS"): // Game Set
			player->GameSet(board, moves);
			moves.clear();
			printf("GMOK:\n");
			fflush(stdout);
			break;
		default:
			printf("DBPT: ERROR! Unknown Command [%s]\n", buffer.c_str());
			fflush(stdout);
			break;
		}
	}
	return 0;
}


