#include "../headers/game.hpp"
#include "../headers/global.hpp"
#include "../headers/structs.hpp"


float game::getMinXUI(int object){

    float pointOutput = angleOffsetUI(object,0).x;
    if(UI[object].sides<=10){
        if(circleShapePoly(UI[object].objectType)){


            for(int iS=1;iS<UI[object].sides;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }
            }

        }else if(rectShapePoly(UI[object].objectType)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }
            }
        }else if(convexShapePoly(UI[object].objectType)){


            for(int iS=1;iS<UI[object].points;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }

            }
        }

    }else{
        pointOutput = -UI[object].sizeModifier*baseUnit*2;
    }
    if(cursorMode=="edit"){
        return pointOutput+baseUnit*UI[object].X-5;
    }
    return pointOutput+baseUnit*UI[object].X;
}
//gets minimum Y
float game::getMinYUI(int object){

    float pointOutput = angleOffsetUI(object,0).y;
    if(UI[object].sides<=10){
        if(circleShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].sides;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }
            }

        }else if(rectShapePoly(UI[object].objectType)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }
            }
        }else if(convexShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].points;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }

            }
        }
    }else{
        pointOutput = -UI[object].sizeModifier*baseUnit*2;
    }


    if(cursorMode=="edit"){
        return pointOutput+baseUnit*UI[object].Y-5;
    }
    return pointOutput+baseUnit*UI[object].Y;
}
//gets maximum X
float game::getMaxXUI(int object){

    float pointOutput = angleOffsetUI(object,0).x;
    if(UI[object].sides<=10){
        if(circleShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].sides;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }
            }

        }else if(rectShapePoly(UI[object].objectType)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }
            }
        }else if(convexShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].points;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }

            }
        }
    }else{
        pointOutput = UI[object].sizeModifier*baseUnit*2;
    }

    if(cursorMode=="edit"){
        return pointOutput+baseUnit*UI[object].X+5;
    }
    return pointOutput+baseUnit*UI[object].X;
}
//gets maximum y
float game::getMaxYUI(int object){

    float pointOutput = angleOffsetUI(object,0).y;
    if(UI[object].sides<=10){
        if(circleShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].sides;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }
            }

        }else if(rectShapePoly(UI[object].objectType)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }
            }
        }else if(convexShapePoly(UI[object].objectType)){

            for(int iS=1;iS<UI[object].points;iS++){
                returnXY pointValue = angleOffsetUI(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }

            }
        }
    }else{
        pointOutput = UI[object].sizeModifier*baseUnit*2;
    }

    if(cursorMode=="edit"){
        return pointOutput+baseUnit*UI[object].Y+5;
    }
    return pointOutput+baseUnit*UI[object].Y;
}
//gets maximum normal of shape
maxMin game::getMaxNormalUI(int object,returnXY normal){
    maxMin output;
    output.maxMin=-std::numeric_limits<float>::infinity();
    float test;
    returnXY testPoint;
    //circle
    if(UI[object].sides>10){
        output.maxMin = baseUnit*UI[object].X*normal.x+baseUnit*UI[object].Y*normal.y+baseUnit*UI[object].sizeModifier*2;
        output.point.x=baseUnit*UI[object].X;
        output.point.y=baseUnit*UI[object].Y;
        return output;
    }

    if(circleShapePoly(UI[object].objectType)){
        for(int i = 0;i<UI[object].sides;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(rectShapePoly(UI[object].objectType)){
        for(int i = 0;i<4;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(convexShapePoly(UI[object].objectType)){
        for(int i = 0;i<UI[object].points;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else{
        output.point.x=baseUnit*UI[object].X;
        output.point.y=baseUnit*UI[object].Y;

        output.maxMin = projectPointOntoNormal(output.point,normal);
    }
    return output;
}
//gets minimum normal of shape
maxMin game::getMinNormalUI(int object,returnXY normal){
    maxMin output;
    output.maxMin=std::numeric_limits<float>::infinity();
    float test;
    returnXY testPoint;
    //circle
    if(UI[object].sides>10){
        output.maxMin = baseUnit*UI[object].X*normal.x+baseUnit*UI[object].Y*normal.y-baseUnit*UI[object].sizeModifier*2;
        output.point.x=baseUnit*UI[object].X;
        output.point.y=baseUnit*UI[object].Y;
        return output;
    }

    if(circleShapePoly(UI[object].objectType)){
        for(int i = 0;i<UI[object].sides;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(rectShapePoly(UI[object].objectType)){
        for(int i = 0;i<4;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(convexShapePoly(UI[object].objectType)){
        for(int i = 0;i<UI[object].points;i++){
            testPoint = angleOffsetUI(object,i);

            testPoint.x+=baseUnit*UI[object].X;
            testPoint.y+=baseUnit*UI[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else{
        output.point.x=baseUnit*UI[object].X;
        output.point.y=baseUnit*UI[object].Y;

        output.maxMin = projectPointOntoNormal(output.point,normal);
    }
    return output;
}
