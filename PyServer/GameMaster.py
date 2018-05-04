
"""
   Server for Reversi
"""

import Reversi, Players
import argparse

ThisPlayerList = Players.PlayerTable

print("Start")

argp = argparse.ArgumentParser(description="Reversi")
argp.add_argument("-p", nargs="+",action='append')

args = argp.parse_args()

players = [ThisPlayerList[x[0]](*x[1:]) for x in args.p]
if(len(players) == 1):
    players += [ThisPlayerList[x[0]](*x[1:]) for x in args.p]
    
for p1 in players:
    for p2 in players:
        if p1 is p2:
            continue
        print("[{0}] vs [{1}]".format(p1.GetName(),p2.GetName()))
        game = Reversi.Game(p1, p2)
        result = game.GamePlay(DebugPrintFinalBoard=1)
        print(result)
        if (result[0]>result[1]):
            print("{} win".format(p1.GetName()))
        elif (result[0]<result[1]):
            print("{} win".format(p2.GetName()))
        else:
            print("draw")

   

print("End")
print("success")