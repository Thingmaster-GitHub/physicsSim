
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
const int W = 1366;
const int H = 768;
int objectCount =15;
float baseUnit = (W/128+H/72)/2;

bool debug = false;

bool physics = true;

float zoomAMT=1;

float jumpCountDown = 1;
int canJump=0;

const bool centerCamera = true;
float camOffsetX=0;
float camOffsetY=0;
bool scrMove=false;
float initXoff;
float initYoff;
float initMX;
float initMY;

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
    //4 is text
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
    returnXY offset = {0,0};
    int layer=0;
    std::string text="lorium ipsum";
    bool selected=false;
    bool clicked =false;
};

std::vector<object> objects;
std::vector<object> UI;

std::vector<int> objectLoadOrder;

class game{
    public:
        //runs program :3
        void run(){
            loadObjectsJSON(UI,"UI.json");
            for(int i=0;i<UI.size();i++){
                if (!UI[i].texture.loadFromFile(UI[i].loc)){
                    //throw error or something
                }
            }
            objectCount = loadObjectsJSON(objects,"save.json");
            initialize();


            sf::ContextSettings settings;
            settings.antiAliasingLevel = 8.0;
            sf::RenderWindow window(sf::VideoMode({W, H}), "game"/*, sf::Style::default, settings*/);





            while (window.isOpen())
            {

                while (const std::optional event = window.pollEvent())
                {

                    if (event->is<sf::Event::Closed>()){
                        window.close();
                    } else if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()){

                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)){
                            zoom(mouseWheelScrolled->delta);
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
                    //use this for text input!
                    //IDK how I'll use this yet, but it'll sure be usefull!
                    if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
                    {
                        if (textEntered->unicode < 128)
                            std::cout << "ASCII character typed: " << static_cast<char>(textEntered->unicode) << std::endl;
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
                        objects[i].X = ((position.x)*zoomAMT-camOffsetX-W/2*zoomAMT)/baseUnit;
                        objects[i].Y = ((position.y)*zoomAMT-camOffsetY-H/2*zoomAMT)/baseUnit;
                        mouseObject=i;
                    }


                    //draws outline of selected object
                    if(objects[i].selected){
                        drawOutline(window,i);
                    }
                    //sets position of grabbd object
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                        if(objects[i].grabbed==true){

                                objects[i].X=objects[mouseObject].X-objects[i].offset.x;
                                objects[i].Y=objects[mouseObject].Y-objects[i].offset.y;

                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete)){
                                    deleteObject(i);
                                }

                        }
                    }else{
                        objects[i].offset.x=0;
                        objects[i].offset.y=0;
                        objects[i].grabbed=false;
                    }
                }
                for(int i = 0;i<UI.size();i++){
                    drawUI(window,objectLoadOrder[i],UI);
                }

                //baseCollision moved to Lclick

                //probably should be in mouse events ^
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
        void Lclick(){

            baseCollision();
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
                for(int i=0;i<objectCount;i++){
                    objects[i].selected=false;
                }
            }else if(clickQ=1){
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                    objects[queriedTrue].selected=true;
                    objects[queriedTrue].grabbed=true;
                    objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                    objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                }else{
                    if(objects[queriedTrue].selected==false){
                        for(int i=0;i<objectCount;i++){
                            objects[i].selected=false;
                        }
                        objects[queriedTrue].selected=true;
                        objects[queriedTrue].grabbed=true;
                        objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
                        objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
                    }else{
                        for(int i=0;i<objectCount;i++){
                            if(objects[i].selected==true){
                                objects[i].grabbed=true;
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
                    //checks if
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
            }
            //resets clicked values
            for(int i=0;i<objectCount;i++){
                objects[i].clicked=false;
            }
            std::cout<<"ran\n";
        }
        //handles keyboard inputs
        void input(const sf::Keyboard::Scan key){
            if(key==sf::Keyboard::Scancode::Equal){
                createObject();
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)){
                if(key==sf::Keyboard::Scancode::Num1){

                    for(int i=0; i<objectCount;i++){
                        if(objects[i].grabbed==true){
                            objects[i].objectType=0;
                            if(objects[i].mass!=-1){
                                objects[i].color=4294967295;
                            }
                            objects[i].sides=3;
                        }
                    }
                }else if(key==sf::Keyboard::Scancode::Num2){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=1;
                                if(objects[i].mass!=-1){
                                    objects[i].color=4278255615;
                                }
                                objects[i].sides=4;
                            }
                    }
                }else if(key==sf::Keyboard::Scancode::Num3){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=2;
                                objects[i].width=20;
                                objects[i].height=10;

                                if(objects[i].mass!=-1){
                                    objects[i].color=4294967295;
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


                                if(objects[i].mass!=-1){
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
                            }
                        }
                }else if(key==sf::Keyboard::Scancode::Num5){
                        for(int i=0; i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].objectType=4;
                            }
                        }
                }else if(key==sf::Keyboard::Scancode::I){
                        for(int i=0;i<objectCount;i++){
                            if(objects[i].grabbed==true){
                                objects[i].gravity=false;
                                objects[i].mass=-1;
                                objects[i].color=0x777777ff;
                            }
                        }
                }
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
            LayerObjects();
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
            }
            LayerObjects();
        }
        void zoom(float ammount){
            if(ammount<0){
                zoomAMT*=(-1*ammount/10)+1;
            }else{
                zoomAMT/=(ammount/10)+1;
            }
            std::cout<<zoomAMT<<"\n";
        }
        //draws outline
        void drawOutline(sf::RenderTarget& window,int i){
            sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

            outline[0].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);
            outline[1].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);
            outline[2].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+W/2,(getMinY(i)+camOffsetY)/zoomAMT+H/2);
            outline[3].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMinY(i)+camOffsetY)/zoomAMT+H/2);
            outline[4].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+W/2,(getMaxY(i)+camOffsetY)/zoomAMT+H/2);

            window.draw(outline);
        }
        //saves objects to json files
        void saveObjectsJSON(const std::vector<object>& objects, const std::string& filename) {
            nlohmann::json j;

            for (size_t i = 0; i < objectCount; ++i) {
                const auto& obj = objects[i];
                j.push_back({
                    {"X", obj.X}, {"Y", obj.Y}, {"velX", obj.velX}, {"velY", obj.velY},{"velRot", obj.velRot},
                    {"rotation", obj.rotation}, {"sides", obj.sides}, {"sizeModifier", obj.sizeModifier},
                    {"objectType", obj.objectType}, {"gravity", obj.gravity},{"airRes", obj.airRes},{"solid",obj.solid}, {"mass", obj.mass},
                    {"color", obj.color}, {"width", obj.width}, {"height", obj.height},
                    {"points", obj.points}, {"pointList", std::vector<float>(obj.pointList, obj.pointList + 30)},{"coefficentOfFriction",obj.coefficentOfFriction},{"layer",obj.layer},{"text",obj.text},
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

                if (!obj.texture.loadFromFile(obj.loc)) {
                    std::cerr << "Failed to load texture: " << obj.loc << '\n';
                }

                objectsVect.push_back(obj);
            }
            return objectsVect.size();


        }
        //controls camera
        //calculated projected offset of shape for rotation
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
            if(circleShapePoly(i)){


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
                }else if(rectShapePoly(i)){
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
                }else if(convexShapePoly(i)){
                    sf::ConvexShape shape;
                    shape.setPointCount(scene[i].points);

                    for(int iP = 0; iP<scene[i].points;iP++){
                        shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier), (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)));
                        //std::cout<<"x: "<<objects[i].pointList[iP*2]<<"\ny: "<<objects[i].pointList[iP*2+1]<<"\n";
                    }


                    shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y+camOffsetY)+H/2});

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setFillColor(sf::Color(scene[i].color));

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&objects[i].texture);

                    window.draw(shape);

                }
                if(debug==true){
                    std::cout<<"DRAWN!: "<<i<<"\n";
                }

        }
        //draws shapes
        void drawShape(sf::RenderTarget& window,int i,std::vector<object>& scene){
            if(!(getMinX(i)/zoomAMT > -camOffsetX/zoomAMT+W/2||getMaxX(i)/zoomAMT < -camOffsetX / zoomAMT-W/2)||getMinY(i)/zoomAMT > -camOffsetY/zoomAMT+H/2||getMaxY(i)/zoomAMT < -camOffsetY / zoomAMT-H/2){
                if(circleShapePoly(i)){


                    sf::CircleShape shape((baseUnit*scene[i].sizeModifier*2)/zoomAMT,scene[i].sides);

                    shape.setOrigin({(baseUnit*scene[i].sizeModifier*2)/zoomAMT, (baseUnit*scene[i].sizeModifier*2)/zoomAMT});

                    shape.setFillColor(sf::Color(scene[i].color));

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+W/2, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+H/2});


                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
                }else if(rectShapePoly(i)){
                    sf::RectangleShape shape(sf::Vector2f((scene[i].width*baseUnit)/zoomAMT, (scene[i].height*baseUnit)/zoomAMT));

                    shape.setOrigin({(scene[i].width*baseUnit/2)/zoomAMT, (scene[i].height*baseUnit/2)/zoomAMT});

                    shape.setFillColor(sf::Color(scene[i].color));

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+W/2, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+H/2});

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&scene[i].texture);

                    window.draw(shape);
                }else if(convexShapePoly(i)){
                    sf::ConvexShape shape;
                    shape.setPointCount(scene[i].points);

                    for(int iP = 0; iP<scene[i].points;iP++){
                        shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier)/zoomAMT, (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)/zoomAMT));
                        //std::cout<<"x: "<<objects[i].pointList[iP*2]<<"\ny: "<<objects[i].pointList[iP*2+1]<<"\n";
                    }


                    shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+W/2, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+H/2});

                    sf::Angle angle = sf::degrees(scene[i].rotation);
                    shape.setRotation(angle);

                    shape.setFillColor(sf::Color(scene[i].color));

                    if(scene[i].collidedSAT==true&&debug==true){
                        shape.setFillColor(sf::Color(0xff0000ff));
                        scene[i].collidedSAT=false;
                    }
                    shape.setTexture(&objects[i].texture);

                    window.draw(shape);

                }
                if(debug==true){
                    std::cout<<"DRAWN!: "<<i<<"\n";
                }
            }

        }

        //should check in reverse order and only return the first collided if the mouse is clicking or dragging, if it if neither, this shouldn't run.
        //FINISH THIS
        //this is TODO
        //searched in reverse do to draw call order.
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
                        //std::cout<<"top Y 1: "<<YMin<<"\ntop Y 2: "<<YMinCh<<"\nbottom Y 1: "<<YMax<<"\nbottom Y 2: "<<YMaxCh<<"\n\n";
                        if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                            if(objects[i].objectType==-1||objects[iP].objectType==-1){
                                if(debug==true){
                                    //std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(bounding box)\n";
                                }

                                objects[i].collidedbox=true;
                                objects[iP].collidedbox=true;
                                output = SAT(i,iP);//unfinished
                                if(output.difference<0-1){
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
            return pointOutput+baseUnit*objects[object].X;
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


            return pointOutput+baseUnit*objects[object].Y;
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
            return pointOutput+baseUnit*objects[object].X;
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

            if(circleShapePoly(object)){
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
            }else if(rectShapePoly(object)){
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
            }else if(convexShapePoly(object)){
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

            if(circleShapePoly(object)){
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
            }else if(rectShapePoly(object)){
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
            }else if(convexShapePoly(object)){
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
            if(objects[object].objectType==2||objects[object].objectType==-3){
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

        //rotates normal vector 90 degrees
        returnXY invertNormal(returnXY normal){
            returnXY output;
            output.x=normal.y;
            output.y=-normal.x;
            return output;
        }

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
            }else if(objects[i].objectType==2){
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
            pointNotButter.setPosition({objects[i].pointProjected.x+camOffsetX+W/2,objects[i].pointProjected.y+camOffsetY+H/2});

            window.draw(pointNotButter);
            pointNotButter.setPosition({objects[i].pointProjected2.x+camOffsetX+W/2,objects[i].pointProjected2.y+camOffsetY+H/2});

            window.draw(pointNotButter);


        }

};


int main()
{
    game app;
    app.run();

    return 0;
}


