#include "../headers/game.hpp"
//pastes
void game::paste(){
    for(int i=0;i<copyArray.size();i++){

        object tmpobj=copyArray[i].coppied;
        tmpobj.selected=true;
        objects[copyArray[i].coppiedObj].selected=false;
        objects[copyArray[i].coppiedObj].grabbed=false;
        objects.push_back(tmpobj);
        objectCount++;
    }
    initialize();
    NoEdit();
}
//copies
void game::copy(){
    copyArray.clear();

    for(int i=0;i<objectCount;i++){
        if(objects[i].selected){
            cpy tmpCpy = {objects[i],i};
            copyArray.push_back(tmpCpy);
        }
    }
}
