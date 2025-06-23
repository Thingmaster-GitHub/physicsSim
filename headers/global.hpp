#pragma once
#include "game.hpp"
//just a normal header file, nothing to see here
extern int W;
extern int H;

extern float baseUnit;

extern bool debug;

extern bool physics;

extern float jumpCountDown;
extern int canJump;

extern bool centerCamera;
extern float camOffsetX;
extern float camOffsetY;

extern const bool wrap;//depriciated

extern int mouseObject;
extern float coefficientOfRestitution;

extern std::vector<object> objects;

