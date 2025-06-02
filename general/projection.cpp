#include "../headers/game.hpp"
//get normal between two points, just kidding it's rotated 90 degrees and I'm not fixing is cause it's already used too much in my code :/
returnXY game::getNormal(returnXY point1,returnXY point2){
    returnXY output;
    output.x =  -(point2.y-point1.y);
    output.y = (point2.x-point1.x);

    float length = sqrt(square(output.x)+square(output.y));

    if (length != 0) {
        output.x /= length;
        output.y /= length;
    }

    return output;
}
//projects  point onto normal vector
float game::projectPointOntoNormal(const returnXY& point, const returnXY& normal) {
    // Calculate the dot product of the point and the normal
    return point.x * normal.x + point.y * normal.y;
}
//rotates normal vector 90 degrees
returnXY game::invertNormal(returnXY normal){
    returnXY output;
    output.x=normal.y;
    output.y=-normal.x;
    return output;
}
