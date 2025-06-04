#include "headers/game.hpp"


//things in this file need fixing
//at least when I'm done sorting them

//makes text input unusable if it can't be used in this context (ex. sides on a rectangle)
void game::NoEdit(){
    std::vector<int> selectedObj;
    for(int i=0;i<objects.size();i++){
        if(objects[i].selected){
            selectedObj.push_back(i);
        }
    }
    if(selectedObj.size()==1){
        singleObjNoEdit(selectedObj[0]);
    }else if(selectedObj.size()>1){
        multiObjNoEdit(selectedObj);
    }
}
void game::singleObjNoEdit(int obj){
    std::vector<std::string> key = {"text label","Color","layer","size","sides"};
    std::vector<int> out = typeCheckSingle(obj);
    for(int i=0;i<UI.size();i++){
        if(TextShapePoly(UI[i].objectType)){
            for(int iP=0;iP<key.size();iP++){
                if(key[iP]==UI[i].txtLbl){
                    if(out[iP]==0){
                        if(isVisableQ(UI[i])){
                            toggleVisable(UI[i]);
                        }
                        UI[i].editable=false;
                    }else if(out[iP]==1){
                        if(!isVisableQ(UI[i])){
                            toggleVisable(UI[i]);

                        }
                        if(UI[i].color==0xaaaaaaff){
                            UI[i].color=0xffffffff;
                        }
                        UI[i].editable=true;
                    }else{
                        UI[i].color=0xaaaaaaff;
                        UI[i].editable=false;
                    }
                }
            }
        }
    }
}
//multi object grey out logic
void game::multiObjNoEdit(std::vector<int>& obj){
    std::vector<std::string> key = {"text label","Color","layer","size","sides"};
    std::vector<int> outputValues = typeCheckMulti(obj);
    if(key.size()!=outputValues.size()){
        //throw error or something
    }
    for(int i=0;i<UI.size();i++){
        if(TextShapePoly(UI[i].objectType)){
            for(int iP=0;iP<key.size();iP++){
                if(key[iP]==UI[i].txtLbl){
                    if(outputValues[iP]==0){
                        if(isVisableQ(UI[i])){
                            toggleVisable(UI[i]);
                        }
                        UI[i].editable=false;
                    }else if(outputValues[iP]==1){
                        if(!isVisableQ(UI[i])){
                            toggleVisable(UI[i]);
                        }
                        UI[i].editable=true;
                    }else{
                        UI[i].color=0xaaaaaaff;
                    }
                }
            }
        }
    }
}
//checks if object is visable
bool game::isVisableQ(object& obj){
    if((obj.color & 0x000000ff)!=0){
        return true;
    }
    return false;
}
//toggles object visibility
void game::toggleVisable(object& obj){
    if(isVisableQ(obj)){
        obj.color = obj.color & 0xffffff00;
    }else{
        obj.color += 0x000000ff;
    }
}
//{"text label","Color","layer","size","sides"};
//checks vallid visible text inputs
//0 is invisable 1 is visible, anything else is grey
std::vector<int> game::typeCheckMulti(std::vector<int>& obj){
    std::vector<int> output = {1,1,1,1,1};
    for(int i=0;i<obj.size();i++){
        if(circleShapePoly(objects[obj[i]].objectType)){
            std::vector<int> mal {1,1,1,1,1};
            for(int iP=0;iP<mal.size();iP++){
                output[iP]*=mal[iP];
            }
        }else if(convexShapePoly(objects[obj[i]].objectType)){
            std::vector<int> mal {1,1,1,1,0};
            for(int iP=0;iP<mal.size();iP++){
                output[iP]*=mal[iP];
            }
        }else if(TextShapePoly(objects[obj[i]].objectType)){
            std::vector<int> mal {1,1,1,2,0};
            for(int iP=0;iP<mal.size();iP++){
                output[iP]*=mal[iP];
            }
        }else if(rectShapePoly(objects[obj[i]].objectType)){
            std::vector<int> mal {1,1,1,2,0};
            for(int iP=0;iP<mal.size();iP++){
                output[iP]*=mal[iP];
            }
        }
    }
    return output;
}
std::vector<int> game::typeCheckSingle(int obj){
    std::vector<int> output = {1,1,1,1,1};

        if(circleShapePoly(objects[obj].objectType)){
            std::vector<int> mal {1,1,1,1,1};
            for(int i=0;i<mal.size();i++){
                output[i]*=mal[i];
            }
        }else if(convexShapePoly(objects[obj].objectType)){
            std::vector<int> mal {1,1,1,1,0};
            for(int i=0;i<mal.size();i++){
                output[i]*=mal[i];
            }
        }else if(TextShapePoly(objects[obj].objectType)){
            std::vector<int> mal {1,1,1,2,0};
            for(int i=0;i<mal.size();i++){
                output[i]*=mal[i];
            }
        }else if(rectShapePoly(objects[obj].objectType)){
            std::vector<int> mal {1,1,1,2,0};
            for(int i=0;i<mal.size();i++){
                output[i]*=mal[i];
            }
        }

    return output;
}
