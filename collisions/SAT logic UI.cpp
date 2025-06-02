#include "../headers/game.hpp"

//SAT calculations
SATout game::SATUI(int o1,int o2){
    SATout output;


    if((UI[o1].sides>10||UI[o2].sides>10)&&((UI[o1].objectType==-1)||(UI[o2].objectType==-1))){

        returnXY point1;
        returnXY point2;
        point1.x=UI[o1].X;
        point1.y=UI[o1].Y;
        point2.x=UI[o2].X;
        point2.y=UI[o2].Y;
        output.normal = getNormal(point1,point2);
        float tmpX = output.normal.x;
        float tmpY = output.normal.y;
        output.normal.x=tmpY;
        output.normal.y=-tmpX;
        if(UI[o1].objectType==-1){
            if(sqrt(square(UI[o1].X-UI[o2].X)+square(UI[o1].Y-UI[o2].Y)) > (baseUnit*UI[o2].sizeModifier*2)){
                output.difference=1;
                return output;
            }
            output.difference=sqrt(square(UI[o1].X-UI[o2].X)+square(UI[o1].Y-UI[o2].Y))-(baseUnit*UI[o2].sizeModifier*2);
        }else{
            if(sqrt(square(UI[o1].X-UI[o2].X)+square(UI[o1].Y-UI[o2].Y)) > (baseUnit*UI[o1].sizeModifier*2)){
                output.difference=1;
                return output;
            }
            output.difference=sqrt(square(UI[o1].X-UI[o2].X)+square(UI[o1].Y-UI[o2].Y))-(baseUnit*UI[o1].sizeModifier*2);
        }


    }
    if(UI[o1].sides>10||UI[o2].sides>10){
        if(UI[o2].sides>10){
            if(circleShapePoly(UI[o1].objectType)){
                output = SATLoopCirclePolyUI(o1,o2,UI[o1].sides,output);
                if(output.difference>0){
                    return output;
                }


            }else if(rectShapePoly(UI[o1].objectType)){
                output = SATLoopCirclePolyUI(o1,o2,4,output);
                if(output.difference>0){
                    return output;
                }
            }else if(convexShapePoly(UI[o1].objectType)){
                output = SATLoopCirclePolyUI(o1,o2,UI[o1].points,output);
                if(output.difference>0){
                    return output;
                }
            }
        }else{
            if(circleShapePoly(UI[o2].objectType)){

                output = SATLoopCirclePolyUI(o2,o1,UI[o2].sides,output);
                if(output.difference>0){
                    return output;
                }

            }else if(rectShapePoly(UI[o2].objectType)){
                output = SATLoopCirclePolyUI(o2,o1,4,output);
                if(output.difference>0){
                    return output;
                }
            }else if(convexShapePoly(UI[o2].objectType)){
                output = SATLoopCirclePolyUI(o2,o1,UI[o2].points,output);
                if(output.difference>0){
                    return output;
                }
            }
        }


    }
    if(circleShapePoly(UI[o1].objectType)){
        if(UI[o1].sides<=10){
            output = SATLoopUI(o1,o2,UI[o2].sides,output);
            if(output.difference>0){
                return output;
            }
        }

    }else if(rectShapePoly(UI[o1].objectType)){
        output = SATLoopUI(o1,o2,4,output);
        if(output.difference>0){
            return output;
        }
    }else if(convexShapePoly(UI[o1].objectType)){
        output = SATLoopUI(o1,o2,UI[o1].points,output);
        if(output.difference>0){
            return output;
        }
    }

    if(circleShapePoly(UI[o2].objectType)){
        if(UI[o2].sides<=10){
            output = SATLoopUI(o2,o1,UI[o2].sides,output);
        }
        if(output.difference>0){
            return output;
        }

    }else if(rectShapePoly(UI[o2].objectType)){
        output = SATLoopUI(o2,o1,4,output);
        if(output.difference>0){
            return output;
        }
    }else if(convexShapePoly(UI[o2].objectType)){
        output = SATLoopUI(o2,o1,UI[o2].points,output);
        if(output.difference>0){
            return output;
        }
    }







    return output;
}
//polygon on ponlygon collisions
SATout game::SATLoopUI(int o1,int o2,int times,SATout output){
    returnXY normal;
    maxMin max1;
    maxMin min1;
    maxMin max2;
    maxMin min2;
    for(int i = 0;i<times;i++){
        returnXY point1 = angleOffsetUI(o1,i);
        returnXY point2;
        if(i+1<times){
            point2 = angleOffsetUI(o1, i+1);
        }else{
            point2 = angleOffsetUI(o1, 0);
        }
        normal = getNormal(point1,point2);

        max1 = getMaxNormalUI(o1,normal);
        min1 = getMinNormalUI(o1,normal);
        max2 = getMaxNormalUI(o2,normal);
        min2 = getMinNormalUI(o2,normal);

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
SATout game::SATLoopCirclePolyUI(int o1, int o2, int times,SATout output){
    returnXY normal;
    maxMin max1;
    maxMin min1;
    maxMin max2;
    maxMin min2;
    for(int i = 0;i<times;i++){
        returnXY point1 = angleOffsetUI(o1,i);
        point1.x+=UI[o1].X;
        point1.y+=UI[o1].Y;
        returnXY point2;

        point2.x = UI[o2].X;
        point2.y = UI[o2].Y;

        normal = invertNormal(getNormal(point1,point2));

        max1 = getMaxNormalUI(o1,normal);
        min1 = getMinNormalUI(o1,normal);
        max2 = getMaxNormalUI(o2,normal);
        min2 = getMinNormalUI(o2,normal);
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
