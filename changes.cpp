//probably need to change how draw calls are handled, like specifing layers somehow, I'll do that later
        //checks bounding box collisions and runs SAT if intersects
        void baseCollisionUI(){
            float XMax;
            float XMin;
            float YMax;
            float YMin;

            float XMaxCh;
            float XMinCh;
            float YMaxCh;
            float YMinCh;
            SATout output;
            for(int i = 0;i<UI.size();i++){
                XMax = getMaxXUI(i);
                XMin = getMinXUI(i);
                YMax = getMaxYUI(i);
                YMin = getMinYUI(i);
                for(int iP=i+1;iP<objectCount;iP++){

                    if(i!=iP){
                        XMaxCh = getMaxXUI(iP);
                        XMinCh = getMinXUI(iP);
                        YMaxCh = getMaxYUI(iP);
                        YMinCh = getMinYUI(iP);
                        //std::cout<<"top Y 1: "<<YMin<<"\ntop Y 2: "<<YMinCh<<"\nbottom Y 1: "<<YMax<<"\nbottom Y 2: "<<YMaxCh<<"\n\n";
                        if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                            if(UI[i].objectType==-1||UI[iP].objectType==-1){
                                if(debug==true){
                                    //std::cout<<i<<" ("<<UI[i].X<<", "<<UI[i].Y<<") intersects with "<<iP<<" ("<<UI[iP].X<<", "<<UI[iP].Y<<") !(bounding box)\n";
                                }

                                UI[i].collidedbox=true;
                                UI[iP].collidedbox=true;
                                output = SATUI(i,iP);
                                if(output.difference<-1||( (cornerDistCheck(i).distance < 10&&UI[iP].objectType==-1) || ( cornerDistCheck(iP).distance < 10&&UI[i].objectType==-1) &&cursorMode=="edit")){
                                    UI[i].collidedSAT=true;
                                    UI[iP].collidedSAT=true;
                                    collisionResponse(i,iP);
                                    if(debug==true){
                                        std::cout<<i<<" ("<<UI[i].X<<", "<<UI[i].Y<<") intersects with "<<iP<<" ("<<UI[iP].X<<", "<<UI[iP].Y<<") !(SAT)\n";
                                    }
                                }
                            }
                        }
                    }

                }

            }
        }

        //SAT calculations
        SATout SATUI(int o1,int o2){
            SATout output;
            returnXY normal;
            float max1;
            float min1;
            float max2;
            float min2;
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
        SATout SATLoopUI(int o1,int o2,int times,SATout output){
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
        SATout SATLoopCirclePolyUI(int o1, int o2, int times,SATout output){
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
        //returns offset of specified point from shape center
        returnXY angleOffsetUI(int i,int point){//should return the point as an offset from the center of the shape with X and Y values

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

                //std::cout<<"point: "<<point<<"\ndeg: " << radToDeg(rad)<<"\ndist: "<< dist <<"\nbaseUnit: "<<baseUnit<<"\n";
            }else if(convexShapePoly(UI[i].objectType)){
                float dist = sqrt(square(UI[i].pointList[point*2])+square(UI[i].pointList[point*2+1]))*baseUnit*UI[i].sizeModifier;
                float rad =asin(UI[i].pointList[point*2+1]*baseUnit*UI[i].sizeModifier/dist);

                float x=UI[i].pointList[point*2];
                float y=UI[i].pointList[point*2+1];
                //std::cout<<"\n\npoint: "<<point<<"\n\nXch: "<<X<<"\nYch: "<<Y<<"\n\nXpoint: "<<UI[i].pointList[point*2]<<"\nYpoint: "<<UI[i].pointList[point*2+1]<<"\n\nx: "<<x<<"\ny: "<<y<<"\n";

                if(congruent(UI[i].pointList[point*2],UI[i].pointList[point*2+1])){
                    if(x<0){
                        output.x=dist*sin(-degToRad(UI[i].rotation+90)+rad);
                        output.y=dist*cos(-degToRad(UI[i].rotation+90)+rad);
                    }else{
                        output.x=dist*sin(-degToRad(UI[i].rotation-90)-rad);
                        output.y=dist*cos(-degToRad(UI[i].rotation-90)-rad);
                    }

                    //std::cout<<"point: "<<point<<"\n\n";
                }else{
                    if(x<0){
                        output.x=dist*sin(-degToRad(UI[i].rotation+90)+rad);
                        output.y=dist*cos(-degToRad(UI[i].rotation+90)+rad);
                    }else{
                        output.x=dist*sin(-degToRad(UI[i].rotation-90)-rad);
                        output.y=dist*cos(-degToRad(UI[i].rotation-90)-rad);
                    }

                }

                //std::cout<<"point: "<<point<<"\n";
                //std::cout<<"rotation: "<<rot<<"\n";

                //std::cout<<"point: "<<point<<"\nX: "<<x<<"\nY: "<<y<<"\ninX: "<<UI[i].pointList[point*2]<<"\ninY: "<<UI[i].pointList[point*2+1]<<"\n\n";
            }

            //std::cout<<"object type: "<<UI[i].objectType<<"\n";

            return output;
        }
        float getMinXUI(int object){

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
        float getMinYUI(int object){

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
        float getMaxXUI(int object){

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
        float getMaxYUI(int object){

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
        maxMin getMaxNormalUI(int object,returnXY normal){
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
        maxMin getMinNormalUI(int object,returnXY normal){
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
