#include "../headers/headers.cpp"
//used for bounding box
//gets minimum X
float game::getMinX(int object){

    float pointOutput = angleOffset(object,0).x;
    if(objects[object].sides<=10){
        if(circleShapePoly(object)){


            for(int iS=1;iS<objects[object].sides;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }
            }

        }else if(rectShapePoly(object)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }
            }
        }else if(convexShapePoly(object)){


            for(int iS=1;iS<objects[object].points;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x<pointOutput){
                    pointOutput=pointValue.x;
                }

            }
        }

    }else{
        pointOutput = -objects[object].sizeModifier*baseUnit*2;
    }
    return pointOutput+objects[object].X;
}
//gets minimum Y
float game::getMinY(int object){

    float pointOutput = angleOffset(object,0).y;
    if(objects[object].sides<=10){
        if(circleShapePoly(object)){

            for(int iS=1;iS<objects[object].sides;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }
            }

        }else if(rectShapePoly(object)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }
            }
        }else if(convexShapePoly(object)){

            for(int iS=1;iS<objects[object].points;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y<pointOutput){
                    pointOutput=pointValue.y;
                }

            }
        }
    }else{
        pointOutput = -objects[object].sizeModifier*baseUnit*2;
    }


    return pointOutput+objects[object].Y;
}
//gets maximum X
float game::getMaxX(int object){

    float pointOutput = angleOffset(object,0).x;
    if(objects[object].sides<=10){
        if(circleShapePoly(object)){

            for(int iS=1;iS<objects[object].sides;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }
            }

        }else if(rectShapePoly(object)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }
            }
        }else if(convexShapePoly(object)){

            for(int iS=1;iS<objects[object].points;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.x>pointOutput){
                    pointOutput=pointValue.x;
                }

            }
        }
    }else{
        pointOutput = objects[object].sizeModifier*baseUnit*2;
    }
    return pointOutput+objects[object].X;
}
//gets maximum y
float game::getMaxY(int object){

    float pointOutput = angleOffset(object,0).y;
    if(objects[object].sides<=10){
        if(circleShapePoly(object)){

            for(int iS=1;iS<objects[object].sides;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }
            }

        }else if(rectShapePoly(object)){

            for(int iS=1;iS<4;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }
            }
        }else if(convexShapePoly(object)){

            for(int iS=1;iS<objects[object].points;iS++){
                returnXY pointValue = angleOffset(object,iS);
                if(pointValue.y>pointOutput){
                    pointOutput=pointValue.y;
                }

            }
        }
    }else{
        pointOutput = objects[object].sizeModifier*baseUnit*2;
    }
    return pointOutput+objects[object].Y;
}
//gets maximum normal of shape
maxMin game::getMaxNormal(int object,returnXY normal){
    maxMin output;
    output.maxMin=-std::numeric_limits<float>::infinity();
    float test;
    returnXY testPoint;
    //circle
    if(objects[object].sides>10){
        output.maxMin = objects[object].X*normal.x+objects[object].Y*normal.y+baseUnit*objects[object].sizeModifier*2;
        output.point.x=objects[object].X;
        output.point.y=objects[object].Y;
        return output;
    }

    if(circleShapePoly(object)){
        for(int i = 0;i<objects[object].sides;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(rectShapePoly(object)){
        for(int i = 0;i<4;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(convexShapePoly(object)){
        for(int i = 0;i<objects[object].points;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test>output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else{
        output.point.x=objects[object].X;
        output.point.y=objects[object].Y;

        output.maxMin = projectPointOntoNormal(output.point,normal);
    }
    return output;
}
//gets minimum normal of shape
maxMin game::getMinNormal(int object,returnXY normal){
    maxMin output;
    output.maxMin=std::numeric_limits<float>::infinity();
    float test;
    returnXY testPoint;
    //circle
    if(objects[object].sides>10){
        output.maxMin = objects[object].X*normal.x+objects[object].Y*normal.y-baseUnit*objects[object].sizeModifier*2;
        output.point.x=objects[object].X;
        output.point.y=objects[object].Y;
        return output;
    }

    if(circleShapePoly(object)){
        for(int i = 0;i<objects[object].sides;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(rectShapePoly(object)){
        for(int i = 0;i<4;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else if(convexShapePoly(object)){
        for(int i = 0;i<objects[object].points;i++){
            testPoint = angleOffset(object,i);

            testPoint.x+=objects[object].X;
            testPoint.y+=objects[object].Y;
            test = projectPointOntoNormal(testPoint,normal);

            if(test<output.maxMin){
                output.maxMin=test;
                output.point = testPoint;
            }
        }
    }else{
        output.point.x=objects[object].X;
        output.point.y=objects[object].Y;

        output.maxMin = projectPointOntoNormal(output.point,normal);
    }
    return output;
}
