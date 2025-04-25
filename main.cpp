
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
const int W = 1366;
const int H = 768;
int objectCount = 0;
const float baseUnit = (W/128+H/72)/2;

bool debug =    false;

bool physics = true;

float jumpCountDown = 1;
int canJump=0;

const bool centerCamera = true;
float camOffsetX=0;
float camOffsetY=0;

const bool wrap = false;//don't enable with centered camera.

int mouseObject;
float coefficientOfRestitution=0.6;
struct returnXY{
    float x;
    float y;
};
struct maxMin{
    float maxMin;
    returnXY point;
};
struct SATout{
    float difference = -std::numeric_limits<float>::infinity();
    returnXY normal;
    returnXY point1;
    returnXY point2;
};

struct triggerProperties{
    std::string id = "default";
    std::string event = "none";
    //for event "destroy"
    bool destroyO2 = false;
    int typeReq = 1;
    bool isIDReq = false;
    string IDReq = "default"
};

struct object{
    float X = 0;
    float Y = 0;

    float velX=0;
    float velY=0;

    float velRot=0;//times 500
    float rotation=0;


    int sides = 0;//dont't set to 4, just make it a rectangle

    float sizeModifier=2;

    int objectType = 0;
    //-4 is nothing :3
    //-3 is trigger
    //-2 is camera
    //-1 is the mouse pointer
    //0 is object
    //1 is player
    //2 is rectangle
    //3 genaric is convex polygon
    bool gravity=false;
    bool airRes=true;
    bool solid=true;
    float mass=10;//mass of the object DO NOT SET TO ZERO
    unsigned int color = 0xffffffff;
    float width = 0;
    float height = 0;

    int points = 0;
    float pointList[30];//just pretend it's not a max of 30 (15 points :/)
    bool collidedbox = false;
    bool collidedSAT = false;
    bool grabbed=false;
    returnXY pointProjected;
    returnXY pointProjected2;
    float coefficentOfFriction = 0.5;
    triggerProperties trigger = {};
    std::string loc= "blank.png";
    sf::Texture texture;
    int layer=0;
    std::string text = "lorium ipsum";
    std::string fontLoc = "Comic Sans MS.ttf";
    sf::Font font;

};
std::vector<object> objects;
std::vector<int> objectLoadOrder;

