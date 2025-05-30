#include "headers/game.hpp"
#include "headers/structs.hpp"
#include "headers/global.hpp"

void game::run(){
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
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(W), static_cast<unsigned int>(H)}), "editor"/*, sf::Style::default, settings*/);





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
        Camera(pos);

        //draws all shapes+transformations
        for(int i = 0; i<objectCount;i++){
            drawShape(window,i,objects);


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
            //draws outline of selected object
            if(objects[i].selected){
                drawOutline(window,i);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete)){
                    deleteObject(i);
                    i--;
                    LayerObjects();
                }
            }
        }
        winOutline(window);
        for(int i = 0;i<UI.size();i++){
            drawUI(window,i,UI);
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
//initilizes program
void game::initialize(){
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
//saves objects to json files
//created with the help of chatgpt
void game::saveObjectsJSON(const std::vector<object>& objects, const std::string& filename) {
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
        obj.txtLbl=item["txtLbl"];

        if (!obj.texture.loadFromFile(obj.loc)) {
            std::cerr << "Failed to load texture: " << obj.loc << '\n';
        }

        objectsVect.push_back(obj);
    }
    return objectsVect.size();


}
