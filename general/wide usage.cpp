#include "../headers/headers.cpp"
//degrees to radians
float game::degToRad(float deg){
    while(deg>360){
        deg-=360;
    }
    return deg*(3.14/180);
}
//radians to degrees
float game::radToDeg(float rad){
    return rad*(180/3.14);
}
//returns offset of specified point from shape center
returnXY game::angleOffset(int i,int point){//should return the point as an offset from the center of the shape with X and Y values

    returnXY output = {0,0};
    if(circleShapePoly(i))/*circleShape*/{
        if(objects[i].sides%2!=0){
            output.x=baseUnit*objects[i].sizeModifier*2*sin(-degToRad(objects[i].rotation+180/objects[i].sides+360/objects[i].sides*(point-1)));
            output.y=baseUnit*objects[i].sizeModifier*2*cos(-degToRad(objects[i].rotation+180/objects[i].sides+360/objects[i].sides*(point-1)));
        }else{
            output.x=baseUnit*objects[i].sizeModifier*2*sin(-degToRad(objects[i].rotation+360/objects[i].sides*(point-1)));
            output.y=baseUnit*objects[i].sizeModifier*2*cos(-degToRad(objects[i].rotation+360/objects[i].sides*(point-1)));
        }

    }else if(rectShapePoly(i)){
        float dist = sqrt(square(objects[i].width/2)+square(objects[i].height/2))*baseUnit;
        float rad = asin(objects[i].height*baseUnit/2/dist);
        if(point%2!=0){
            output.x=dist*sin(-degToRad(objects[i].rotation+90*(point))+rad);
            output.y=dist*cos(-degToRad(objects[i].rotation+90*(point))+rad);
        }else{
            output.x=dist*sin(-degToRad(objects[i].rotation+90*(point)+90)-rad);
            output.y=dist*cos(-degToRad(objects[i].rotation+90*(point)+90)-rad);
        }

        //std::cout<<"point: "<<point<<"\ndeg: " << radToDeg(rad)<<"\ndist: "<< dist <<"\nbaseUnit: "<<baseUnit<<"\n";
    }else if(convexShapePoly(i)){
        float x=objects[i].pointList[point*2];
        float y=objects[i].pointList[point*2+1];

        float dist = sqrt(square(x)+square(y))*baseUnit*objects[i].sizeModifier;
        float rad =asin(y*baseUnit*objects[i].sizeModifier/dist);


        //std::cout<<"\n\npoint: "<<point<<"\n\nXch: "<<X<<"\nYch: "<<Y<<"\n\nXpoint: "<<objects[i].pointList[point*2]<<"\nYpoint: "<<objects[i].pointList[point*2+1]<<"\n\nx: "<<x<<"\ny: "<<y<<"\n";

        if(congruent(x,y)){
            if(x<0){
                output.x=dist*sin(-degToRad(objects[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(objects[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(objects[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(objects[i].rotation-90)-rad);
            }

            //std::cout<<"point: "<<point<<"\n\n";
        }else{
            if(x<0){
                output.x=dist*sin(-degToRad(objects[i].rotation+90)+rad);
                output.y=dist*cos(-degToRad(objects[i].rotation+90)+rad);
            }else{
                output.x=dist*sin(-degToRad(objects[i].rotation-90)-rad);
                output.y=dist*cos(-degToRad(objects[i].rotation-90)-rad);
            }

        }

        //std::cout<<"point: "<<point<<"\n";
        //std::cout<<"rotation: "<<rot<<"\n";

        //std::cout<<"point: "<<point<<"\nX: "<<x<<"\nY: "<<y<<"\ninX: "<<objects[i].pointList[point*2]<<"\ninY: "<<objects[i].pointList[point*2+1]<<"\n\n";
    }

    //std::cout<<"object type: "<<objects[i].objectType<<"\n";

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
float game::distance(returnXY point1,returnXY point2){
    return sqrt(square(point1.x-point2.x)+square(point1.y-point2.y));
}
