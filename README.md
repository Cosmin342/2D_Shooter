# 2D Survival Shooter

2D game developed in OpenGL and C++

## About the game

The idea of the game is to kill a number of enemies to win it.

Player is spawned on a map where exist some walls and enemies. Enemies appear
random on map and follow player, trying to decrease his life. If an enemy
hit the player, it disappear and life of player will decrease.

Player can kill the enemies by launching bullets, but bullets have a limited
range and he has a cooldown of 1s until he can shoot again.

## Controls

`W A S D` to move the player in the maze.<br/>
`Left click` to launch a bullet to an enemy.<br/>
`Mouse cursor` to change direction where a bullet is launched.

## Source code

Source code for current game can be found in folder `\src\lab_m1\tema1`

## Build

### Prerequisites

You need to install CMake to compile the project.

### Steps

From PowerShell on Windows with CMake, use following commands:  

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Run

After building the project, use following command `` .\bin\Debug\GFXFramework `` to run the game.