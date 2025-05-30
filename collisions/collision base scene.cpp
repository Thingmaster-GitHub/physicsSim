#include "../headers/game.hpp"
#include "../headers/global.hpp"
#include "../headers/structs.hpp"
//probably need to change how draw calls are handled, like specifing layers somehow, I'll do that later
//checks bounding box collisions and runs SAT if intersects
void game::baseCollision(){
    float XMax;
    float XMin;
    float YMax;
    float YMin;

    float XMaxCh;
    float XMinCh;
    float YMaxCh;
    float YMinCh;
    SATout output;
    for(int i = 0;i<objectCount;i++){
        XMax = getMaxX(i);
        XMin = getMinX(i);
        YMax = getMaxY(i);
        YMin = getMinY(i);
        for(int iP=i+1;iP<objectCount;iP++){

            if(i!=iP){
                XMaxCh = getMaxX(iP);
                XMinCh = getMinX(iP);
                YMaxCh = getMaxY(iP);
                YMinCh = getMinY(iP);

                if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                    if(objects[i].objectType==-1||objects[iP].objectType==-1){
                        if(debug==true){

                        }

                        objects[i].collidedbox=true;
                        objects[iP].collidedbox=true;
                        output = SAT(i,iP);
                        if(output.difference<-1||( (cornerDistCheck(i).distance < 10&&objects[iP].objectType==-1) || (( cornerDistCheck(iP).distance < 10 && objects[i].objectType==-1) &&cursorMode=="edit"))){
                            objects[i].collidedSAT=true;
                            objects[iP].collidedSAT=true;
                            collisionResponse(i,iP);
                            if(debug==true){
                                std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(SAT)\n";
                            }
                        }
                    }
                }
            }

        }

    }
}
//respone to collision
void game::collisionResponse(int o1,int o2){

    //mouse pointer
    if(objects[o1].objectType==-1||objects[o2].objectType==-1){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            if(objects[o1].objectType==-1){
                if(objects[o2].offset.x==0){
                    //objects[o2].grabbed=true;
                    //objects[o2].selected=true;
                    objects[o2].clicked=true;
                    objects[o2].offset.x=objects[o1].X-objects[o2].X;
                    objects[o2].offset.y=objects[o1].Y-objects[o2].Y;
                }
            }else{
                if(objects[o1].offset.x==0){
                    //objects[o1].grabbed=true;
                    //objects[o1].selected=true;
                    objects[o1].clicked=true;
                    objects[o1].offset.x=objects[o2].X-objects[o1].X;
                    objects[o1].offset.y=objects[o2].Y-objects[o1].Y;
                }
            }
        }
    }
}
