#include "../headers/game.hpp"
//returns the distance to nearest corner of specified shape
pointDist game::cornerDistCheck(int o){
    pointDist check= {std::numeric_limits<float>::infinity(),0};
    for(int i=0;i<=pointCount(o);i++){

        returnXY point = angleOffset(o,i);

        returnXY pointXY = {(point.x+baseUnit*objects[o].X+camOffsetX)/zoomAMT+W/2 ,(point.y+baseUnit*objects[o].Y+camOffsetY)/zoomAMT+H/2};

        returnXY position ={(baseUnit*objects[mouseObject].X+camOffsetX)/zoomAMT+W/2,(baseUnit*objects[mouseObject].Y+camOffsetY)/zoomAMT+H/2};


        float distance=sqrt(square(pointXY.x-position.x)+square(pointXY.y-position.y));

        if(distance<=check.distance){
            check.distance=distance;
            check.point=i;


        }
    }
    return check;
}
//apply this in more places pls
//should've written this earlier
//gets point count of index
int game::pointCount(int i){
    if(circleShapePoly(objects[i].objectType)){
        return objects[i].sides;
    }else if(rectShapePoly(objects[i].objectType)){
        return 4;
    }else if(convexShapePoly(objects[i].objectType)){
        return objects[i].points;
    }else{
        return 0;
    }
}
//degrees to radians
float game::degToRad(float deg){
    for(;deg>360;deg-=360){}
    return deg*(3.14/180);
}
//radians to degrees
float game::radToDeg(float rad){
    return rad*(180/3.14);
}
//returns offset of specified point from shape center
returnXY game::angleOffset(int i,int point){//should return the point as an offset from the center of the shape with X and Y values

    returnXY output = {0,0};
    if(circleShapePoly(objects[i].objectType))/*circleShape*/{
        if(objects[i].sides%2!=0){
            output.x=baseUnit*objects[i].sizeModifier*2*sin(-degToRad(objects[i].rotation+180/objects[i].sides+360/objects[i].sides*(point-1)));
            output.y=baseUnit*objects[i].sizeModifier*2*cos(-degToRad(objects[i].rotation+180/objects[i].sides+360/objects[i].sides*(point-1)));
        }else{
            output.x=baseUnit*objects[i].sizeModifier*2*sin(-degToRad(objects[i].rotation+360/objects[i].sides*(point-1)));
            output.y=baseUnit*objects[i].sizeModifier*2*cos(-degToRad(objects[i].rotation+360/objects[i].sides*(point-1)));
        }

    }else if(rectShapePoly(objects[i].objectType)){
        float dist = sqrt(square(objects[i].width/2)+square(objects[i].height/2))*baseUnit;
        float rad = asin(objects[i].height*baseUnit/2/dist);
        if(point%2!=0){
            output.x=dist*sin(-degToRad(objects[i].rotation+90*(point))+rad);
            output.y=dist*cos(-degToRad(objects[i].rotation+90*(point))+rad);
        }else{
            output.x=dist*sin(-degToRad(objects[i].rotation+90*(point)+90)-rad);
            output.y=dist*cos(-degToRad(objects[i].rotation+90*(point)+90)-rad);
        }


    }else if(convexShapePoly(objects[i].objectType)){//why didn't I just use a distance formula??
        float dist = sqrt(square(objects[i].pointList[point*2])+square(objects[i].pointList[point*2+1]))*baseUnit*objects[i].sizeModifier;
        float rad =asin(objects[i].pointList[point*2+1]*baseUnit*objects[i].sizeModifier/dist);

        float x=objects[i].pointList[point*2];
        float y=objects[i].pointList[point*2+1];


        if(congruent(x,y)){
            if(x<0){
                output.x=dist*sin(-degToRad(objects[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(objects[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(objects[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(objects[i].rotation-90)-rad);
            }


        }else{
            if(x<0){
                output.x=dist*sin(-degToRad(objects[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(objects[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(objects[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(objects[i].rotation-90)-rad);
            }

        }




    }



    return output;
}
//returns offset of specified point from shape center
returnXY game::angleOffsetUI(int i,int point){//should return the point as an offset from the center of the shape with X and Y values

    returnXY output = {0,0};
    if(circleShapePoly(UI[i].objectType))/*circleShape*/{
        if(UI[i].sides%2!=0){
            output.x=baseUnit*UI[i].sizeModifier*2*sin(-degToRad(UI[i].rotation+180/UI[i].sides+360/UI[i].sides*(point-1)));
            output.y=baseUnit*UI[i].sizeModifier*2*cos(-degToRad(UI[i].rotation+180/UI[i].sides+360/UI[i].sides*(point-1)));
        }else{
            output.x=baseUnit*UI[i].sizeModifier*2*sin(-degToRad(UI[i].rotation+360/UI[i].sides*(point-1)));
            output.y=baseUnit*UI[i].sizeModifier*2*cos(-degToRad(UI[i].rotation+360/UI[i].sides*(point-1)));
        }

    }else if(rectShapePoly(UI[i].objectType)){
        float dist = sqrt(square(UI[i].width/2)+square(UI[i].height/2))*baseUnit;
        float rad = asin(UI[i].height*baseUnit/2/dist);
        if(point%2!=0){
            output.x=dist*sin(-degToRad(UI[i].rotation+90*(point))+rad);
            output.y=dist*cos(-degToRad(UI[i].rotation+90*(point))+rad);
        }else{
            output.x=dist*sin(-degToRad(UI[i].rotation+90*(point)+90)-rad);
            output.y=dist*cos(-degToRad(UI[i].rotation+90*(point)+90)-rad);
        }


    }else if(convexShapePoly(UI[i].objectType)){
        float dist = sqrt(square(UI[i].pointList[point*2])+square(UI[i].pointList[point*2+1]))*baseUnit*UI[i].sizeModifier;
        float rad =asin(UI[i].pointList[point*2+1]*baseUnit*UI[i].sizeModifier/dist);

        float x=UI[i].pointList[point*2];
        float y=UI[i].pointList[point*2+1];


        if(congruent(x,y)){
            if(x<0){
                output.x=dist*sin(-degToRad(UI[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(UI[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(UI[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(UI[i].rotation-90)-rad);
            }


        }else{
            if(x<0){
                output.x=dist*sin(-degToRad(UI[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(UI[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(UI[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(UI[i].rotation-90)-rad);
            }

        }


    }



    return output;
}
//squares input
float game::square(float n){
    return n*n;
}
//if the polarity of both inputs is the same, returns true
bool game::congruent(float x,float y){
    if(x<0&&y<0){
        return true;
    }else if(x>0&&y>0){
        return true;
    }else{
        return false;
    }
}
