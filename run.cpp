#include "game.hpp"
#include "structs.hpp"
#include "global.hpp"

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
