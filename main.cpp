
#include "headers/game.hpp"

const int mX=48;
const int mY=28;
const int W = 1366;
const int H = 768;
int objectCount =15;
float baseUnit = (W/128+H/72)/2;

bool debug = false;



float zoomAMT=1;

float jumpCountDown = 1;
int canJump=0;

const bool centerCamera = true;
float camOffsetX=0;
float camOffsetY=0;
bool scrMove=false;
float initXoff;
float initYoff;
float initMX;
float initMY;

const bool wrap = false;//don't enable with centered camera.
aaaaaa
std::string cursorMode = "select";

int mouseObject;
float coefficientOfRestitution=0.6;

std::vector<object> objects;
std::vector<object> UI;

std::vector<cpy> copyArray;



int main()
{
    game app;
    app.run();

    return 0;
}
