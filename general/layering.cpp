#include "../headers/headers.cpp"

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
