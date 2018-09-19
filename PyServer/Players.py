
import sys, random, copy
import Reversi


PlayerTable={
    "random"        : (lambda *args,**kwargs: RandomPlayer(*args,**kwargs))
    ,"user"         : (lambda *args,**kwargs: UserPlayer(*args,**kwargs))
    ,"simplemonte"  : (lambda *args,**kwargs: SimpleMonteCarloMethodPlayer(*args,**kwargs))
    ,"stdsvr"       : (lambda *args,**kwargs: StdioServerPlayer(*args,**kwargs))
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


import subprocess

class StdioServerPlayer(Reversi.PlayerBase):
    def __init__(self, cmd, *args):
        self.process = subprocess.Popen(
            [cmd, *args],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            bufsize=1,
            universal_newlines=True)
        self._CmdSend("MTWN:")
        name = self._CmdRecieve("MTNM")
        return super().__init__(name)
    def Close(self):
        self._CmdSend("MTTM:")
        self.process.wait()
    def Reset(self, id, board:Reversi.Board):
        super().Reset(id, board)
        self._CmdSend("GMRT:")
        self._CmdRecieve("GMOK")
        enemy = Reversi.Enemy(self.id)
        for x in range(8):
            for y in range(8):
                if(board.board[y][x] == self.id):
                    pos = "abcdefgh"[x]+"12345678"[y]
                    self._CmdSend("GMSP:{}".format(pos))
                elif(board.board[y][x] == enemy):
                    pos = "abcdefgh"[x]+"12345678"[y]
                    self._CmdSend("GMSE:{}".format(pos))
        self._CmdSend("GMST:")
        self._CmdRecieve("GMOK")
    def GetMove(self, board, moves):
        for m in moves:
            pos = "abcdefgh"[m.GetX()]+"12345678"[m.GetY()]
            self._CmdSend("GMME:{}".format(pos))
        self._CmdSend("GMRM:")
        
        pos = self._CmdRecieve("GMMV")
        x = "abcdefgh".find(pos[0])
        y = "12345678".find(pos[1])
        return Reversi.Move(self.id,x,y)
    def GameSet(self, score, moves):
        for m in moves:
            pos = "abcdefgh"[m.GetX()]+"12345678"[m.GetY()]
            self._CmdSend("GMME:{}".format(pos))

        if(self.id == Reversi.FIRST):
            self._CmdSend("GMGS:{0}-{1}".format(score[0],score[1]))
        else:
            self._CmdSend("GMGS:{1}-{0}".format(score[0],score[1]))
        self._CmdRecieve("GMOK")

    def _CmdSend(self, cmd:str):
        print(cmd, file = self.process.stdin)
        self.process.stdin.flush()
    def _CmdRecieve(self, cmd: str):
        while(1):
            s = self.process.stdout.readline().rstrip()
            cs = s.split(':')
            if(cs[0] == cmd):
                return cs[1]
            elif(cs[0] == "DBPT"):
                print(cs[1])
            else:
                raise SystemError("SubProcess command Error")



        