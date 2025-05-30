#include "../headers/game.hpp"
#include "../headers/global.hpp"
#include "../headers/structs.hpp"
//controls zoom
void game::zoom(float ammount){
    if(ammount<0){
        zoomAMT*=(-1*ammount/10)+1;
    }else{
        zoomAMT/=(ammount/10)+1;
    }
}
//left click
void game::Lclick(){
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
                if(UI[i].clicked){
                    selected=i;
                    UI[i].txtBoxSelected=false;
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
            if(objects[i].selected){
                check = cornerDistCheck(i);
                if(check.distance<10){
                    pointSelected=true;

                    queriedObjectSelected=i;
                    Ochecked=i;
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
    }else if(clickQ==1){
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
                if(objects[i].clicked){
                    queriedTrue=i;
                }
            }
            objects[queriedTrue].grabbed=true;
            objects[queriedTrue].offset.x=objects[mouseObject].X-objects[queriedTrue].X;
            objects[queriedTrue].offset.y=objects[mouseObject].Y-objects[queriedTrue].Y;
            objects[queriedTrue].selected=true;

        }else{
            //finds top object
            for(int i=0;i<objectCount;i++){
                if(objects[i].clicked){
                    queriedTrue=i;
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
//controls camera
void game::Camera(sf::Vector2i position){
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
