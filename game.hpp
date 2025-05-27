#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "structs.hpp"

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
