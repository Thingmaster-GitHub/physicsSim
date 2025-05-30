#include "../headers/game.hpp"
#include "../headers/global.hpp"
#include "../headers/structs.hpp"
//SAT calculations
SATout game::SAT(int o1,int o2){
    SATout output;

    if((objects[o1].sides>10||objects[o2].sides>10)&&((objects[o1].objectType==-1)||(objects[o2].objectType==-1))){

        returnXY point1;
        returnXY point2;
        point1.x=objects[o1].X;
        point1.y=objects[o1].Y;
        point2.x=objects[o2].X;
        point2.y=objects[o2].Y;
        output.normal = getNormal(point1,point2);
        float tmpX = output.normal.x;
        float tmpY = output.normal.y;
        output.normal.x=tmpY;
        output.normal.y=-tmpX;
        if(objects[o1].objectType==-1){
            if(sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y)) > (baseUnit*objects[o2].sizeModifier*2)){
                output.difference=1;
                return output;
            }
            output.difference=sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y))-(baseUnit*objects[o2].sizeModifier*2);
        }else{
            if(sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y)) > (baseUnit*objects[o1].sizeModifier*2)){
                output.difference=1;
                return output;
            }
            output.difference=sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y))-(baseUnit*objects[o1].sizeModifier*2);
        }


    }
    if(objects[o1].sides>10||objects[o2].sides>10){
        if(objects[o2].sides>10){
            if(circleShapePoly(objects[o1].objectType)){
                output = SATLoopCirclePoly(o1,o2,objects[o1].sides,output);
                if(output.difference>0){
                    return output;
                }


            }else if(rectShapePoly(objects[o1].objectType)){
                output = SATLoopCirclePoly(o1,o2,4,output);
                if(output.difference>0){
                    return output;
                }
            }else if(convexShapePoly(objects[o1].objectType)){
                output = SATLoopCirclePoly(o1,o2,objects[o1].points,output);
                if(output.difference>0){
                    return output;
                }
            }
        }else{
            if(circleShapePoly(objects[o2].objectType)){

                output = SATLoopCirclePoly(o2,o1,objects[o2].sides,output);
                if(output.difference>0){
                    return output;
                }

            }else if(rectShapePoly(objects[o2].objectType)){
                output = SATLoopCirclePoly(o2,o1,4,output);
                if(output.difference>0){
                    return output;
                }
            }else if(convexShapePoly(objects[o2].objectType)){
                output = SATLoopCirclePoly(o2,o1,objects[o2].points,output);
                if(output.difference>0){
                    return output;
                }
            }
        }


    }
    if(circleShapePoly(objects[o1].objectType)){
        if(objects[o1].sides<=10){
            output = SATLoop(o1,o2,objects[o2].sides,output);
            if(output.difference>0){
                return output;
            }
        }

    }else if(rectShapePoly(objects[o1].objectType)){
        output = SATLoop(o1,o2,4,output);
        if(output.difference>0){
            return output;
        }
    }else if(convexShapePoly(objects[o1].objectType)){
        output = SATLoop(o1,o2,objects[o1].points,output);
        if(output.difference>0){
            return output;
        }
    }

    if(circleShapePoly(objects[o2].objectType)){
        if(objects[o2].sides<=10){
            output = SATLoop(o2,o1,objects[o2].sides,output);
        }
        if(output.difference>0){
            return output;
        }

    }else if(rectShapePoly(objects[o2].objectType)){
        output = SATLoop(o2,o1,4,output);
        if(output.difference>0){
            return output;
        }
    }else if(convexShapePoly(objects[o2].objectType)){
        output = SATLoop(o2,o1,objects[o2].points,output);
        if(output.difference>0){
            return output;
        }
    }







    return output;
}
//polygon on ponlygon collisions
SATout game::SATLoop(int o1,int o2,int times,SATout output){
    returnXY normal;
    maxMin max1;
    maxMin min1;
    maxMin max2;
    maxMin min2;
    for(int i = 0;i<times;i++){
        returnXY point1 = angleOffset(o1,i);
        returnXY point2;
        if(i+1<times){
            point2 = angleOffset(o1, i+1);
        }else{
            point2 = angleOffset(o1, 0);
        }
        normal = getNormal(point1,point2);

        max1 = getMaxNormal(o1,normal);
        min1 = getMinNormal(o1,normal);
        max2 = getMaxNormal(o2,normal);
        min2 = getMinNormal(o2,normal);

        if(min2.maxMin-max1.maxMin>output.difference){
            output.difference=min2.maxMin-max1.maxMin;
            output.normal=normal;
            output.point2=min2.point;
            output.point1=max1.point;
            //output.point2=min2.point;
        }
        if(min1.maxMin-max2.maxMin>output.difference){
            output.difference=min1.maxMin-max2.maxMin;
            output.normal=normal;
            output.point1=min1.point;
            output.point2=max2.point;
            //output.point2=min1.point;
        }


        if(max1.maxMin < min2.maxMin || min1.maxMin > max2.maxMin){
            return output;
        }
    }

    return output;
}
//SAT calculation for circle on polygon colisions
SATout game::SATLoopCirclePoly(int o1, int o2, int times,SATout output){
    returnXY normal;
    maxMin max1;
    maxMin min1;
    maxMin max2;
    maxMin min2;
    for(int i = 0;i<times;i++){
        returnXY point1 = angleOffset(o1,i);
        point1.x+=objects[o1].X;
        point1.y+=objects[o1].Y;
        returnXY point2;

        point2.x = objects[o2].X;
        point2.y = objects[o2].Y;

        normal = invertNormal(getNormal(point1,point2));

        max1 = getMaxNormal(o1,normal);
        min1 = getMinNormal(o1,normal);
        max2 = getMaxNormal(o2,normal);
        min2 = getMinNormal(o2,normal);
        if(min2.maxMin-max1.maxMin>output.difference){
            output.difference=min2.maxMin-max1.maxMin;
            output.normal=normal;
            output.point1=max1.point;
            output.point2=min2.point;
            //output.point2=min2.point;
        }
        if(min1.maxMin-max2.maxMin>output.difference){
            output.difference=min1.maxMin-max2.maxMin;
            output.normal=normal;
            output.point2=max2.point;
            output.point1=min1.point;
            //not needed as normals are from object one
            //output.point2=min1.point;
        }
        if(max1.maxMin < min2.maxMin || min1.maxMin > max2.maxMin){

            return output;

        }

    }

    return output;
}
