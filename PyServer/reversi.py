
import copy
import Reversi


FIRST = 1
SECOND = 2

def Enemy(p):
    return SECOND if (p == FIRST) else FIRST

class Board(object):
    """ Board status for Reversi """
    NONE = 0
    FIRST = FIRST
    SECOND = SECOND
    DIR = ((0,1),(0,-1),(1,0),(-1,0),(1,1),(1,-1),(-1,1),(-1,-1))
    def __init__(self, *args, **kwargs):
        self._BoardInit();
        return super().__init__(*args, **kwargs)
    #### Public
    def Move(self, move):
        return self._Move(self.board, move.GetX(), move.GetY(), move.Me(), move.Enemy())
    def MoveCheck(self, move):
        return self._Move(self.board, move.GetX(), move.GetY(), move.Me(), move.Enemy(), True)
    def GenMoves(self, player):
        """ retrun list of Move """
        moves = [Move(player,x,y) for x in range(8) for y in range(8)]
        return [x for x in moves if self.MoveCheck(x)]
    def CanMove(self, player):
        ms = self.GenMoves(player)
        return len(ms) > 0
    def IsGameEnd(self):
        return not (self.CanMove(FIRST) or self.CanMove(SECOND))
    def Score(self):
        fs = len([self.board[y][x] for x in range(8) for y in range(8) if self.board[y][x] == Board.FIRST])
        ss = len([self.board[y][x] for x in range(8) for y in range(8) if self.board[y][x] == Board.SECOND])
        return [fs,ss]
        
    #### Public (debug)
    def PrintBoard(self):
        print(" abcdefgh")
        i = 0;
        for line in self.board:
            str ="12345678"[i]
            i+=1
            for cell in line:
                if cell == Board.NONE:
                    str += '.'
                elif cell == Board.FIRST:
                    str += 'o'
                elif cell == Board.SECOND:
                    str += 'x'
                else:
                    str += '_'
            print(str)
    #### Private
    def _BoardInit(self):
        self.board = [[Board.NONE for x in range(8)] for y in range(8)];
        self.board[3][4] = Board.FIRST
        self.board[4][3] = Board.FIRST
        self.board[3][3] = Board.SECOND
        self.board[4][4] = Board.SECOND
    def _Move(self,b,x,y,p,e,c=False):
        rv = False
        if(b[y][x]!=Board.NONE):
            return rv
        if(c):
            for d in Board.DIR:
                rv = rv or self._MoveLine(b,x,y,d[0],d[1],p,e,c)
        else:
            for d in Board.DIR:
                rv = self._MoveLine(b,x,y,d[0],d[1],p,e,c) or rv
            if(rv):
                b[y][x]=p
        return rv
    def _MoveLine(self,b,x,y,dx,dy,p,e,c=False):
        if(not c):
            bs = copy.deepcopy(b)
        else:
            bs = b
        x += dx; y+= dy;

        if(not self._RangeCheck(x,y)):
            return False
        if(bs[y][x] != e):
            return False
        if(not c):
            bs[y][x] = p 
        x += dx; y+= dy;
        if(not self._RangeCheck(x,y)):
            return False
        while(bs[y][x] == e):
            if(not c):
                bs[y][x] = p
            x += dx; y+= dy;
            if(not self._RangeCheck(x,y)):
                return False
        if(bs[y][x] == p):
            if(not c):
                b[:] = bs[:]
            return True
        return False
    def _RangeCheck(self,x,y):
        return ((x in range(8)) and (y in range(8)))
            


class Move(object):
    """ Move for Reversi """
    FIRST  = FIRST
    SECOND = SECOND
    def __init__(self, p, x, y, *args, **kwargs):
        self.player = p
        self.x = x
        self.y = y
        return super().__init__(*args, **kwargs)
    def __str__(self):
        if(self.player == Move.FIRST):
            rv = "ABCDEFGH"[self.x]+"12345678"[self.y]
        elif(self.player == Move.SECOND):
            rv = "abcdefgh"[self.x]+"12345678"[self.y]
        else:
            rv = "ERROR"
        return rv
    def GetX(self):
        return self.x
    def GetY(self):
        return self.y
    def Me(self):
        return self.player
    def Enemy(self):
        return Enemy(self.player)

class PlayerBase(object):
    """ Base for Player """
    def __init__(self, name):
        self.name = name
    def GetName(self):
        return self.name
    def Reset(self, id, board):
        self.id = id
    def GetID(self):
        return self.id


class Game(object):
    """ Reversi game  """
    def __init__(self, player1, player2, *args, **kwargs):
        self.players = (player1, player2)
        return super().__init__(*args, **kwargs)
    def GamePlay(self, **kwargs):
        """ 
        kw 'DebugPrintAllBoard'  : print board after all moves.
        kw 'DebugPrintFinalBoard': print board after game.
        """
        board = Board()
        current = 0
        players = [[FIRST, self.players[0]],[SECOND, self.players[1]]]
        for p in players:
            p[1].Reset(p[0],board)
        moves = []
        while(not board.IsGameEnd()):
            move = players[current][1].GetMove(copy.deepcopy(board), moves)
            if(not board.Move(move)):
                return self.ErrorMove(players[1-current][1], board, move, **kwargs)
            if('DebugPrintAllBoard' in kwargs.keys()):
                board.PrintBoard()
            moves = [move]
            while(not (board.IsGameEnd() or board.CanMove(players[1-current][0]))):
                move = players[current][1].GetMove(copy.deepcopy(board), [])
                if(not board.Move(move)):
                    return self.ErrorMove(players[1-current][1], board, move, **kwargs)
                if('DebugPrintAllBoard' in kwargs.keys()):
                    board.PrintBoard()
                moves += [move]
            current = 1 - current
        if('DebugPrintFinalBoard' in kwargs.keys()):
            board.PrintBoard()
        return board.Score()
    def ErrorMove(self, player, board, move, **kwargs):
        if('ErrorPrint'):
            print("Error:")
            board.PrintBoard()
            print(move)
        stat = board.Score()
        if(move.Player()==Move.FIRST):
            stat[0] = -1
        else:
            stat[1] = -1
        return stat






        

        





    
        


