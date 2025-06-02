#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#pragma once

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
};
struct pointDist{
    float distance=0;
    int point=0;
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
    //4 is text
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
    returnXY offset = {0,0};
    int layer=0;
    std::string text="lorium ipsum";
    std::string fontLoc = "Comic Sans MS.ttf";
    sf::Font font;
    bool selected=false;
    bool clicked =false;

    bool pointGrabbed=false;
    int grabbedPoint = 0;//not used by circleShape
    std::string txtLbl = "default";
    std::string textExtra="";//UI element, please ignore
    bool txtBoxSelected = false;
    bool editable = false; //does't do anything yet
    float bgDist =1;//unimplimented
};
struct cpy{
    object coppied;
    int coppiedObj;
};

