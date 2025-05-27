
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "structs.hpp"
#include "global.hpp"

mX=48;
mY=28;

W = 1280;
H = 720;

objectCount =15;
baseUnit = (W/128+H/72)/2;

debug = false;



zoomAMT=1;

jumpCountDown = 1;
canJump=0;

centerCamera = true;
camOffsetX=0;
camOffsetY=0;
scrMove=false;
initXoff;
initYoff;
initMX;
initMY;

wrap = false;//don't enable with centered camera.

cursorMode = "select";

mouseObject;
coefficientOfRestitution=0.6;


objects;
UI;

UILoadOrder;
objectLoadOrder;
copyArray;
class game{
    public:
        //runs program :3
        void run(){
            loadObjectsJSON(UI,"UI.json");
            for(int i=0;i<UI.size();i++){
                if (!UI[i].texture.loadFromFile(UI[i].loc)){
                    //throw error or something
                }
                if (!UI[i].font.openFromFile(UI[i].fontLoc)){
                    //throw error or something
                }
            }
            objectCount = loadObjectsJSON(objects,"save.json");
            initialize();


            sf::ContextSettings settings;
            settings.antiAliasingLevel = 8.0;
            sf::RenderWindow window(sf::VideoMode({W, H}), "editor"/*, sf::Style::default, settings*/);





            while (window.isOpen())
            {

                while (const std::optional event = window.pollEvent())
                {

                    if (event->is<sf::Event::Closed>()){
                        window.close();
                    } else if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()){

                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)){
                            zoom(mouseWheelScrolled->delta);
                        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                            scaleObj(mouseWheelScrolled->delta);

                        }else{
                            rotateObj(mouseWheelScrolled->delta);
                        }
                    }else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                        input(keyPressed->scancode);
                    }else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()){
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left){
                            Lclick();
                        }
                    }
                    if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
                    {
                        if (textEntered->unicode < 128){
                            textCh(static_cast<char>(textEntered->unicode));
                        }

                    }


                }


                window.clear(sf::Color::Black);
                //shouldn't write it like this
                // I'm too lazy to make this better
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                Camera(mouseObject,pos);

                //draws all shapes+transformations
                for(int i = 0; i<objectCount;i++){
                    drawShape(window,objectLoadOrder[i],objects);


                    if(debug==true){
                        debuger(window,i);
                    }

                    if(objects[i].objectType==-1){
                        sf::Vector2i position = sf::Mouse::getPosition(window);


                        //inverse of draw call in debug
                        //{(baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2}
                        objects[i].X = ((position.x)*zoomAMT-camOffsetX-mX*baseUnit*zoomAMT)/baseUnit;
                        objects[i].Y = ((position.y)*zoomAMT-camOffsetY-mY*baseUnit*zoomAMT)/baseUnit;
                        mouseObject=i;
                    }

                    if(cursorMode=="edit"&&objects[i].pointGrabbed){
                        grPointMV(i);
                    }
                    //draws outline of selected object
                    if(objects[i].selected){
                        drawOutline(window,i);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete)){
                            deleteObject(i);
                        }
                    }
                    //sets position of grabbd object
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                        if(objects[i].grabbed==true){

                                objects[i].X=objects[mouseObject].X-objects[i].offset.x;
                                objects[i].Y=objects[mouseObject].Y-objects[i].offset.y;



                        }
                    }else{
                        objects[i].offset.x=0;
                        objects[i].offset.y=0;
                        objects[i].grabbed=false;
                        objects[i].pointGrabbed=false;
                        objects[i].grabbedPoint=0;
                    }
                }
                winOutline(window);
                for(int i = 0;i<UI.size();i++){
                    drawUI(window,UILoadOrder[i],UI);
                    if(UI[i].objectType==-1){
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        UI[i].X=(position.x-W/2)/baseUnit;
                        UI[i].Y=(position.y-H/2)/baseUnit;

                        //pointNotButter.setPosition({(baseUnit*UI[i].X)+W/2, (baseUnit*UI[i].Y)+H/2});
                    }
                    if(debug){
                        debugerUI(window,i);
                    }
                }
                if(cursorMode=="edit"){
                    for(int i=0;i<objectCount;i++){
                        if(objects[i].selected){
                            drawPoints(window,i);

                        }
                    }
                }



                //testingLayoutInf(window);
                window.display();
            }

            for(int i=0;i<objectCount;i++){
                if(objects[i].mass==std::numeric_limits<float>::infinity()){
                    objects[i].mass=-1;
                }
            }
            saveObjectsJSON(objects,"save.json");
            //saveObjectsJSON(UI,"UI.json");
        }

    private:
        void scaleObj(float amt){

            for(int i=0;i<objectCount;i++){
                if(objects[i].selected){
                    std::cout<<"amt: "<<amt<<"\nsize: "<<objects[i].sizeModifier<<"\n";
                    if(objects[i].sizeModifier>1){
                        objects[i].sizeModifier+=amt;
                    }else{
                        if(amt>0){
                            objects[i].sizeModifier*=amt*1.5;
                        }else{
                            objects[i].sizeModifier/=abs(amt*1.5);
                        }
                    }
                }
            }
        }
        void winOutline(sf::RenderTarget& window){
            sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

            outline[0].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);
            outline[1].position = sf::Vector2f((-W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);
            outline[2].position = sf::Vector2f((-W/2+camOffsetX)/zoomAMT+mX*baseUnit,(-H/2+camOffsetY)/zoomAMT+mY*baseUnit);
            outline[3].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(-H/2+camOffsetY)/zoomAMT+mY*baseUnit);
            outline[4].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);

            window.draw(outline);
        }
        //loads data into textExtra for UI elements
        void textSelected(){
            int count=0;
            int selected=0;
            for(int i=0;i<objectCount;i++){
                if(objects[i].selected){
                    count++;
                    selected=i;
                }
            }
            if(count==1){
                for(int i=0;i<UI.size();i++){
                    if(UI[i].txtLbl=="text label"){
                        UI[i].textExtra=objects[selected].txtLbl;
                    }
                    if(UI[i].txtLbl=="Color"){
                        std::stringstream sstream;
                        sstream << std::hex << objects[selected].color;
                        UI[i].textExtra = sstream.str();
                    }
                    if(UI[i].txtLbl=="layer"){

                        UI[i].textExtra = std::to_string(objects[selected].layer);
                    }
                    if(UI[i].txtLbl=="size"){

                        UI[i].textExtra = std::to_string(objects[selected].sizeModifier);
                    }
                }
            }
        }
        //changes text
        void textCh(char txt){
            int selected=0;
            int count=0;
            for(int iP=0;iP<UI.size();iP++){
                if(UI[iP].txtBoxSelected){
                    selected=iP;
                    count++;

                }
            }

            for(int i=0;i<objectCount;i++){

                if(objects[i].selected){
                    if(objects[i].txtBoxSelected){
                        if(txt==0x08){
                            if(objects[i].text.size()!=0){
                                objects[i].text.pop_back();
                            }
                        }else{
                            objects[i].text+=txt;
                        }
                    }else if(count==1){
                            //soo manny forr loooops
                            //asdijahshfik
                            if(UI[selected].txtLbl=="text label"){
                                if(txt==0x08){
                                    if(UI[selected].textExtra.size()!=0){
                                        UI[selected].textExtra.pop_back();
                                    }
                                }else{
                                    UI[selected].textExtra+=txt;
                                }
                                objects[i].txtLbl=UI[selected].textExtra;
                            }else if(UI[selected].txtLbl=="Color"){
                                if(txt==0x08){
                                    if(UI[selected].textExtra.size()!=0){
                                        UI[selected].textExtra.pop_back();
                                    }
                                }else{
                                    UI[selected].textExtra+=txt;
                                }
                                std::stringstream ss;
                                ss << std::hex << UI[selected].textExtra;
                                ss >> objects[i].color;
                            }else if(UI[selected].txtLbl=="layer"){
                                if(txt==0x08){
                                    if(UI[selected].textExtra.size()!=0){
                                        UI[selected].textExtra.pop_back();
                                    }
                                }else{
                                    UI[selected].textExtra+=txt;
                                }
                                std::stringstream ss;
                                ss << UI[selected].textExtra;
                                ss >> objects[i].layer;
                                LayerObjects();
                            }else if(UI[selected].txtLbl=="size"){
                                if(txt==0x08){
                                    if(UI[selected].textExtra.size()!=0){
                                        UI[selected].textExtra.pop_back();
                                    }
                                }else{
                                    UI[selected].textExtra+=txt;
                                }
                                std::stringstream ss;
                                ss << UI[selected].textExtra;
                                ss >> objects[i].sizeModifier;
                                LayerObjects();
                            }
                    }

                }
            }
        }
        //pastes
        void paste(){
            for(int i=0;i<copyArray.size();i++){

                object tmpobj=copyArray[i].coppied;
                tmpobj.selected=true;
                objects[copyArray[i].coppiedObj].selected=false;
                objects[copyArray[i].coppiedObj].grabbed=false;
                objects.push_back(tmpobj);
                objectCount++;
            }
            initialize();

        }
        //copies
        void copy(){
            copyArray.clear();

            for(int i=0;i<objectCount;i++){
                if(objects[i].selected){
                    cpy tmpCpy = {objects[i],i};
                    copyArray.push_back(tmpCpy);
                }
            }
        }
        //moves covexShape points
        void convexMvPoint(int o){
            //removes rotation
            //get rid of this if statement for thousands of floating point impercision errors :3
            if(objects[o].rotation!=0){
                for(int i=0;i<pointCount(o);i++){
                returnXY point = angleOffset(o,i);

                objects[o].pointList[i*2]=point.x/objects[o].sizeModifier/baseUnit;
                objects[o].pointList[i*2+1]=point.y/objects[o].sizeModifier/baseUnit;
                }
                objects[o].rotation=0;
            }
            int point = objects[o].grabbedPoint;

            objects[o].pointList[point*2] = (objects[mouseObject].X-objects[o].X)/2;
            objects[o].pointList[point*2+1] = (objects[mouseObject].Y-objects[o].Y)/2;

            //recenter
            float X=0;
            float Y=0;
            if(objects[o].mass<=0){
                objects[o].mass=std::numeric_limits<float>::infinity();
            }
            for(int i=0;i<objects[o].points;i++){
                X += objects[o].pointList[i*2];
                Y += objects[o].pointList[i*2+1];

            }
            X/=objects[o].points;
            Y/=objects[o].points;

            for(int i=0;i<objects[o].points;i++){

                objects[o].pointList[i*2]-=X;
                objects[o].pointList[i*2+1]-=Y;

            }


            objects[o].X+=X*2;
            objects[o].Y+=Y*2;


        }
        //rectacle move point
        void rectMvPoint(int o){
            float initW=objects[o].width;
            float initH=objects[o].height;
            //caclulate 2 normals of points from point in object
            returnXY p1;
            if(objects[o].grabbedPoint==4){
                p1 = angleOffset(o,1);
            }else if(objects[o].grabbedPoint==1){
                p1 = angleOffset(o,2);
            }else{
                p1 = angleOffset(o,objects[o].grabbedPoint+1);
            }

            returnXY p2 = {angleOffset(o,objects[o].grabbedPoint)};
            returnXY mousePoint = {objects[mouseObject].X,objects[mouseObject].Y};
            returnXY center = {objects[o].X,objects[o].Y};
            returnXY Opposite;

            if(objects[o].grabbedPoint<3){
                Opposite = {angleOffset(o,objects[o].grabbedPoint+2)};
            }else{
                Opposite = {angleOffset(o,objects[o].grabbedPoint-2)};
            }

            Opposite.x/=baseUnit;
            Opposite.y/=baseUnit;

            Opposite.x+=center.x;
            Opposite.y+=center.y;


            returnXY n1 = getNormal(p1,p2);
            returnXY n2 = invertNormal(n1);

            if(objects[o].grabbedPoint%2==0){
                n2=n1;
                n1 = invertNormal(n1);
            }
            float mouseProj1 = projectPointOntoNormal(mousePoint,n1);
            float mouseProj2 = projectPointOntoNormal(mousePoint,n2);

            float centerProj1 = projectPointOntoNormal(Opposite,n1);
            float centerProj2 = projectPointOntoNormal(Opposite,n2);

            float dist1 = (mouseProj1-centerProj1);
            float dist2 = (mouseProj2-centerProj2);
            //height
            if(dist1<0){
                dist1*=-1;


            }
            //width
            if(dist2<0){

                dist2*=-1;

            }


            float h=objects[o].height+(dist1-initH)/2;
            float w=objects[o].width+(dist2-initW)/2;
            if(h>0){

                objects[o].height = (h);
            }
            if(w>0){
                objects[o].width = (w);
            }
            //need to change position of rectangle so it stays in place visually

            objects[o].pointProjected=Opposite;

            returnXY OPost;
            if(objects[o].grabbedPoint<3){
                OPost = {angleOffset(o,objects[o].grabbedPoint+2)};
            }else{
                OPost = {angleOffset(o,objects[o].grabbedPoint-2)};
            }
            OPost.x/=baseUnit;
            OPost.y/=baseUnit;

            OPost.x+=center.x;
            OPost.y+=center.y;

            objects[o].X+=Opposite.x-OPost.x;
            objects[o].Y+=Opposite.y-OPost.y;

            if(!(objects[o].grabbedPoint == cornerDistCheck(o).point)){
                objects[o].grabbedPoint = cornerDistCheck(o).point;

                objects[o].X-=Opposite.x-OPost.x;
                objects[o].Y-=Opposite.y-OPost.y;


                if(objects[o].grabbedPoint<3){
                    OPost = {angleOffset(o,objects[o].grabbedPoint+2)};
                }else{
                    OPost = {angleOffset(o,objects[o].grabbedPoint-2)};
                }
                OPost.x/=baseUnit;
                OPost.y/=baseUnit;

                OPost.x+=center.x;
                OPost.y+=center.y;

                objects[o].X+=Opposite.x-OPost.x;
                objects[o].Y+=Opposite.y-OPost.y;
            }


        }
        //grabbed point movement
        void grPointMV(int o){
            if(circleShapePoly(objects[o].objectType)){
                objects[o].sizeModifier = sqrt(square(objects[o].X-objects[mouseObject].X)+square(objects[o].Y-objects[mouseObject].Y))/2;
            }else if(rectShapePoly(objects[o].objectType)){
                rectMvPoint(o);
            }else if(convexShapePoly(objects[o].objectType)){
                convexMvPoint(o);
            }
        }
        //returns the distance to nearest corner of specified shape
        pointDist cornerDistCheck(int o){
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
        int pointCount(int i){
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
        //draws points
        void drawPoints(sf::RenderTarget& window,int o){
            sf::CircleShape pointNotButter(6,4);
            pointNotButter.setOrigin({6,6});
            pointNotButter.setOutlineColor(sf::Color(0,0,0));
            pointNotButter.setOutlineThickness(2.f);
            pointNotButter.setRotation(sf::degrees(45));

            for(int i=0;i<pointCount(o);i++){
                returnXY point = angleOffset(o,i);
                pointNotButter.setPosition({(point.x+baseUnit*objects[o].X+camOffsetX)/zoomAMT+mX*baseUnit , (point.y+baseUnit*objects[o].Y+camOffsetY)/zoomAMT+mY*baseUnit});



                returnXY position = {(baseUnit*objects[mouseObject].X+camOffsetX)/zoomAMT+W/2, (baseUnit*objects[mouseObject].Y+camOffsetY)/zoomAMT+H/2};

                if(sqrt(square((point.x+baseUnit*objects[o].X+camOffsetX)/zoomAMT+W/2-position.x)+square((point.y+baseUnit*objects[o].Y+camOffsetY)/zoomAMT+H/2-position.y))<10){
                    pointNotButter.setFillColor(sf::Color(100,100,100));
                }else{
                    pointNotButter.setFillColor(sf::Color(255,255,255));
                }
                //pointNotButter.setPosition({position.x , position.y});



                window.draw(pointNotButter);
            }
        }
        //left click
        void Lclick(){
            //deselects text box
            for(int i=0;i<objectCount;i++){
                objects[i].txtBoxSelected=false;
            }
            for(int i=0;i<UI.size();i++){
                UI[i].txtBoxSelected=false;
            }

                baseCollisionUI();
                int count=0;
                for(int i=0;i<UI.size();i++){
                    if(UI[i].clicked){
                        count++;
                    }
                }
                if(count==0){
                    baseCollision();
                }else{
                    if(count>1){
                        //asiufgdsiuhvjk time to code somthing I did't plan on or want to do!
                        //selecting UI elements
                        //yipee
                        // :(
                        int selected=0;
                        for(int i=0;i<UI.size();i++){
                            if(UI[UILoadOrder[i]].clicked){
                                selected=i;
                                UI[UILoadOrder[i]].txtBoxSelected=false;
                            }
                        }
                        //have fun reading my spaghetti code

                        if(TextShapePoly(UI[selected].objectType)){
                            UI[selected].txtBoxSelected=true;
                        }
                    }

                    //unclicks
                    for(int i=0;i<objectCount;i++){
                        objects[i].clicked=false;
                    }
                    for(int i=0;i<UI.size();i++){
                        UI[i].clicked=false;
                    }
                }

                //point selection
                if(cursorMode=="edit"){
                    pointDist check = {};
                    int queriedObjectSelected = 0;
                    bool pointSelected = false;

                    int Ochecked=0;

                    for(int i=0;i<objectCount;i++){
                        if(objects[objectLoadOrder[i]].selected){
                            check = cornerDistCheck(objectLoadOrder[i]);
                            if(check.distance<10){
                                pointSelected=true;

                                queriedObjectSelected=objectLoadOrder[i];
                                Ochecked=objectLoadOrder[i];
                            }
                        }
                    }
                    if(pointSelected){
                        objects[Ochecked].grabbedPoint=check.point;
                        objects[Ochecked].pointGrabbed=true;



                        for(int i=0;i<objectCount;i++){
                            objects[i].clicked=false;
                        }
                        return;
                    }
                }

                //check for clicked object
                int clickQ = 0;
                int queriedTrue=0;
                for(int i=0;i<objectCount;i++){
                    if(objects[i].clicked){
                        clickQ++;
                        queriedTrue=i;
                    }
                }


                if(clickQ<1){
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)==false&&count==0){
                        for(int i=0;i<objectCount;i++){
                            objects[i].selected=false;
                        }
                    }
                }else if(clickQ=1){
                    if(TextShapePoly(objects[queriedTrue].objectType)){
                        objects[queriedTrue].txtBoxSelected=true;
                    }


                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                        if(objects[queriedTrue].selected==true){
                            objects[queriedTrue].selected=false;
                        }else{
                            objects[queriedTrue].selected=true;
                        }
                        if(cursorMode=="select"){
                            objects[queriedTrue].grabbed=true;
                        }
                        objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                        objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                    }else{
                        if(objects[queriedTrue].selected==false){
                            for(int i=0;i<objectCount;i++){
                                objects[i].selected=false;
                            }
                            objects[queriedTrue].selected=true;
                            if(cursorMode=="select"){
                                objects[queriedTrue].grabbed=true;
                            }
                            objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                            objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                        }else{
                            for(int i=0;i<objectCount;i++){
                                if(objects[i].selected==true){
                                    if(cursorMode=="select"){
                                        objects[i].grabbed=true;
                                    }
                                    objects[i].offset.x=objects[mouseObject].X-objects[i].X;
                                    objects[i].offset.y=objects[mouseObject].Y-objects[i].Y;
                                }
                            }

                        }

                    }
                }else if(clickQ>1){
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                        //selects top object

                        for(int i=0;i<objectCount;i++){
                            if(objects[objectLoadOrder[i]].clicked){
                                queriedTrue=objectLoadOrder[i];
                            }
                        }
                        objects[queriedTrue].grabbed=true;
                        objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                        objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                        objects[queriedTrue].selected=true;

                    }else{
                        //finds top object
                        for(int i=0;i<objectCount;i++){
                            if(objects[objectLoadOrder[i]].clicked){
                                queriedTrue=objectLoadOrder[i];
                            }
                        }
                        //checks if selected object isn't selected
                        if(objects[queriedTrue].selected==false){
                            //deselects all
                            for(int i=0;i<objectCount;i++){
                                objects[i].selected=false;
                            }

                            objects[queriedTrue].grabbed=true;
                            objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                            objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                            objects[queriedTrue].selected=true;
                        }else{
                            //sets selected objects to grabbed
                            for(int i=0;i<objectCount;i++){
                                if(objects[i].selected==true){
                                    objects[i].grabbed=true;
                                    objects[i].offset.x=objects[mouseObject].X-objects[i].X;
                                    objects[i].offset.y=objects[mouseObject].Y-objects[i].Y;
                                }
                            }
                        }
                    }
                    if(TextShapePoly(objects[queriedTrue].objectType)){
                        objects[queriedTrue].txtBoxSelected=true;
                    }

                }
                //resets clicked values
                for(int i=0;i<objectCount;i++){
                    objects[i].clicked=false;
                }
                if(count==0){
                    textSelected();
                }


        }
        //handles keyboard inputs
        void input(const sf::Keyboard::Scan key){
            if(key==sf::Keyboard::Scancode::Equal){
                createObject();
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)||sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)){
                //if(cursorMode=="select"){
                    if(key==sf::Keyboard::Scancode::Num1){

                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=0;
                                if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                                    objects[i].color=0xffffffff;
                                }
                                objects[i].sides=3;
                                objects[i].sizeModifier=2;
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::Num2){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=1;
                                if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                                    objects[i].color=0xff00ffff;
                                }
                                objects[i].sides=4;
                                objects[i].sizeModifier=2;
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::Num3){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=2;
                                objects[i].width=20;
                                objects[i].height=10;

                                if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                                    objects[i].color=0xffffffff;
                                }
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::Num4){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=3;

                                objects[i].points=7;

                                objects[i].pointList[0]=1.8942855596542358;
                                objects[i].pointList[1]=4.548571586608887;
                                objects[i].pointList[2]=2.6942856311798096;
                                objects[i].pointList[3]=0.2485712170600891;
                                objects[i].pointList[4]=2.1942856311798096;
                                objects[i].pointList[5]=-1.3714287281036377;
                                objects[i].pointList[6]=-0.5857136845588684;
                                objects[i].pointList[7]=-3.4514284133911133;
                                objects[i].pointList[8]=-1.6557143926620483;
                                objects[i].pointList[9]=-2.4514284133911133;
                                objects[i].pointList[10]=-2.52571439743042;
                                objects[i].pointList[11]=0.26857125759124756;
                                objects[i].pointList[12]=-2.015714406967163;
                                objects[i].pointList[13]=2.208571434020996;


                                if(objects[i].color==0xffffffff||objects[i].color==0xff00ffff){
                                    objects[i].color=16711935;
                                }
                                float X=0;
                                float Y=0;
                                for(int iP=0;iP<objects[i].points;iP++){
                                    X += objects[i].pointList[iP*2];
                                    Y += objects[i].pointList[iP*2+1];

                                }
                                X/=objects[i].points;
                                Y/=objects[i].points;

                                for(int iP=0;iP<objects[i].points;iP++){

                                    objects[i].pointList[iP*2]-=X;
                                    objects[i].pointList[iP*2+1]-=Y;

                                }
                                objects[i].sizeModifier=2;
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::Num5){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=4;
                                if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                                    objects[i].color=0xffffffff;
                                }
                                objects[i].sizeModifier=24;
                                objects[i].width=20;
                                objects[i].height=10;
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::I){
                        for(int i=0;i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].gravity=false;
                                objects[i].mass=-1;
                            }
                        }
                    }else if(key==sf::Keyboard::Scancode::C){
                        copy();
                    }else if(key==sf::Keyboard::Scancode::V){
                        paste();
                    }else if(key==sf::Keyboard::Scancode::D){
                        cursorMode="select";
                    }else if(key==sf::Keyboard::Scancode::E){
                        cursorMode="edit";
                    }
                //}
            }
        }
        //deletes object
        void deleteObject(int i){

            objects.erase(objects.begin() + i);
            objectCount--;
        }
        //creates object
        void createObject(){
            object tmpObj; //{objects[mouseObject].X,objects[mouseObject].Y,0,0,0,0,3,2,0,true,true,true,10,0xffffffff,0,5,0,{},false,false,false,{},{},1}
            tmpObj.sides=3;
            tmpObj.mass=10;
            tmpObj.gravity=true;
            tmpObj.X=objects[mouseObject].X;
            tmpObj.Y=objects[mouseObject].Y;
            objects.push_back(tmpObj);
            objectCount++;
            initialize();
        }
        //sets object load order for drawing
        void LayerObjectsUI(){
            UILoadOrder.clear();

            int maxLoaded=-std::numeric_limits<int>::infinity();
            int minLoaded=std::numeric_limits<int>::infinity();
            for(int i=0;i<UI.size();i++){
                if(UI[i].layer>maxLoaded){
                    maxLoaded=UI[i].layer;
                }
                if(UI[i].layer<minLoaded){
                    minLoaded=UI[i].layer;
                }
            }

            for(int i=minLoaded;i<=maxLoaded;i++){
                for(int ip=0;ip<UI.size();ip++){
                    if(UI[ip].layer==i){
                        UILoadOrder.push_back(ip);
                    }
                }
            }
        }
        //sets object load order for drawing
        void LayerObjects(){
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
        //rotates selected objects
        void rotateObj(float ammount){
            for(int i=0;i<objectCount;i++){
                if(objects[i].grabbed==true){
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LAlt)){
                        objects[i].rotation+=ammount;
                    }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                        objects[i].rotation+=ammount*20;
                    }else{
                        objects[i].rotation+=ammount*10;
                    }
                }
            }
        }
        //initilizes program
        void initialize(){
            for(int i = 0;i<objectCount;i++){
                float X=0;
                float Y=0;
                if(objects[i].mass<=0){
                    objects[i].mass=std::numeric_limits<float>::infinity();
                }
                for(int iP=0;iP<objects[i].points;iP++){
                    X += objects[i].pointList[iP*2];
                    Y += objects[i].pointList[iP*2+1];

                }
                X/=objects[i].points;
                Y/=objects[i].points;

                for(int iP=0;iP<objects[i].points;iP++){

                    objects[i].pointList[iP*2]-=X;
                    objects[i].pointList[iP*2+1]-=Y;

                }
                if (!objects[i].texture.loadFromFile(objects[i].loc)){
                    //throw error or something
                }
                if (!objects[i].font.openFromFile(objects[i].fontLoc)){
                    //throw error or something
                }
            }
            LayerObjects();
            LayerObjectsUI();
        }
        //controls zoom
        void zoom(float ammount){
            if(ammount<0){
                zoomAMT*=(-1*ammount/10)+1;
            }else{
                zoomAMT/=(ammount/10)+1;
            }
        }
        //draws outline
        void drawOutline(sf::RenderTarget& window,int i){
            sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

            if(cursorMode=="edit"){
                outline[0].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[1].position = sf::Vector2f((getMinX(i)+5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[2].position = sf::Vector2f((getMinX(i)+5+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+5+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[3].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+5+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[4].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
            }else{
                outline[0].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[1].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[2].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[3].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
                outline[4].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
            }

            window.draw(outline);
        }
        //saves objects to json files
        //created with the help of chatgpt
        void saveObjectsJSON(const std::vector<object>& objects, const std::string& filename) {
            nlohmann::json j;

            for (size_t i = 0; i < objectCount; ++i) {
                const auto& obj = objects[i];
                j.push_back({
                    {"X", obj.X}, {"Y", obj.Y}, {"velX", obj.velX}, {"velY", obj.velY},{"velRot", obj.velRot},
                    {"rotation", obj.rotation}, {"sides", obj.sides}, {"sizeModifier", obj.sizeModifier},
                    {"objectType", obj.objectType}, {"gravity", obj.gravity},{"airRes", obj.airRes},{"solid",obj.solid}, {"mass", obj.mass},
                    {"color", obj.color}, {"width", obj.width}, {"height", obj.height},
                    {"points", obj.points}, {"pointList", std::vector<float>(obj.pointList, obj.pointList + 30)},{"coefficentOfFriction",obj.coefficentOfFriction},{"layer",obj.layer},{"text",obj.text},{"txtLbl",obj.txtLbl},
                            {"loc", obj.loc},
                            {"trigger", {
                                {"id", obj.trigger.id},
                                {"event", obj.trigger.event},
                                {"destroyO2", obj.trigger.destroyO2},
                                {"typeReq", obj.trigger.typeReq}
                            }}
                });
            }

            std::ofstream file(filename);
            file << j.dump(4); // Pretty-print with indent of 4 spaces
            file.close();
        }
        //loads objects from json file
        //created with the help of chatgpt
        int loadObjectsJSON(std::vector<object>& objectsVect, const std::string& filename) {
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
                obj.txtLbl=item["txtLbl"];

                if (!obj.texture.loadFromFile(obj.loc)) {
                    std::cerr << "Failed to load texture: " << obj.loc << '\n';
                }

                objectsVect.push_back(obj);
            }
            return objectsVect.size();


        }
        //controls camera
        void Camera(int mouseObject,sf::Vector2i position){
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
                if(scrMove==false){
                    scrMove=true;
                    initXoff=camOffsetX;
                    initYoff=camOffsetY;

                    initMX=position.x*zoomAMT;
                    initMY=position.y*zoomAMT;
                }else{

                    camOffsetX=initXoff-initMX+position.x*zoomAMT;
                    camOffsetY=initYoff-initMY+position.y*zoomAMT;
                }
            }else{
                if(scrMove==true){
                    scrMove=false;

                }
            }
        }
        //draws UI
        void drawUI(sf::RenderTarget& window,int i,std::vector<object>& scene){
            if(circleShapePoly(scene[i].objectType)){


                sf::CircleShape shape((baseUnit*scene[i].sizeModifier*2),scene[i].sides);

                shape.setOrigin({(baseUnit*scene[i].sizeModifier*2), (baseUnit*scene[i].sizeModifier*2)});

                shape.setFillColor(sf::Color(scene[i].color));

                sf::Angle angle = sf::degrees(scene[i].rotation);
                shape.setRotation(angle);

                shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y)+H/2});

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
            }else if(TextShapePoly(scene[i].objectType)){
                sf::Text shape(scene[i].font);


                returnXY point = angleOffsetUI(i,4);

                shape.setPosition({(point.x+baseUnit*scene[i].X)+W/2, (point.y+baseUnit*scene[i].Y)+H/2});
                sf::Angle angle = sf::degrees(scene[i].rotation);
                shape.setRotation(angle);

                shape.setFillColor(sf::Color(scene[i].color));
                shape.setCharacterSize(scene[i].sizeModifier*baseUnit/12);
                if((std::rand()%100)<20&&scene[i].txtBoxSelected){
                    shape.setString(scene[i].text+scene[i].textExtra+"|");
                }else{
                    shape.setString(scene[i].text+scene[i].textExtra);
                }

                window.draw(shape);
            }else if(rectShapePoly(scene[i].objectType)){
                    sf::RectangleShape shape(sf::Vector2f((scene[i].width*baseUnit), (scene[i].height*baseUnit)));

                    shape.setOrigin({(scene[i].width*baseUnit/2), (scene[i].height*baseUnit/2)});

                    shape.setFillColor(sf::Color(scene[i].color));

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y)+H/2});

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
            }else if(convexShapePoly(scene[i].objectType)){
                    sf::ConvexShape shape;
                    shape.setPointCount(scene[i].points);

                    for(int iP = 0; iP<scene[i].points;iP++){
                        shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier), (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)));

                    }


                    shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y+camOffsetY)+H/2});

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setFillColor(sf::Color(scene[i].color));

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);

            }
                if(debug==true){
                    std::cout<<"DRAWN!: "<<i<<"\n";
                }

        }
        //draws shapes
        void drawShape(sf::RenderTarget& window,int i,std::vector<object>& scene){
            if(!(getMinX(i)/zoomAMT > -camOffsetX/zoomAMT+W/2||getMaxX(i)/zoomAMT < -camOffsetX / zoomAMT-W/2)||getMinY(i)/zoomAMT > -camOffsetY/zoomAMT+H/2||getMaxY(i)/zoomAMT < -camOffsetY / zoomAMT-H/2){
                if(circleShapePoly(scene[i].objectType)){


                    sf::CircleShape shape((baseUnit*scene[i].sizeModifier*2)/zoomAMT,scene[i].sides);

                    shape.setOrigin({(baseUnit*scene[i].sizeModifier*2)/zoomAMT, (baseUnit*scene[i].sizeModifier*2)/zoomAMT});

                    shape.setFillColor(sf::Color(scene[i].color));

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});


                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
                }else if(TextShapePoly(scene[i].objectType)){
                    sf::Text shape(scene[i].font);


                    //shape.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                    returnXY point = angleOffset(i,4);

                    shape.setPosition({(point.x+baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (point.y+baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setFillColor(sf::Color(scene[i].color));
                    shape.setCharacterSize(scene[i].sizeModifier/zoomAMT*baseUnit/12);
                    if((std::rand()%100)<20&&scene[i].txtBoxSelected){
                        shape.setString(scene[i].text+"|");
                    }else{
                        shape.setString(scene[i].text);
                    }
                    window.draw(shape);
                }else if(rectShapePoly(scene[i].objectType)){
                    sf::RectangleShape shape(sf::Vector2f((scene[i].width*baseUnit)/zoomAMT, (scene[i].height*baseUnit)/zoomAMT));

                    shape.setOrigin({(scene[i].width*baseUnit/2)/zoomAMT, (scene[i].height*baseUnit/2)/zoomAMT});

                    shape.setFillColor(sf::Color(scene[i].color));

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
                }else if(convexShapePoly(scene[i].objectType)){
                    sf::ConvexShape shape;
                    shape.setPointCount(scene[i].points);

                    for(int iP = 0; iP<scene[i].points;iP++){
                        shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier)/zoomAMT, (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)/zoomAMT));

                    }


                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setFillColor(sf::Color(scene[i].color));

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);

                }
                if(debug==true){
                    std::cout<<"DRAWN!: "<<i<<"\n";
                }
            }

        }
        //probably need to change how draw calls are handled, like specifing layers somehow, I'll do that later
        //checks bounding box collisions and runs SAT if intersects
        void baseCollision(){
            float XMax;
            float XMin;
            float YMax;
            float YMin;

            float XMaxCh;
            float XMinCh;
            float YMaxCh;
            float YMinCh;
            SATout output;
            for(int i = 0;i<objectCount;i++){
                XMax = getMaxX(i);
                XMin = getMinX(i);
                YMax = getMaxY(i);
                YMin = getMinY(i);
                for(int iP=i+1;iP<objectCount;iP++){

                    if(i!=iP){
                        XMaxCh = getMaxX(iP);
                        XMinCh = getMinX(iP);
                        YMaxCh = getMaxY(iP);
                        YMinCh = getMinY(iP);

                        if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                            if(objects[i].objectType==-1||objects[iP].objectType==-1){
                                if(debug==true){

                                }

                                objects[i].collidedbox=true;
                                objects[iP].collidedbox=true;
                                output = SAT(i,iP);
                                if(output.difference<-1||( (cornerDistCheck(i).distance < 10&&objects[iP].objectType==-1) || ( cornerDistCheck(iP).distance < 10&&objects[i].objectType==-1) &&cursorMode=="edit")){
                                    objects[i].collidedSAT=true;
                                    objects[iP].collidedSAT=true;
                                    collisionResponse(i,iP);
                                    if(debug==true){
                                        std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(SAT)\n";
                                    }
                                }
                            }
                        }
                    }

                }

            }
        }
        //SAT calculations
        SATout SAT(int o1,int o2){
            SATout output;
            returnXY normal;
            float max1;
            float min1;
            float max2;
            float min2;
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
        SATout SATLoop(int o1,int o2,int times,SATout output){
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
        SATout SATLoopCirclePoly(int o1, int o2, int times,SATout output){
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
        //graphs points that are baseUnit appart in spacing
        void testingLayoutInf(sf::RenderTarget& window){
            sf::CircleShape pointNotButter(1,20);
            pointNotButter.setOrigin({1, 1});
            for(int iX =0;iX*baseUnit<W;iX++){
                for(int iY =0;iY*baseUnit<H;iY++){
                    pointNotButter.setPosition({iX*baseUnit, iY*baseUnit});

                    window.draw(pointNotButter);

                }
            }
        }
        //degrees to radians
        float degToRad(float deg){
            for(int i = 0;deg>360;deg-=360){}
            return deg*(3.14/180);
        }
        //radians to degrees
        float radToDeg(float rad){
            return rad*(180/3.14);
        }
        //returns offset of specified point from shape center
        returnXY angleOffset(int i,int point){//should return the point as an offset from the center of the shape with X and Y values

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


                if(congruent(objects[i].pointList[point*2],objects[i].pointList[point*2+1])){
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
        //squares input
        float square(float n){
            return n*n;
        }
        //if the polarity of both inputs is the same, returns true
        bool congruent(float x,float y){
            if(x<0&&y<0){
                return true;
            }else if(x>0&&y>0){
                return true;
            }else{
                return false;
            }
        }
        //used for bounding box
        //gets minimum X
        float getMinX(int object){

            float pointOutput = angleOffset(object,0).x;
            if(objects[object].sides<=10){
                if(circleShapePoly(objects[object].objectType)){


                    for(int iS=1;iS<objects[object].sides;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.x<pointOutput){
                            pointOutput=pointValue.x;
                        }
                    }

                }else if(rectShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<4;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.x<pointOutput){
                            pointOutput=pointValue.x;
                        }
                    }
                }else if(convexShapePoly(objects[object].objectType)){


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
            if(cursorMode=="edit"){
                return pointOutput+baseUnit*objects[object].X-5;
            }
            return pointOutput+baseUnit*objects[object].X;
        }
        //gets minimum Y
        float getMinY(int object){

            float pointOutput = angleOffset(object,0).y;
            if(objects[object].sides<=10){
                if(circleShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<objects[object].sides;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.y<pointOutput){
                            pointOutput=pointValue.y;
                        }
                    }

                }else if(rectShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<4;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.y<pointOutput){
                            pointOutput=pointValue.y;
                        }
                    }
                }else if(convexShapePoly(objects[object].objectType)){

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


            if(cursorMode=="edit"){
                return pointOutput+baseUnit*objects[object].Y-5;
            }
            return pointOutput+baseUnit*objects[object].Y;
        }
        //gets maximum X
        float getMaxX(int object){

            float pointOutput = angleOffset(object,0).x;
            if(objects[object].sides<=10){
                if(circleShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<objects[object].sides;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.x>pointOutput){
                            pointOutput=pointValue.x;
                        }
                    }

                }else if(rectShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<4;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.x>pointOutput){
                            pointOutput=pointValue.x;
                        }
                    }
                }else if(convexShapePoly(objects[object].objectType)){

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

            if(cursorMode=="edit"){
                return pointOutput+baseUnit*objects[object].X+5;
            }
            return pointOutput+baseUnit*objects[object].X;
        }
        //gets maximum y
        float getMaxY(int object){

            float pointOutput = angleOffset(object,0).y;
            if(objects[object].sides<=10){
                if(circleShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<objects[object].sides;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.y>pointOutput){
                            pointOutput=pointValue.y;
                        }
                    }

                }else if(rectShapePoly(objects[object].objectType)){

                    for(int iS=1;iS<4;iS++){
                        returnXY pointValue = angleOffset(object,iS);
                        if(pointValue.y>pointOutput){
                            pointOutput=pointValue.y;
                        }
                    }
                }else if(convexShapePoly(objects[object].objectType)){

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

            if(cursorMode=="edit"){
                return pointOutput+baseUnit*objects[object].Y+5;
            }
            return pointOutput+baseUnit*objects[object].Y;
        }
        //gets maximum normal of shape
        maxMin getMaxNormal(int object,returnXY normal){
            maxMin output;
            output.maxMin=-std::numeric_limits<float>::infinity();
            float test;
            returnXY testPoint;
            //circle
            if(objects[object].sides>10){
                output.maxMin = baseUnit*objects[object].X*normal.x+baseUnit*objects[object].Y*normal.y+baseUnit*objects[object].sizeModifier*2;
                output.point.x=baseUnit*objects[object].X;
                output.point.y=baseUnit*objects[object].Y;
                return output;
            }

            if(circleShapePoly(objects[object].objectType)){
                for(int i = 0;i<objects[object].sides;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test>output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else if(rectShapePoly(objects[object].objectType)){
                for(int i = 0;i<4;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test>output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else if(convexShapePoly(objects[object].objectType)){
                for(int i = 0;i<objects[object].points;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test>output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else{
                output.point.x=baseUnit*objects[object].X;
                output.point.y=baseUnit*objects[object].Y;

                output.maxMin = projectPointOntoNormal(output.point,normal);
            }
            return output;
        }
        //gets minimum normal of shape
        maxMin getMinNormal(int object,returnXY normal){
            maxMin output;
            output.maxMin=std::numeric_limits<float>::infinity();
            float test;
            returnXY testPoint;
            //circle
            if(objects[object].sides>10){
                output.maxMin = baseUnit*objects[object].X*normal.x+baseUnit*objects[object].Y*normal.y-baseUnit*objects[object].sizeModifier*2;
                output.point.x=baseUnit*objects[object].X;
                output.point.y=baseUnit*objects[object].Y;
                return output;
            }

            if(circleShapePoly(objects[object].objectType)){
                for(int i = 0;i<objects[object].sides;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test<output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else if(rectShapePoly(objects[object].objectType)){
                for(int i = 0;i<4;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test<output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else if(convexShapePoly(objects[object].objectType)){
                for(int i = 0;i<objects[object].points;i++){
                    testPoint = angleOffset(object,i);

                    testPoint.x+=baseUnit*objects[object].X;
                    testPoint.y+=baseUnit*objects[object].Y;
                    test = projectPointOntoNormal(testPoint,normal);

                    if(test<output.maxMin){
                        output.maxMin=test;
                        output.point = testPoint;
                    }
                }
            }else{
                output.point.x=baseUnit*objects[object].X;
                output.point.y=baseUnit*objects[object].Y;

                output.maxMin = projectPointOntoNormal(output.point,normal);
            }
            return output;
        }
        //get normal between two points, just kidding it's rotated 90 degrees and I'm not fixing is cause it's already used too much in my code :/
        returnXY getNormal(returnXY point1,returnXY point2){
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
        float projectPointOntoNormal(const returnXY& point, const returnXY& normal) {
            // Calculate the dot product of the point and the normal
            return point.x * normal.x + point.y * normal.y;
        }
        //respone to collision
        void collisionResponse(int o1,int o2){

            //mouse pointer
            if(objects[o1].objectType==-1||objects[o2].objectType==-1){
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                    if(objects[o1].objectType==-1){
                        if(objects[o2].offset.x==0){
                        //objects[o2].grabbed=true;
                        //objects[o2].selected=true;
                        objects[o2].clicked=true;
                        objects[o2].offset.x=objects[o1].X-objects[o2].X;
                        objects[o2].offset.y=objects[o1].Y-objects[o2].Y;
                        }
                    }else{
                        if(objects[o1].offset.x==0){
                        //objects[o1].grabbed=true;
                        //objects[o1].selected=true;
                        objects[o1].clicked=true;
                        objects[o1].offset.x=objects[o2].X-objects[o1].X;
                        objects[o1].offset.y=objects[o2].Y-objects[o1].Y;
                        }
                    }
                }
            }
        }
        //respone to collision
        void collisionResponseUI(int o1,int o2){

            //mouse pointer
            if(UI[o1].objectType==-1||UI[o2].objectType==-1){
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                    if(UI[o1].objectType==-1){
                        UI[o2].clicked=true;
                    }else{
                        UI[o1].clicked=true;
                    }
                }
            }
        }
        //returns if object is of type regular polygon(sf::circleShape)
        bool circleShapePoly(int type){
            if(type==0||type==1){
                return true;
            }else{
                return false;
            }
        }
        //returns if object is type rectangle
        bool rectShapePoly(int type){
            if(type==2||type==-3||type==4){
                return true;
            }else{
                return false;
            }
        }
        //returns if object is type convex polygon
        bool convexShapePoly(int type){
            if(type==3){
                return true;
            }else{
                return false;
            }
        }
        //returns if object is type text
        bool TextShapePoly(int type){
            if(type==4){
                return true;
            }else{
                return false;
            }
        }
        //rotates normal vector 90 degrees
        returnXY invertNormal(returnXY normal){
            returnXY output;
            output.x=normal.y;
            output.y=-normal.x;
            return output;
        }
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

                for(int iP=i+1;iP<UI.size();iP++){

                    if(i!=iP){
                        XMaxCh = getMaxXUI(iP);
                        XMinCh = getMinXUI(iP);
                        YMaxCh = getMaxYUI(iP);
                        YMinCh = getMinYUI(iP);

                        if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                            if(UI[i].objectType==-1||UI[iP].objectType==-1){
                                if(debug==true){

                                }
                                UI[i].collidedbox=true;
                                UI[iP].collidedbox=true;
                                output = SATUI(i,iP);
                                if(output.difference<-1||( (cornerDistCheck(i).distance < 10&&UI[iP].objectType==-1) || ( cornerDistCheck(iP).distance < 10&&UI[i].objectType==-1) &&cursorMode=="edit")){
                                    UI[i].collidedSAT=true;
                                    UI[iP].collidedSAT=true;
                                    collisionResponseUI(i,iP);
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


            }else if(convexShapePoly(UI[i].objectType)){
                float dist = sqrt(square(UI[i].pointList[point*2])+square(UI[i].pointList[point*2+1]))*baseUnit*UI[i].sizeModifier;
                float rad =asin(UI[i].pointList[point*2+1]*baseUnit*UI[i].sizeModifier/dist);

                float x=UI[i].pointList[point*2];
                float y=UI[i].pointList[point*2+1];


                if(congruent(UI[i].pointList[point*2],UI[i].pointList[point*2+1])){
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
        //debug view
        void debuger(sf::RenderTarget& window,int i){
            sf::CircleShape pointNotButter(4,20);
            pointNotButter.setOrigin({4, 1});

            sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

            outline[0].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);
            outline[1].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);
            outline[2].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+W/2,(getMinY(i)+camOffsetY)/zoomAMT+H/2);
            outline[3].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMinY(i)+camOffsetY)/zoomAMT+H/2);
            outline[4].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);


            if(objects[i].collidedbox==true){
                outline[0].color = sf::Color(0xff0000ff);
                outline[1].color = sf::Color(0xff0000ff);
                outline[2].color = sf::Color(0xff0000ff);
                outline[3].color = sf::Color(0xff0000ff);
                outline[4].color = sf::Color(0xff0000ff);
                objects[i].collidedbox=false;
            }

            window.draw(outline);

            //drawing points
            if((objects[i].objectType==0||objects[i].objectType==1)&&objects[i].sides<=10){

                for(int iP=0;iP<objects[i].sides;iP++){
                    sf::CircleShape pointNotButter(10/zoomAMT,20);
                    pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
                    returnXY point = angleOffset(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(objects[i].objectType==2||objects[i].objectType==4){
                for(int iP=0;iP<4;iP++){
                    sf::CircleShape pointNotButter(10/zoomAMT,20);
                    pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
                    returnXY point = angleOffset(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(objects[i].objectType==3){
                for(int iP=0;iP<objects[i].points;iP++){
                    sf::CircleShape pointNotButter(10/zoomAMT,20);
                    pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
                    returnXY point = angleOffset(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(objects[i].objectType==-1||objects[i].objectType==-2){
                sf::CircleShape pointNotButter(10/zoomAMT,20);
                pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});


                pointNotButter.setPosition({(baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
                pointNotButter.setFillColor(sf::Color(255,0,0));

                window.draw(pointNotButter);

                pointNotButter.setPosition({W/2,H/2});

                window.draw(pointNotButter);

                pointNotButter.setPosition({-W/2,H/2});

                window.draw(pointNotButter);

                pointNotButter.setPosition({-W/2,-H/2});

                window.draw(pointNotButter);

                pointNotButter.setPosition({W/2,-H/2});

                window.draw(pointNotButter);
            }
            pointNotButter.setFillColor(sf::Color(0x00ff00ff));
            pointNotButter.setPosition({objects[i].pointProjected.x*baseUnit+camOffsetX+W/2,objects[i].pointProjected.y*baseUnit+camOffsetY+H/2});

            window.draw(pointNotButter);
            pointNotButter.setPosition({objects[i].pointProjected2.x*baseUnit+camOffsetX+W/2,objects[i].pointProjected2.y*baseUnit+camOffsetY+H/2});

            window.draw(pointNotButter);


        }

        void debugerUI(sf::RenderTarget& window,int i){
            sf::CircleShape pointNotButter(4,20);
            pointNotButter.setOrigin({4, 1});

            sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

            outline[0].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMaxYUI(i))+H/2);
            outline[1].position = sf::Vector2f((getMinXUI(i))+W/2,(getMaxYUI(i))+H/2);
            outline[2].position = sf::Vector2f((getMinXUI(i))+W/2,(getMinYUI(i))+H/2);
            outline[3].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMinYUI(i))+H/2);
            outline[4].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMaxYUI(i))+H/2);


            if(UI[i].collidedbox==true){
                outline[0].color = sf::Color(0xff0000ff);
                outline[1].color = sf::Color(0xff0000ff);
                outline[2].color = sf::Color(0xff0000ff);
                outline[3].color = sf::Color(0xff0000ff);
                outline[4].color = sf::Color(0xff0000ff);
                UI[i].collidedbox=false;
            }

            window.draw(outline);

            //drawing points
            if((UI[i].objectType==0||UI[i].objectType==1)&&UI[i].sides<=10){

                for(int iP=0;iP<UI[i].sides;iP++){
                    sf::CircleShape pointNotButter(10,20);
                    pointNotButter.setOrigin({10, 10});
                    returnXY point = angleOffset(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(UI[i].objectType==2||UI[i].objectType==4){
                for(int iP=0;iP<4;iP++){
                    sf::CircleShape pointNotButter(10,20);
                    pointNotButter.setOrigin({10, 10});
                    returnXY point = angleOffsetUI(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(UI[i].objectType==3){
                for(int iP=0;iP<UI[i].points;iP++){
                    sf::CircleShape pointNotButter(10,20);
                    pointNotButter.setOrigin({10, 10});
                    returnXY point = angleOffsetUI(i,iP);

                    pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
                    pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

                    window.draw(pointNotButter);
                }
            }else if(UI[i].objectType==-1||UI[i].objectType==-2){
                sf::CircleShape pointNotButter(10,20);
                pointNotButter.setOrigin({10, 10});


                pointNotButter.setPosition({(baseUnit*UI[i].X)+W/2, (baseUnit*UI[i].Y)+H/2});
                if(UI[i].objectType==-1){
                    pointNotButter.setFillColor(sf::Color(255,0,255));
                }else{
                    pointNotButter.setFillColor(sf::Color(255,0,0));
                }


                window.draw(pointNotButter);
            }
            pointNotButter.setFillColor(sf::Color(0x00ff00ff));
            pointNotButter.setPosition({UI[i].pointProjected.x*baseUnit+W/2,UI[i].pointProjected.y*baseUnit});

            window.draw(pointNotButter);
            pointNotButter.setPosition({UI[i].pointProjected2.x*baseUnit+W/2,UI[i].pointProjected2.y*baseUnit});

            window.draw(pointNotButter);


        }


};


int main()
{
    game app;
    app.run();

    return 0;
}
