
import sys, random
import Reversi


PlayerTable={
    "random": (lambda *args,**kwargs: RandomPlayer(*args,**kwargs))
    ,"user" : (lambda *args,**kwargs: UserPlayer(*args,**kwargs))
    }

class RandomPlayer(Reversi.PlayerBase):
    _count = 0
    def __init__(self, name_opt=""):
        name = "RandomPlayer";
        if( name_opt ):
            name += "({})".format(name_opt)
        else:
            if(RandomPlayer._count > 0):
                name += "({})".format(RandomPlayer._count)
            RandomPlayer._count+=1
        return super().__init__(name)
    def GetMove(self, board, moves):
        ms = board.GenMoves(self.id)
        m = ms[random.randrange(len(ms))]
        return m;

class UserPlayer(Reversi.PlayerBase):
    _count = 0
    def __init__(self, name_opt=""):
        name = "RandomPlayer";
        if( name_opt ):
            name += "({})".format(name_opt)
        else:
            if(UserPlayer._count > 0):
                name += "({})".format(UserPlayer._count)
            UserPlayer._count+=1
        return super().__init__(name)
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

