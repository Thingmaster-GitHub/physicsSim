#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "headers/headers.cpp"
//runs the program :3
void game::run(){

    returnXY p;
    loadObjectsJSON(objects,"save.json");

    LayerObjects();

    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    typedef std::chrono::high_resolution_clock clock;

    float timediff=20;



    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8.0;
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(W), static_cast<unsigned int>(H)}), "game"/*, sf::Style::Close, settings*/);

    //recenters convex polygons
    for(int i = 0;i<objects.size();i++){
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
        for(int i = 0; i<objects.size();i++){
            drawShape(window,i);

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
