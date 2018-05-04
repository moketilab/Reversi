
"""
   Server for Reversi
"""

import sys, copy, random

import Reversi, Players

print("Start")

user  = Players.UserPlayer()
randp = Players.RandomPlayer()

game = Reversi.Game(user, randp)
# print(game.GamePlay(DebugPrintAllBoard=1,DebugPrintFinalBoard=1))
print(game.GamePlay(DebugPrintFinalBoard=1))
    

print("End")
print("success")