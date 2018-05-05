
import sys, random, copy
import Reversi


PlayerTable={
    "random"        : (lambda *args,**kwargs: RandomPlayer(*args,**kwargs))
    ,"user"         : (lambda *args,**kwargs: UserPlayer(*args,**kwargs))
    ,"simplemonte"  : (lambda *args,**kwargs: SimpleMonteCarloMethodPlayer(*args,**kwargs))
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

class SimpleMonteCarloMethodPlayer(Reversi.PlayerBase):
    def __init__(self, sample_num= "100"):
        name = "SimpleMonteCalroMethodPlayer({})".format(sample_num)
        self.sample_num = int(sample_num)
        return super().__init__(name)
    def GetMove(self, board, _moves):
        moves = board.GenMoves(self.id)
        challenge = self.sample_num // len(moves) + 1
        best_score = -1.0
        best_move  = None
        for move in moves:
            bcopy = copy.deepcopy(board)
            bcopy.Move(move)
            score = self._PlayOutNTimes(bcopy,challenge)
            if(score > best_score):
                best_score = score
                best_move  = move
        return best_move
    def _PlayOutNTimes(self, board:Reversi.Board, n:int):
        win = 0; lose = 0; draw = 0;
        for i in range(n):
            score = self._PlayOut(board)
            if(score[0] > score[1]):
                win += 1
            elif(score[0] == score[1]):
                draw += 1
            else:
                lose += 1
        return float(win)/(win + lose + draw);
    def _PlayOut(self, board:Reversi.Board):
        b = copy.deepcopy(board)
        p = Reversi.Enemy(self.id)
        while(not b.IsGameEnd()):
            moves = b.GenMoves(p)
            if(moves):
                b.Move(moves[random.randrange(len(moves))])
            p = Reversi.Enemy(p)
        score = b.Score()
        if(self.id == Reversi.FIRST):
            return score;
        return [score[1], score[0]]
     
        

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

