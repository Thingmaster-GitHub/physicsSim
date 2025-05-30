#include "../headers/game.hpp"
#include "../headers/global.hpp"
#include "../headers/structs.hpp"
//loads data into textExtra for UI elements
void game::textSelected(){
    int count=0;
    int selected=0;
    for(int i=0;i<objectCount;i++){
        if(objects[i].selected){
            count++;
            selected=i;
        }
    }
    if(count==1){
        for(int i=0;i<UI.size();i++){
            if(UI[i].txtLbl=="text label"){
                UI[i].textExtra=objects[selected].txtLbl;
            }
            if(UI[i].txtLbl=="Color"){
                std::stringstream sstream;
                sstream << std::hex << objects[selected].color;
                UI[i].textExtra = sstream.str();
            }
            if(UI[i].txtLbl=="layer"){

                UI[i].textExtra = std::to_string(objects[selected].layer);
            }
            if(UI[i].txtLbl=="size"){

                UI[i].textExtra = std::to_string(objects[selected].sizeModifier);
            }
        }
    }
}
//changes text
void game::textCh(char txt){
    int selected=0;
    int count=0;
    for(int iP=0;iP<UI.size();iP++){
        if(UI[iP].txtBoxSelected){
            selected=iP;
            count++;

        }
    }

    for(int i=0;i<objectCount;i++){

        if(objects[i].selected){
            if(objects[i].txtBoxSelected){
                if(txt==0x08){
                    if(objects[i].text.size()!=0){
                        objects[i].text.pop_back();
                    }
                }else{
                    objects[i].text+=txt;
                }
            }else if(count==1){
                //soo manny forr loooops
                //asdijahshfik
                if(UI[selected].txtLbl=="text label"){
                    if(txt==0x08){
                        if(UI[selected].textExtra.size()!=0){
                            UI[selected].textExtra.pop_back();
                        }
                    }else{
                        UI[selected].textExtra+=txt;
                    }
                    objects[i].txtLbl=UI[selected].textExtra;
                }else if(UI[selected].txtLbl=="Color"){
                    if(txt==0x08){
                        if(UI[selected].textExtra.size()!=0){
                            UI[selected].textExtra.pop_back();
                        }
                    }else{
                        UI[selected].textExtra+=txt;
                    }
                    std::stringstream ss;
                    ss << std::hex << UI[selected].textExtra;
                    ss >> objects[i].color;
                }else if(UI[selected].txtLbl=="layer"){
                    if(txt==0x08){
                        if(UI[selected].textExtra.size()!=0){
                            UI[selected].textExtra.pop_back();
                        }
                    }else{
                        UI[selected].textExtra+=txt;
                    }
                    std::stringstream ss;
                    ss << UI[selected].textExtra;
                    ss >> objects[i].layer;
                    LayerObjects();
                }else if(UI[selected].txtLbl=="size"){
                    if(txt==0x08){
                        if(UI[selected].textExtra.size()!=0){
                            UI[selected].textExtra.pop_back();
                        }
                    }else{
                        UI[selected].textExtra+=txt;
                    }
                    std::stringstream ss;
                    ss << UI[selected].textExtra;
                    ss >> objects[i].sizeModifier;
                    LayerObjects();
                }
            }

        }
    }
}
