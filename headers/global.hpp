#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "structs.hpp"
#pragma once

extern const int mX;//middle x
extern const int mY;//middle y
extern const int W;
extern const int H;
extern int objectCount;
extern float baseUnit;

extern bool debug;


extern float zoomAMT;

extern float jumpCountDown;
extern int canJump;

extern const bool centerCamera;
extern float camOffsetX;
extern float camOffsetY;
extern bool scrMove;
extern float initXoff;
extern float initYoff;
extern float initMX;
extern float initMY;

extern const bool wrap;

extern std::string cursorMode;

extern int mouseObject;
extern float coefficientOfRestitution;


extern std::vector<object> objects;
extern std::vector<object> UI;

extern std::vector<cpy> copyArray;
