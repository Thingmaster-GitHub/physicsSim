
#include "headers/headers.cpp"


int W = 1366;
int H = 768;

float baseUnit = (W/128+H/72)/2;

bool debug =    false;

bool physics = true;

float jumpCountDown = 1;
int canJump=0;

bool centerCamera = true;
float camOffsetX=0;
float camOffsetY=0;

const bool wrap = false;//don't enable with centered camera.

int mouseObject;
float coefficientOfRestitution=0.6;

std::vector<object> objects;


int main()
{
    game app;
    app.run();

    return 0;
}

