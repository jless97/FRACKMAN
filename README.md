# FrackMan 2D Video Game

## Background
FrackMan is a 2D video game similar to the classic arcade game "Dig-Dug". The game consists of a playable
grid, which is populated by several game objects (or actors). The user plays as the FrackMan, who is mining
for oil barrels, which is the collectable item required to beat the current level. Each level consists of
a specified set of invisible oil barrels that are collected by the player by the player running into them.
However, the game has an added level of difficulty due to various enemies (i.e. protesters), who don't 
appreciate the FrackMan fracking for oil, and try to make the FrackMan leave the oil fields, and return 
home. These protesters will track down the player, and continuously shout at the player, until the player
finally has enough, and decides to head home and leave the oil fields for good.

In addition to the main player (FrackMan) and the enemies (Protesters), there are many other objects within
the game that add to the dimensions of the playable levels. First of all, the oil field is composed of a 
64x64 grid of dirt that is mined by the FrackMan character. Hidden within the dirt are several objects that
can either be picked up the FrackMan character, or serve as obstacles. Some of the pick-up objects are gold
chunks (which can be used to bribe the Protesters to leave the FrackMan alone), water deposits (which serve
as ammunition for the FrackMan's water gun to deter the Protesters' advance), sonar kits (which 'ping' a
specified area around the FrackMan to hone in on oil barrels), and many more.

For this project, I implemented the game configuration, game grid, and all of the objects within the game.
However, the graphics display setup, and the game engine, we're configured by Professor Smallberg. 
Therefore, I built the bulk of the video game, but exploit the game engine and graphics APIs provided by
the Professor (displayed using XQuartz).

## Built With
* IDE: Xcode
* Language: C++
* Display Software: XQuartz
