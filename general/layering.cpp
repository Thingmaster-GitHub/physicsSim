#include "../headers/game.hpp"

//sets object load order for drawing
void game::LayerObjectsUI(){
    bool done = false;
    while(!done){
        for(int i=1;i<UI.size();i++){
            if(UI[i].layer<UI[i-1].layer)
                std::swap(UI[i],UI[i-1]);
        }
        bool doneCh=true;
        for(int i=1;i<UI.size();i++){
            if(!(UI[i].layer>=UI[i-1].layer))
                doneCh=false;
        }
        done=doneCh;
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
        bool doneCh=true;
        for(int i=1;i<objects.size();i++){
            if(!(objects[i].layer>=objects[i-1].layer))
                doneCh=false;
        }
        done=doneCh;
    }
}
