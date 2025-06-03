#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "global.hpp"
class game{
public:
    //runs program :3
    void run();

private:
    std::vector<int> typeCheckSingle(int);
    //checks object types and returns array of appropriate values
    std::vector<int> typeCheckMulti(std::vector<int>& obj);
    //toggles visibility
    void toggleVisable(object& obj);
    //checks if object is visable
    bool isVisableQ(object& obj);
    //multi object grey out logic
    void multiObjNoEdit(std::vector<int>& obj);
    //single object grey out logic
    void singleObjNoEdit(int obj);
    //makes text inputs unusable if they cant be used in this context (ex. sides on a rectangle)
    void NoEdit();
    //scales object
    void scaleObj(float amt);
    //outlines window
    void winOutline(sf::RenderTarget& window);
    //loads data into textExtra for UI elements
    void textSelected();
    //changes text
    void textCh(char txt);
    //pastes
    void paste();
    //copies
    void copy();
    //moves covexShape points
    void convexMvPoint(int o);
    //rectacle move point
    void rectMvPoint(int o);
    //grabbed point movement
    void grPointMV(int o);
    //returns the distance to nearest corner of specified shape
    pointDist cornerDistCheck(int o);
    //apply this in more places pls
    //should've written this earlier
    //gets point count of index
    int pointCount(int i);
    //draws points
    void drawPoints(sf::RenderTarget& window,int o);
    //left click
    void Lclick();
    //handles keyboard inputs
    void input(const sf::Keyboard::Scan key);
    //deletes object
    void deleteObject(int i);
    //creates object
    void createObject();
    //sets object load order for drawing
    void LayerObjectsUI();
    //sets object load order for drawing
    void LayerObjects();
    //rotates selected objects
    void rotateObj(float ammount);
    //initilizes program
    void initialize();
    //controls zoom
    void zoom(float ammount);
    //draws outline
    void drawOutline(sf::RenderTarget& window,int i);
    //saves objects to json files
    //created with the help of chatgpt
    void saveObjectsJSON(const std::vector<object>& objects, const std::string& filename);
    //loads objects from json file
    //created with the help of chatgpt
    int loadObjectsJSON(std::vector<object>& objectsVect, const std::string& filename);
    //controls camera
    void Camera(sf::Vector2i position);
    //draws UI
    void drawUI(sf::RenderTarget& window,int i,std::vector<object>& scene);
    //draws shapes
    void drawShape(sf::RenderTarget& window,int i,std::vector<object>& scene);
    //probably need to change how draw calls are handled, like specifing layers somehow, I'll do that later
    //checks bounding box collisions and runs SAT if intersects
    void baseCollision();
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
    void collisionResponse(int o1,int o2);
    //respone to collision
    void collisionResponseUI(int o1,int o2);
    //returns if object is of type regular polygon(sf::circleShape)
    bool circleShapePoly(int type);
    //returns if object is type rectangle
    bool rectShapePoly(int type);
    //returns if object is type convex polygon
    bool convexShapePoly(int type);
    //returns if object is type text
    bool TextShapePoly(int type);
    //rotates normal vector 90 degrees
    returnXY invertNormal(returnXY normal);
    //checks bounding box collisions and runs SAT if intersects
    void baseCollisionUI();
    //SAT calculations
    SATout SATUI(int o1,int o2);
    //polygon on ponlygon collisions
    SATout SATLoopUI(int o1,int o2,int times,SATout output);
    //SAT calculation for circle on polygon colisions
    SATout SATLoopCirclePolyUI(int o1, int o2, int times,SATout output);
    //returns offset of specified point from shape center
    returnXY angleOffsetUI(int i,int point);

    float getMinXUI(int object);
    //gets minimum Y
    float getMinYUI(int object);
    //gets maximum X
    float getMaxXUI(int object);
    //gets maximum y
    float getMaxYUI(int object);
    //gets maximum normal of shape
    maxMin getMaxNormalUI(int object,returnXY normal);
    //gets minimum normal of shape
    maxMin getMinNormalUI(int object,returnXY normal);
    //debug view
    void debuger(sf::RenderTarget& window,int i);

    void debugerUI(sf::RenderTarget& window,int i);

};
