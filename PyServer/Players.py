
import sys, random
import Reversi

class RandomPlayer(Reversi.PlayerBase):
    def GetName(self):
        return "RandomPlayer{}".format(self.id);
    def GetMove(self, board, moves):
        ms = board.GenMoves(self.id)
        m = ms[random.randrange(len(ms))]
        return m;

class UserPlayer(Reversi.PlayerBase):
    def GetName(self):
        return "RandomPlayer{}".format(self.id);
    def GetMove(self, board, moves):
        print("Enemy Move: {}".format([str(m) for m in moves]))
        board.PrintBoard()
        while(1):
            move_string = input("your move ?>")
            try:
                x = "abcdefgh".find(move_string[0])
                y = "12345678".find(move_string[1])
            except:
                continue
            if(not x in range(8)):
                print("Error in X [{}]".format(x))
                continue
            if(not y in range(8)):
                print("Error in Y [{}]".format(y))
                continue
            m = Reversi.Move(self.id, x, y)
            if(board.MoveCheck(m)):
                return m
            print("Can't Move [{}]".format(m))

