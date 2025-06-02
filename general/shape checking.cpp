#include "../headers/game.hpp"
//returns if object is of type regular polygon(sf::circleShape)
bool game::circleShapePoly(int type){
    if(type==0||type==1){
        return true;
    }else{
        return false;
    }
}
//returns if object is type rectangle
bool game::rectShapePoly(int type){
    if(type==2||type==-3||type==4){
        return true;
    }else{
        return false;
    }
}
//returns if object is type convex polygon
bool game::convexShapePoly(int type){
    if(type==3){
        return true;
    }else{
        return false;
    }
}
//returns if object is type text
bool game::TextShapePoly(int type){
    if(type==4){
        return true;
    }else{
        return false;
    }
}