class game{
public:
    //runs program :3
    void run(){

        returnXY p;
        objectCount = loadObjectsJSON(objects,"save.json");

        LayerObjects();

        using std::chrono::duration_cast;
        using std::chrono::nanoseconds;
        typedef std::chrono::high_resolution_clock clock;

        float timediff=20;



        sf::ContextSettings settings;
        settings.antiAliasingLevel = 8.0;
        sf::RenderWindow window(sf::VideoMode({W, H}), "game"/*, sf::Style::Close, settings*/);

        //recenters convex polygons
        for(int i = 0;i<objectCount;i++){
            float X=0;
            float Y=0;
            if(objects[i].mass<=0){
                objects[i].mass=std::numeric_limits<float>::infinity();
            }
            for(int iP=0;iP<objects[i].points;iP++){
                X += objects[i].pointList[iP*2];
                Y += objects[i].pointList[iP*2+1];
                //std::cout<<"iP: "<<iP<<"\npoints: "<<objects[i].points<<"\nX: "<<objects[i].pointList[iP*2+1]<<"\nY: "<<objects[i].pointList[iP*2]<<"\n";
            }
            X/=objects[i].points;
            Y/=objects[i].points;

            for(int iP=0;iP<objects[i].points;iP++){
                //std::cout<<"Y: "<<objects[i].pointList[iP*2+1]<<"\n";
                objects[i].pointList[iP*2]-=X;
                objects[i].pointList[iP*2+1]-=Y;

                //std::cout<<"Yc: "<<objects[i].pointList[iP*2+1]<<"\n\n";
            }
            if (!objects[i].texture.loadFromFile(objects[i].loc)){
                //10/0;
                //I'm too lazy to throw an error here
            }
            if (!objects[i].font.openFromFile(objects[i].fontLoc)){
                //throw error or something
            }
        }



        while (window.isOpen())
        {
            auto start = clock::now();

            while (const std::optional event = window.pollEvent())
            {
                // "close requested" event: we close the window
                if (event->is<sf::Event::Closed>())
                    window.close();
            }

            window.clear(sf::Color::Black);
            p = Camera(p.x,p.y,timediff);

            //draws all shapes+transformations
            for(int i = 0; i<objectCount;i++){
                drawShape(window,objectLoadOrder[i]);

                if(debug==true){
                    debuger(window,i);
                }


                if(physics==true){
                    transform(i,timediff+1);
                }

                if(objects[i].objectType==-1){
                    sf::Vector2i position = sf::Mouse::getPosition(window);


                    objects[i].velX=((objects[i].velX*(1))+(-(objects[i].X)+(position.x-camOffsetX)));
                    objects[i].velY=((objects[i].velY*(1))+(-(objects[i].Y)+(position.y-camOffsetY)));

                    objects[i].X = position.x-camOffsetX;
                    objects[i].Y = position.y-camOffsetY;
                    if(objects[i].Y==std::numeric_limits<float>::infinity()||objects[i].X==std::numeric_limits<float>::infinity()){
                        objects[i].X=0;
                        objects[i].Y=0;
                        camOffsetX=0;
                        camOffsetY=0;
                    }

                    if(debug==true){
                        std::cout<<"vel:("<<(objects[i].velX)<<", "<<(objects[i].velY)<<")\n";
                        std::cout<<"pos:("<<objects[i].X<<", "<<objects[i].Y<<")\n";
                    }
                    mouseObject=i;
                }

                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                    if(objects[i].grabbed==true){
                        objects[i].X=objects[mouseObject].X;
                        objects[i].Y=objects[mouseObject].Y;
                        //std::cout<<"VELWHX:"<<objects[mouseObject].velX<<"VELWHY: "<<objects[mouseObject].velY<<"\n";
                        objects[i].velX=objects[mouseObject].velX;
                        objects[i].velY=objects[mouseObject].velY;
                        //std::cout<<objects[i].velY<<","<<objects[i].velX<<"hmm...\n";
                    }
                }else{

                    objects[i].grabbed=false;
                }

                while(objects[i].velRot>360){
                    if(debug==true){
                        std::cout<<"TOO FAR: "<<objects[i].velRot<<" > "<<objects[i].velRot-360<<"\n";
                    }
                    objects[i].velRot-=360;

                }
                while(objects[i].velRot<-360){
                    objects[i].velRot+=360;

                }
                if(objects[i].velRot==std::numeric_limits<float>::infinity()||objects[i].velRot==-std::numeric_limits<float>::infinity()){
                    objects[i].velRot=0;
                }
                if(objects[i].objectType==1){
                    inputs(i,timediff);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape)){
                    if(physics==true){
                        physics=false;
                    }else{
                        physics=true;
                    }
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C)){
                    if(debug==true){
                        debug=false;
                    }else{
                        debug=true;
                    }
                }

            }
            baseCollision(timediff);
            //testingLayoutInf(window);
            window.display();
            auto end = clock::now();
            //std::cout <<1000000000/duration_cast<nanoseconds>(end-start).count() << "\n";//fps
            timediff=1000000000/duration_cast<nanoseconds>(end-start).count()/60;

            jumpDown(timediff);

        }
    }

