//just a normal header file, nothing to see here
extern const int W = 1366;
extern const int H = 768;
extern int objectCount = 0;
extern const float baseUnit = (W/128+H/72)/2;

extern bool debug =    false;

extern bool physics = true;

extern float jumpCountDown = 1;
extern int canJump=0;

extern const bool centerCamera = true;
extern float camOffsetX=0;
extern float camOffsetY=0;

extern const bool wrap = false;//don't enable with centered camera.

extern int mouseObject;
extern float coefficientOfRestitution=0.6;

extern std::vector<object> objects;
extern std::vector<int> objectLoadOrder;
