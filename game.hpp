#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>

struct returnXY{
    float x;
    float y;
};
struct maxMin{
    float maxMin;
    returnXY point;
};
struct SATout{
    float difference = -std::numeric_limits<float>::infinity();
    returnXY normal;
    returnXY point1;
    returnXY point2;
};

struct triggerProperties{
    std::string id = "default";
    std::string event = "none";
    //for event "destroy"
    bool destroyO2 = false;
    int typeReq = 1;
    bool isIDReq = false;
    std::string IDReq = "default";
};

struct object{
    float X = 0;
    float Y = 0;

    float velX=0;
    float velY=0;

    float velRot=0;//times 500
    float rotation=0;


    int sides = 0;//dont't set to 4, just make it a rectangle

    float sizeModifier=2;

    int objectType = 0;
    //-4 is nothing :3
    //-3 is trigger
    //-2 is camera
    //-1 is the mouse pointer
    //0 is object
    //1 is player
    //2 is rectangle
    //3 genaric is convex polygon
    bool gravity=false;
    bool airRes=true;
    bool solid=true;
    float mass=10;//mass of the object DO NOT SET TO ZERO
    unsigned int color = 0xffffffff;
    float width = 0;
    float height = 0;

    int points = 0;
    float pointList[30];//just pretend it's not a max of 30 (15 points :/)
    bool collidedbox = false;
    bool collidedSAT = false;
    bool grabbed=false;
    returnXY pointProjected;
    returnXY pointProjected2;
    float coefficentOfFriction = 0.5;
    triggerProperties trigger = {};
    std::string loc= "blank.png";
    sf::Texture texture;
    int layer=0;
    std::string text = "lorium ipsum";
    std::string fontLoc = "Comic Sans MS.ttf";
    sf::Font font;

};

class game{
public:
    //runs program :3
    void run();

private:
    //sets object load order for drawing
    void LayerObjects();
    //loads in objects
    int loadObjectsJSON(std::vector<object>& objectsVect, const std::string& filename);

    void trigger(int o1,int o2);
    //jump countdown
    void jumpDown(float timediff);

    void inputs(int Object,float timediff);
    //controls camera
    returnXY Camera(float PX,float PY,float timediff);
    //draws shapes
    void drawShape(sf::RenderTarget& window,int i);
    //transforms objects by velocity, gravity, and air resistance.
    void transform(int i,float timediff);
    //checks bounding box collisions and runs SAT if intersects
    void baseCollision(float timediff);
    //SAT calculations
    SATout SAT(int o1,int o2);
    //polygon on ponlygon collisions
    SATout SATLoop(int o1,int o2,int times,SATout output);
    //SAT calculation for circle on polygon colisions
    SATout SATLoopCirclePoly(int o1, int o2, int times,SATout output);
    //graphs points that are baseUnit appart in spacing
    void testingLayoutInf(sf::RenderTarget& window);
    //degrees to radians
    float degToRad(float deg);
    //radians to degrees
    float radToDeg(float rad);
    //returns offset of specified point from shape center
    returnXY angleOffset(int i,int point);
    //squares input
    float square(float n);
    //if the polarity of both inputs is the same, returns true
    bool congruent(float x,float y);
    //used for bounding box
    //gets minimum X
    float getMinX(int object);
    //gets minimum Y
    float getMinY(int object);
    //gets maximum X
    float getMaxX(int object);
    //gets maximum y
    float getMaxY(int object);
    //gets maximum normal of shape
    maxMin getMaxNormal(int object,returnXY normal);
    //gets minimum normal of shape
    maxMin getMinNormal(int object,returnXY normal);
    //get normal between two points, just kidding it's rotated 90 degrees and I'm not fixing is cause it's already used too much in my code :/
    returnXY getNormal(returnXY point1,returnXY point2);
    //projects  point onto normal vector
    float projectPointOntoNormal(const returnXY& point, const returnXY& normal);
    //respone to collision
    void collisionResponse(int o1,int o2,SATout input,float timediff);
    //returns if object is of type regular polygon(sf::circleShape)
    bool circleShapePoly(int object);
    //returns if object is type rectangle
    bool rectShapePoly(int object);
    //returns if object is type convex polygon
    bool convexShapePoly(int object);

    bool TextShapePoly(int object);
    //rotates normal vector 90 degrees
    returnXY invertNormal(returnXY normal);
    //don't input an obect besides ones for this calculated SAT input
    //calculated projected offset of shape for rotation
    float VelRotCalc(int o1,int o2,SATout input,int direction);

    float distance(returnXY point1,returnXY point2);

    void friction(int o1,int o2,returnXY normal,float timediff);

    void debuger(sf::RenderTarget& window,int i);

};