private:
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
    //loads in objects
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
    //trigger logic
    void trigger(int o1,int o2){

        if(objects[o1].trigger.event=="destroy"){
            if(objects[o1].trigger.typeReq==objects[o2].objectType && (objects[o1].trigger.isIDReq==false || objects[o1].trigger.IDReq==objects[o2].trigger) ){
                for(int i=0;i<objectCount;i++){
                    if(objects[o1].trigger.id==objects[i].trigger.id){
                        objects[i].objectType=-4;
                        objects[i].mass=0;
                        objects[i].gravity=false;
                        objects[i].airRes=false;
                        objects[i].solid=false;
                    }
                }
            }
        }

    }
    //jump countdown
    void jumpDown(float timediff){
        if(jumpCountDown>=0){
            jumpCountDown-=1/timediff/20;
        }
        if(jumpCountDown<0){
            canJump=0;
        }
    }
    //controls inputs
    void inputs(int Object,float timediff){
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
    returnXY Camera(float PX,float PY,float timediff){
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
    void drawShape(sf::RenderTarget& window,int i){
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
    void transform(int i,float timediff){
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
    //checks bounding box collisions and runs SAT if intersects
    void baseCollision(float timediff){
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
                    //std::cout<<"top Y 1: "<<YMin<<"\ntop Y 2: "<<YMinCh<<"\nbottom Y 1: "<<YMax<<"\nbottom Y 2: "<<YMaxCh<<"\n\n";
                    if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                        if(objects[i].solid&&objects[iP].solid){
                            if(debug==true){
                                //std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(bounding box)\n";
                            }

                            objects[i].collidedbox=true;
                            objects[iP].collidedbox=true;
                            output = SAT(i,iP);//unfinished
                            if(output.difference<0-1){
                                objects[i].collidedSAT=true;
                                objects[iP].collidedSAT=true;
                                collisionResponse(i,iP,output,timediff);
                                if(debug==true){
                                    std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(SAT)\n";
                                }
                            }

                        }else if(objects[i].objectType==-3){
                            trigger(i,iP);
                        }if(objects[iP].objectType==-3){
                            trigger(iP,i);
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
        if(objects[o1].sides>10&&objects[o2].sides>10&&sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y))>(baseUnit*objects[o1].sizeModifier*2+baseUnit*objects[o2].sizeModifier*2)){
            output.difference=1;
            return output;
        }else if(objects[o1].sides>10&&objects[o2].sides>10){
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
            output.difference=sqrt(square(objects[o1].X-objects[o2].X)+square(objects[o1].Y-objects[o2].Y))-(baseUnit*objects[o1].sizeModifier*2+baseUnit*objects[o2].sizeModifier*2);
            output.point1.x=objects[o1].X;
            output.point1.y=objects[o1].Y;
            output.point2.x=objects[o2].X;
            output.point2.y=objects[o2].Y;
            return output;
        }else if((objects[o1].sides>10||objects[o2].sides>10)&&((objects[o1].objectType==-1)||(objects[o2].objectType==-1))){

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
                if(circleShapePoly(o1)){
                    output = SATLoopCirclePoly(o1,o2,objects[o1].sides,output);
                    if(output.difference>0){
                        return output;
                    }


                }else if(rectShapePoly(o1)){
                    output = SATLoopCirclePoly(o1,o2,4,output);
                    if(output.difference>0){
                        return output;
                    }
                }else if(convexShapePoly(o1)){
                    output = SATLoopCirclePoly(o1,o2,objects[o1].points,output);
                    if(output.difference>0){
                        return output;
                    }
                }
            }else{
                if(circleShapePoly(o2)){

                    output = SATLoopCirclePoly(o2,o1,objects[o2].sides,output);
                    if(output.difference>0){
                        return output;
                    }

                }else if(rectShapePoly(o2)){
                    output = SATLoopCirclePoly(o2,o1,4,output);
                    if(output.difference>0){
                        return output;
                    }
                }else if(convexShapePoly(o2)){
                    output = SATLoopCirclePoly(o2,o1,objects[o2].points,output);
                    if(output.difference>0){
                        return output;
                    }
                }
            }


        }
        if(circleShapePoly(o1)){
            if(objects[o1].sides<=10){
                output = SATLoop(o1,o2,objects[o2].sides,output);
                if(output.difference>0){
                    return output;
                }
            }

        }else if(rectShapePoly(o1)){
            output = SATLoop(o1,o2,4,output);
            if(output.difference>0){
                return output;
            }
        }else if(convexShapePoly(o1)){
            output = SATLoop(o1,o2,objects[o1].points,output);
            if(output.difference>0){
                return output;
            }
        }

        if(circleShapePoly(o2)){
            if(objects[o2].sides<=10){
                output = SATLoop(o2,o1,objects[o2].sides,output);
            }
            if(output.difference>0){
                return output;
            }

        }else if(rectShapePoly(o2)){
            output = SATLoop(o2,o1,4,output);
            if(output.difference>0){
                return output;
            }
        }else if(convexShapePoly(o2)){
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
                //std::cout<< "\nwa:"<<wa<<"\nha: "<<ha<<"\ncmb: "<<cmb<<"\nmlt: "<<mlt<<"\n";
                //std::cout<<"butter: \niX: "<<iX<<"\niY: "<<iY<<"\nout: \niX: "<<iX*baseUnit<<"\niY: "<<iY*baseUnit<<"\nreq: \nheight: "<<H<<"\nwidth: "<<W<<"\nunits: "<<mlt<<"\n";
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
        int butterSize = 5;
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
            float dist = sqrt(square(objects[i].pointList[point*2])+square(objects[i].pointList[point*2+1]))*baseUnit*objects[i].sizeModifier;
            float rad =asin(objects[i].pointList[point*2+1]*baseUnit*objects[i].sizeModifier/dist);

            float x=objects[i].pointList[point*2];
            float y=objects[i].pointList[point*2+1];
            //std::cout<<"\n\npoint: "<<point<<"\n\nXch: "<<X<<"\nYch: "<<Y<<"\n\nXpoint: "<<objects[i].pointList[point*2]<<"\nYpoint: "<<objects[i].pointList[point*2+1]<<"\n\nx: "<<x<<"\ny: "<<y<<"\n";

            if(congruent(objects[i].pointList[point*2],objects[i].pointList[point*2+1])){
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
    float getMinY(int object){

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
    float getMaxX(int object){

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
    float getMaxY(int object){

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
    maxMin getMaxNormal(int object,returnXY normal){
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
    maxMin getMinNormal(int object,returnXY normal){
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
    void collisionResponse(int o1,int o2,SATout input,float timediff){

        float massDiff1=1,massDiff2=1;
        //if(objects[o1].sides>10&&objects[o2].sides>10){
        //object 1
        if(objects[o1].mass!=std::numeric_limits<float>::infinity()&&objects[o2].mass!=std::numeric_limits<float>::infinity()){
            float massTotal = objects[o1].mass+objects[o2].mass;

            massDiff1=((massTotal-objects[o1].mass)/massTotal);
            massDiff2=((massTotal-objects[o2].mass)/massTotal);
        }else if(objects[o1].mass==std::numeric_limits<float>::infinity()&&objects[o2].mass!=std::numeric_limits<float>::infinity()){
            massDiff1=0;
            massDiff2=1;
        }else if(objects[o2].mass==std::numeric_limits<float>::infinity()&&objects[o1].mass!=std::numeric_limits<float>::infinity()){
            massDiff1=1;
            massDiff2=0;
        }else{
            massDiff1=0;
            massDiff2=0;
        }

        if((objects[o1].X - objects[o2].X)*input.normal.x+(objects[o1].Y - objects[o2].Y)*input.normal.y<0){
            input.normal.x=-input.normal.x;
            input.normal.y=-input.normal.y;
        }

        //mouse pointer
        if(objects[o1].objectType==-1||objects[o2].objectType==-1){
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                if(objects[o1].objectType==-1){
                    objects[o2].grabbed=true;
                    objects[o2].velX+=objects[mouseObject].velX;
                    objects[o2].velY+=objects[mouseObject].velY;
                }else{
                    objects[o1].grabbed=true;
                    objects[o1].velX+=objects[mouseObject].velX;
                    objects[o1].velY+=objects[mouseObject].velY;
                }
            }
        }else/*literally everything else*/{


            //removes intersection.
            objects[o1].X+=(-input.difference*input.normal.x)*massDiff1;

            objects[o1].Y+=(-input.difference*input.normal.y)*massDiff1;



            objects[o2].X-=(-input.difference*input.normal.x)*massDiff2;

            objects[o2].Y-=(-input.difference*input.normal.y)*massDiff2;


            if(objects[o1].objectType==1&&(-input.difference*input.normal.y)<0){
                canJump=1;
                jumpCountDown=1;
            }else if(objects[o2].objectType==1&&-(-input.difference*input.normal.y)<0){
                canJump=1;
                jumpCountDown=1;
            }
            float direction =(-input.difference)/abs(input.difference);




            float Vel;
            float velX1;
            float velY1;
            float velX2;
            float velY2;
            float rot1;
            float rot2;


            //really need to redo this
            //violates laws of thermodynamics
            if(massDiff1!=0&&massDiff2!=0){
                Vel= abs(((objects[o1].velX)*objects[o1].mass+abs(objects[o2].velX)*objects[o2].mass)*input.normal.x+((objects[o1].velY)*objects[o1].mass+(objects[o2].velY)*objects[o2].mass)*input.normal.y);
                velX1=Vel*input.normal.x;
                velY1=Vel*input.normal.y;

                velX2=Vel*input.normal.x;
                velY2=Vel*input.normal.y;

                rot1=VelRotCalc(o1,o2,input,direction);
                rot2=VelRotCalc(o2,o1,input,direction);
            }else if(massDiff2!=0){
                Vel= abs(((objects[o2].velX)*objects[o2].mass)*input.normal.x+((objects[o2].velY)*objects[o2].mass)*input.normal.y);
                velX2=Vel*input.normal.x;
                velY2=Vel*input.normal.y;

                rot2=VelRotCalc(o2,o1,input,direction);
            }else if(massDiff1!=0){
                Vel= abs(((objects[o1].velX)*objects[o1].mass)*input.normal.x+((objects[o1].velY)*objects[o1].mass)*input.normal.y);
                velX1=Vel*input.normal.x;
                velY1=Vel*input.normal.y;

                rot1=VelRotCalc(o1,o2,input,direction);
            }

            if(massDiff1!=0){
                if(debug==true){
                    std::cout<<"rotation: "<<o1<<" "<<rot1<<"\n";

                }
                //Vel-=abs(rot1*Vel/objects[o2].mass*500);
                objects[o1].velRot+=rot1*coefficientOfRestitution*Vel/objects[o1].mass*massDiff1;
                objects[o1].velX+=direction*velX1/objects[o1].mass*coefficientOfRestitution/baseUnit*massDiff1;

                objects[o1].velY+=direction*velY1/objects[o1].mass*coefficientOfRestitution/baseUnit*massDiff1;

            }
            if(massDiff2!=0){
                if(debug==true){

                    std::cout<<"rotation: "<<o2<<" "<<rot2<<"\n";
                }
                //Vel-=abs(rot2*Vel/objects[o2].mass*500);
                objects[o2].velRot+=rot2*coefficientOfRestitution*Vel/objects[o2].mass*massDiff2;
                objects[o2].velX+=-direction*velX2/objects[o2].mass*coefficientOfRestitution/baseUnit*massDiff2;

                objects[o2].velY+=-direction*velY2/objects[o2].mass*coefficientOfRestitution/baseUnit*massDiff2;
            }

        }
        friction(o1,o2,input.normal,timediff);
        friction(o2,o1,input.normal,timediff);


    }
    //returns if object is of type regular polygon(sf::circleShape)
    bool circleShapePoly(int object){
        if(objects[object].objectType==0||objects[object].objectType==1){
            return true;
        }else{
            return false;
        }
    }
    //returns if object is type rectangle
    bool rectShapePoly(int object){
        if(objects[object].objectType==2||objects[object].objectType==-3||objects[object].objectType==4){
            return true;
        }else{
            return false;
        }
    }
    //returns if object is type convex polygon
    bool convexShapePoly(int object){
        if(objects[object].objectType==3){
            return true;
        }else{
            return false;
        }
    }

    bool TextShapePoly(int object){
        if(objects[object].objectType==4){
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
    //don't input an obect besides ones for this calculated SAT input
    //calculated projected offset of shape for rotation
    float VelRotCalc(int o1,int o2,SATout input,int direction){
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
            output=-1*sqrt(abs(output));
        }else{
            output=sqrt(abs(output));
        }

        return output;


    }

    float distance(returnXY point1,returnXY point2){
        return sqrt(square(point1.x-point2.x)+square(point1.y-point2.y));
    }

    void friction(int o1,int o2,returnXY normal,float timediff){
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

    void debuger(sf::RenderTarget& window,int i){
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
        pointNotButter.setPosition({objects[i].pointProjected.x+camOffsetX,objects[i].pointProjected.y+camOffsetY});

        window.draw(pointNotButter);
        pointNotButter.setPosition({objects[i].pointProjected2.x+camOffsetX,objects[i].pointProjected2.y+camOffsetY});

        window.draw(pointNotButter);


    }

};


int main()
{
    game app;
    app.run();

    return 0;
}

