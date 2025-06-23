#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "headers/game.hpp"
#include "headers/global.hpp"
    //sets object load order for drawing
    void game::LayerObjects(){
        objectLoadOrder.clear();

        int maxLoaded=-std::numeric_limits<int>::infinity();
        int minLoaded=std::numeric_limits<int>::infinity();
        for(int i=0;i<objectCount;i++){
            if(objects[i].layer>maxLoaded){
                maxLoaded=objects[i].layer;
            }
            if(objects[i].layer<minLoaded){
                minLoaded=objects[i].layer;
            }
        }

        for(int i=minLoaded;i<=maxLoaded;i++){
            for(int ip=0;ip<objectCount;ip++){
                if(objects[ip].layer==i){
                    objectLoadOrder.push_back(ip);
                }
            }
        }
    }
    //loads in objects
    int game::loadObjectsJSON(std::vector<object>& objectsVect, const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error opening JSON file!\n";
            return 0;
        }

        nlohmann::json j;
        file >> j;
        file.close();

        objectsVect.clear();
        for (const auto& item : j) {
            object obj;
            obj.X = item["X"];
            obj.Y = item["Y"];
            obj.velX = item["velX"];
            obj.velY = item["velY"];
            obj.velRot = item["velRot"];
            obj.rotation = item["rotation"];
            obj.sides = item["sides"];
            obj.sizeModifier = item["sizeModifier"];
            obj.objectType = item["objectType"];
            obj.gravity = item["gravity"];
            obj.airRes = item["airRes"];
            obj.solid = item["solid"];
            obj.mass = item["mass"];
            obj.color = item["color"];
            obj.width = item["width"];
            obj.height = item["height"];
            obj.points = item["points"];
            obj.coefficentOfFriction = item["coefficentOfFriction"];
            obj.loc = item["loc"];
            obj.layer = item["layer"];
            auto pointList = item["pointList"].get<std::vector<float>>();
            std::copy(pointList.begin(), pointList.end(), obj.pointList);

            obj.trigger.id = item["trigger"]["id"];
            obj.trigger.event = item["trigger"]["event"];
            obj.trigger.destroyO2 = item["trigger"]["destroyO2"];
            obj.trigger.typeReq = item["trigger"]["typeReq"];
            obj.text=item["text"];
            obj.layer = item["layer"];

            if (!obj.texture.loadFromFile(obj.loc)) {
                std::cerr << "Failed to load texture: " << obj.loc << '\n';
            }
            obj.X*=baseUnit;
            obj.Y*=baseUnit;
            objectsVect.push_back(obj);
        }
        return objectsVect.size();

    }

    //controls inputs
    void game::inputs(int Object,float timediff){
        float mal=1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)&&objects[Object].objectType==1){
            mal=2;
        }

        if(canJump==0){
            mal/=4;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space)&&objects[Object].objectType==1&&canJump!=0){
            objects[Object].velY=-1000;
            canJump=0;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)&&objects[Object].objectType==1){
            if(objects[Object].velX>-200*mal){
                objects[Object].velX-=50*mal/timediff;
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)&&objects[Object].objectType==1){
            if(objects[Object].velX<200*mal){
                objects[Object].velX+=50*mal/timediff;
            }
        }
    }
    //controls camera
    returnXY game::Camera(float PX,float PY,float timediff){
        for(int i = 0; i<objectCount;i++){
            if(centerCamera==true&&objects[i].objectType==1){
                PX=objects[i].X;
                PY=objects[i].Y;
            }
        }
        for(int i = 0; i<objectCount;i++){
            if(centerCamera==true&&objects[i].objectType==-2){
                objects[i].X=(((objects[i].X)*(timediff*10-1)+(objects[mouseObject].X+PX)/2)/(timediff*10));
                objects[i].Y=(((objects[i].Y)*(timediff*1-1)+(objects[mouseObject].Y+PY)/2)/(timediff*1));
                camOffsetX=-objects[i].X+W/2;
                camOffsetY=-objects[i].Y+H/2;
                if(objects[i].Y==std::numeric_limits<float>::infinity()||objects[i].X==std::numeric_limits<float>::infinity()){
                    objects[i].X=0;
                    objects[i].Y=0;
                    camOffsetX=0;
                    camOffsetY=0;
                }
            }
        }
        returnXY output {PX,PY};
        return output;
    }
    //draws shapes
    void game::drawShape(sf::RenderTarget& window,int i){
        //object culling
        if(!(getMinX(i)>-camOffsetX+W||getMaxX(i)<-camOffsetX)||getMinY(i)>-camOffsetY+H||getMaxY(i)<-camOffsetY){
            if(circleShapePoly(i)){


                sf::CircleShape shape(baseUnit*objects[i].sizeModifier*2,objects[i].sides);

                shape.setOrigin({baseUnit*objects[i].sizeModifier*2, baseUnit*objects[i].sizeModifier*2});

                shape.setFillColor(sf::Color(objects[i].color));

                sf::Angle angle = sf::degrees(objects[i].rotation);
                shape.setRotation(angle);

                shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});


                if(objects[i].collidedSAT==true&&debug==true){
                    shape.setFillColor(sf::Color(0xff0000ff));
                    objects[i].collidedSAT=false;
                }
                shape.setTexture(&objects[i].texture);

                window.draw(shape);
            }else if(TextShapePoly(i)){
                sf::Text shape(objects[i].font);


                //shape.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                returnXY point = angleOffset(i,4);

                shape.setPosition({(point.x+objects[i].X+camOffsetX), (point.y+objects[i].Y+camOffsetY)});
                sf::Angle angle = sf::degrees(objects[i].rotation);
                shape.setRotation(angle);

                shape.setFillColor(sf::Color(objects[i].color));
                shape.setCharacterSize(objects[i].sizeModifier*baseUnit/12);

                shape.setString(objects[i].text);

                window.draw(shape);

                std::cout<<"X: "<<objects[i].X<<"\nY: "<<objects[i].Y<<"\n";
            }else if(rectShapePoly(i)&&objects[i].objectType!=-3){
                sf::RectangleShape shape(sf::Vector2f(objects[i].width*baseUnit, objects[i].height*baseUnit));

                shape.setOrigin({objects[i].width*baseUnit/2, objects[i].height*baseUnit/2});

                shape.setFillColor(sf::Color(objects[i].color));

                sf::Angle angle = sf::degrees(objects[i].rotation);
                shape.setRotation(angle);

                shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});

                if(objects[i].collidedSAT==true&&debug==true){
                    shape.setFillColor(sf::Color(0xff0000ff));
                    objects[i].collidedSAT=false;
                }
                shape.setTexture(&objects[i].texture);

                window.draw(shape);
            }else if(convexShapePoly(i)){
                sf::ConvexShape shape;
                shape.setPointCount(objects[i].points);

                for(int iP = 0; iP<objects[i].points;iP++){
                    shape.setPoint(iP, sf::Vector2f(objects[i].pointList[iP*2]*baseUnit*objects[i].sizeModifier, objects[i].pointList[iP*2+1]*baseUnit*objects[i].sizeModifier));
                    //std::cout<<"x: "<<objects[i].pointList[iP*2]<<"\ny: "<<objects[i].pointList[iP*2+1]<<"\n";
                }


                shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});

                sf::Angle angle = sf::degrees(objects[i].rotation);
                shape.setRotation(angle);

                shape.setFillColor(sf::Color(objects[i].color));

                if(objects[i].collidedSAT==true&&debug==true){
                    shape.setFillColor(sf::Color(0xff0000ff));
                    objects[i].collidedSAT=false;
                }
                shape.setTexture(&objects[i].texture);

                window.draw(shape);

            }
            if(debug==true){
                std::cout<<"DRAWN!: "<<i<<"\n";
            }

        }
    }
    //transforms objects by velocity, gravity, and air resistance.
    void game::transform(int i,float timediff){
        objects[i].X+=objects[i].velX/timediff*baseUnit/500;
        objects[i].Y+=objects[i].velY/timediff*baseUnit/500;
        objects[i].rotation+=objects[i].velRot/timediff/500;
        if(wrap==true){
            if(objects[i].X>W){
                objects[i].X-=W;
            }else if(objects[i].X<0){
                objects[i].X+=W;
            }
            if(objects[i].Y>H){

                objects[i].Y-=H;
                //objects[i].Y=H;
            }else if(objects[i].Y<0){
                objects[i].Y+=H;
            }
        }

        //objects[i].velX = don't know what to do for these yet
        //std::cout<<"timediff: "<<timediff<<"\noutput: "<<10/timediff<<"\n";
        if(objects[i].gravity==true){
            //gravity
            objects[i].velY+=(25/timediff);

        }
        if(objects[i].airRes==true){
            objects[i].velY-=(objects[i].velY/timediff/500/objects[i].mass);
            objects[i].velX-=(objects[i].velX/timediff/500/objects[i].mass);
            objects[i].velRot-=(objects[i].velRot/timediff/100/objects[i].mass);
        }

        //std::cout<<objects[i].velY<<"\n";

    }


    //graphs points that are baseUnit appart in spacing
    void game::testingLayoutInf(sf::RenderTarget& window){
        sf::CircleShape pointNotButter(1,20);
        pointNotButter.setOrigin({1, 1});
        for(int iX =0;iX*baseUnit<W;iX++){
            for(int iY =0;iY*baseUnit<H;iY++){
                pointNotButter.setPosition({iX*baseUnit, iY*baseUnit});

                window.draw(pointNotButter);
                //std::cout<< "\nwa:"<<wa<<"\nha: "<<ha<<"\ncmb: "<<cmb<<"\nmlt: "<<mlt<<"\n";
                //std::cout<<"butter: \niX: "<<iX<<"\niY: "<<iY<<"\nout: \niX: "<<iX*baseUnit<<"\niY: "<<iY*baseUnit<<"\nreq: \nheight: "<<H<<"\nwidth: "<<W<<"\nunits: "<<mlt<<"\n";
            }
        }
    }
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
    //rotates normal vector 90 degrees
    returnXY game::invertNormal(returnXY normal){
        returnXY output;
        output.x=normal.y;
        output.y=-normal.x;
        return output;
    }
    //don't input an obect besides ones for this calculated SAT input
    //calculated projected offset of shape for rotation
    float game::VelRotCalc(int o1,int o2,SATout input){
        //choses point
        float Xaverage = (objects[o1].X+objects[o2].X)/2;
        float Yaverage = (objects[o1].Y+objects[o2].Y)/2;
        returnXY pointAverage = {Xaverage,Yaverage};
        returnXY inPoint;
        if(distance(input.point1,pointAverage)<distance(input.point2,pointAverage)){
            inPoint = input.point1;
        }else{
            inPoint = input.point2;

        }
        if(debug==true){
            objects[o1].pointProjected2=input.point2;
            objects[o1].pointProjected=input.point1;
        }
        returnXY normal = invertNormal(input.normal);

        float projectedOffset = projectPointOntoNormal(inPoint,normal);

        returnXY center;
        center.x=objects[o1].X;
        center.y=objects[o1].Y;

        float projectedCenter = projectPointOntoNormal(center,normal);

        float polarityOffset = projectPointOntoNormal(inPoint,input.normal);
        float polarityCenter = projectPointOntoNormal(center,input.normal);

        float output = projectedCenter-projectedOffset;
        //
        if(polarityCenter-polarityOffset>0){
            output*=-1;
        }


        //if(objects[o2].mass==std::numeric_limits<float>::infinity()&&objects[o1].mass!=std::numeric_limits<float>::infinity()){

        //output*=-1;
        //std::cout<<"polarityOffset: "<<polarityOffset<<"\npolarityCenter: "<<polarityCenter<<"\n";
        /*if(objects[o1].sides%2==0&&objects[o1].sides!=0){
         *                    output*=-1;
    }*/
        //}
        if(output<0){
            //std::cout<<pol<<"\n";
            output=-1*sqrt(fabs(output));
        }else{
            output=sqrt(fabs(output));
        }

        return output;


    }

    float game::distance(returnXY point1,returnXY point2){
        return sqrt(square(point1.x-point2.x)+square(point1.y-point2.y));
    }

    void game::friction(int o1,int o2,returnXY normal,float timediff){
        returnXY inverted = invertNormal(normal);
        returnXY vel = {objects[o1].velX,objects[o1].velY};
        returnXY velNormal = vel;

        float len = sqrt(square(velNormal.x)+square(velNormal.y));
        if(len!=0&&objects[o1].objectType!=-1&&objects[o2].objectType!=-1){




            velNormal.x/=len;
            velNormal.y/=len;

            float frictDist = projectPointOntoNormal(vel,inverted);
            frictDist = frictDist*(objects[o1].coefficentOfFriction+objects[o2].coefficentOfFriction)/2/timediff;
            if(frictDist!=std::numeric_limits<float>::infinity()&&frictDist!=-std::numeric_limits<float>::infinity()){
                objects[o1].velX-=frictDist*inverted.x;
                objects[o1].velY-=frictDist*inverted.y;

                objects[o1].velRot*=objects[o1].coefficentOfFriction;

            }
        }


    }

    void game::debuger(sf::RenderTarget& window,int i){
        sf::CircleShape pointNotButter(4,20);
        pointNotButter.setOrigin({4, 1});

        sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

        outline[0].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMaxY(i)+camOffsetY);
        outline[1].position = sf::Vector2f(getMinX(i)+camOffsetX,getMaxY(i)+camOffsetY);
        outline[2].position = sf::Vector2f(getMinX(i)+camOffsetX,getMinY(i)+camOffsetY);
        outline[3].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMinY(i)+camOffsetY);
        outline[4].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMaxY(i)+camOffsetY);


        if(objects[i].collidedbox==true){
            outline[0].color = sf::Color(0xff0000ff);
            outline[1].color = sf::Color(0xff0000ff);
            outline[2].color = sf::Color(0xff0000ff);
            outline[3].color = sf::Color(0xff0000ff);
            outline[4].color = sf::Color(0xff0000ff);
            objects[i].collidedbox=false;
        }

        window.draw(outline);

        if((objects[i].objectType==0||objects[i].objectType==1)&&objects[i].sides<=10){

            for(int iP=0;iP<objects[i].sides;iP++){
                sf::CircleShape pointNotButter(10,20);
                pointNotButter.setOrigin({10, 10});
                returnXY point = angleOffset(i,iP);

                pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
                pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                window.draw(pointNotButter);
            }
        }else if(objects[i].objectType==2){
            for(int iP=0;iP<4;iP++){
                sf::CircleShape pointNotButter(10,20);
                pointNotButter.setOrigin({10, 10});
                returnXY point = angleOffset(i,iP);

                pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
                pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                window.draw(pointNotButter);
            }
        }else if(objects[i].objectType==3){
            for(int iP=0;iP<objects[i].points;iP++){
                sf::CircleShape pointNotButter(10,20);
                pointNotButter.setOrigin({10, 10});
                returnXY point = angleOffset(i,iP);

                pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
                pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                window.draw(pointNotButter);
            }
        }else if(objects[i].objectType==-1||objects[i].objectType==-2){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});


            pointNotButter.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});
            pointNotButter.setFillColor(sf::Color(255,0,0));

            window.draw(pointNotButter);

            pointNotButter.setPosition({static_cast<float>(W/2),static_cast<float>(H/2)});

            window.draw(pointNotButter);

            pointNotButter.setPosition({static_cast<float>(-W/2),static_cast<float>(H/2)});

            window.draw(pointNotButter);

            pointNotButter.setPosition({static_cast<float>(-W/2),static_cast<float>(-H/2)});

            window.draw(pointNotButter);

            pointNotButter.setPosition({static_cast<float>(W/2),static_cast<float>(-H/2)});

            window.draw(pointNotButter);
        }
        pointNotButter.setFillColor(sf::Color(0x00ff00ff));
        pointNotButter.setPosition({objects[i].pointProjected.x+camOffsetX,objects[i].pointProjected.y+camOffsetY});

        window.draw(pointNotButter);
        pointNotButter.setPosition({objects[i].pointProjected2.x+camOffsetX,objects[i].pointProjected2.y+camOffsetY});

        window.draw(pointNotButter);


    }
