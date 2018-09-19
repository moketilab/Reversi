
"""
   Server for Reversi
"""

import Reversi, Players
import argparse

ThisPlayerList = Players.PlayerTable

argp = argparse.ArgumentParser(description="Reversi")
argp.add_argument("-p", nargs="+",action='append',required=True)
argp.add_argument("-l", type=int, default=1)

args = argp.parse_args()

try:
    players = [ThisPlayerList[x[0]](*x[1:]) for x in args.p]
except:
    print("Can't create Players")
    exit()

if(len(players) == 1):
    players += [ThisPlayerList[x[0]](*x[1:]) for x in args.p]
WinLoseRecord=[[[0,0,0] for y in players] for x in players] # [1st win, 2nd win, draw]

for nnn in range(args.l):
    for n1 in range(len(players)):
        p1 = players[n1]
        for n2 in range(len(players)):
            p2 = players[n2]
            if p1 is p2:
                continue
            print("[{0}] vs [{1}]".format(p1.GetName(),p2.GetName()))
            game = Reversi.Game(p1, p2)
            result = game.GamePlay(DebugPrintFinalBoard=1)
            print(result)
            if (result[0]>result[1]):
                print("{} win".format(p1.GetName()))
                WinLoseRecord[n1][n2][0]+=1
            elif (result[0]<result[1]):
                print("{} win".format(p2.GetName()))
                WinLoseRecord[n1][n2][1]+=1
            else:
                print("draw")
                WinLoseRecord[n1][n2][2]+=1

print()
print("Result:")
for n in range(len(players)):
    score=[0 for x in range(9)]
    for v in range(len(players)):
        score[0] += WinLoseRecord[n][v][0] + WinLoseRecord[v][n][1]
        score[3] += WinLoseRecord[n][v][0] 
        score[6] += WinLoseRecord[v][n][1]
        score[1] += WinLoseRecord[n][v][1] + WinLoseRecord[v][n][0]
        score[4] += WinLoseRecord[n][v][1] 
        score[7] += WinLoseRecord[v][n][0]
        score[2] += WinLoseRecord[n][v][2] + WinLoseRecord[v][n][2]
        score[5] += WinLoseRecord[n][v][2] 
        score[8] += WinLoseRecord[v][n][2]
    print("  {0}: {1}-{2}-{3} (1st:{4}-{5}-{6}, 2nd:{7}-{8}-{9}) ".format(players[n].GetName(),*score))
    for v in range(n, len(players)):
        if(n == v):
            continue
        print("    vs {0}:  {1}-{2}-{3} (1st:{4}-{5}-{6}, 2nd:{8}-{7}-{9})"
              .format(players[v].GetName()
                      ,WinLoseRecord[n][v][0] + WinLoseRecord[v][n][1]
                      ,WinLoseRecord[n][v][1] + WinLoseRecord[v][n][0]
                      ,WinLoseRecord[n][v][2] + WinLoseRecord[v][n][2]
                      ,*WinLoseRecord[n][v]
                      ,*WinLoseRecord[v][n]))
    print()

for p in players:
    p.Close()
    



