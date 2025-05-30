#include "headers/game.hpp"
#include "headers/global.hpp"
#include "headers/structs.hpp"


//things in this file need fixing
//at least when I'm done sorting them


//sets object load order for drawing
void game::LayerObjectsUI(){
    UILoadOrder.clear();
    bool done = false;
    while(!done){
        for(int i=1;i<UI.size();i++){
            if(UI[i].layer<UI[i-1].layer)
                std::swap(UI[i],UI[i-1]);
        }
        for(int i=1;i<UI.size();i++){
            if(UI[i].layer>=UI[i-1].layer)
                done=true;
        }
    }
}
//sets object load order for drawing
void game::LayerObjects(){
    bool done = false;
    while(!done){
        for(int i=1;i<objects.size();i++){
            if(objects[i].layer<objects[i-1].layer)
                std::swap(objects[i],objects[i-1]);
        }
        for(int i=1;i<objects.size();i++){
            if(objects[i].layer>=objects[i-1].layer)
                done=true;
        }
    }
}
