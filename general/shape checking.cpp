#include "../headers/headers.cpp"
//returns if object is of type regular polygon(sf::circleShape)
bool game::circleShapePoly(int object){
    if(objects[object].objectType==0||objects[object].objectType==1){
        return true;
    }else{
        return false;
    }
}
//returns if object is type rectangle
bool game::rectShapePoly(int object){
    if(objects[object].objectType==2||objects[object].objectType==-3||objects[object].objectType==4){
        return true;
    }else{
        return false;
    }
}
//returns if object is type convex polygon
bool game::convexShapePoly(int object){
    if(objects[object].objectType==3){
        return true;
    }else{
        return false;
    }
}

bool game::TextShapePoly(int object){
    if(objects[object].objectType==4){
        return true;
    }else{
        return false;
    }
}
