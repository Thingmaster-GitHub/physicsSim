

#include "game.hpp"

W = 1366;
H = 768;
objectCount = 0;
baseUnit = (W/128+H/72)/2;
debug =    false;

physics = true;

jumpCountDown = 1;
canJump=0;

centerCamera = true;
camOffsetX=0;
camOffsetY=0;

wrap = false;//don't enable with centered camera.

mouseObject;
coefficientOfRestitution=0.6;

std::vector<object> objects;
std::vector<int> objectLoadOrder;


int main()
{
    game app;
    app.run();

    return 0;
}

