#include "../headers/game.hpp"

//checks bounding box collisions and runs SAT if intersects
void game::baseCollisionUI(){
    float XMax;
    float XMin;
    float YMax;
    float YMin;

    float XMaxCh;
    float XMinCh;
    float YMaxCh;
    float YMinCh;
    SATout output;
    for(int i = 0;i<UI.size();i++){
        XMax = getMaxXUI(i);
        XMin = getMinXUI(i);
        YMax = getMaxYUI(i);
        YMin = getMinYUI(i);

        for(int iP=i+1;iP<UI.size();iP++){

            if(i!=iP){
                XMaxCh = getMaxXUI(iP);
                XMinCh = getMinXUI(iP);
                YMaxCh = getMaxYUI(iP);
                YMinCh = getMinYUI(iP);

                if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                    if(UI[i].objectType==-1||UI[iP].objectType==-1){
                        if(debug==true){

                        }
                        UI[i].collidedbox=true;
                        UI[iP].collidedbox=true;
                        output = SATUI(i,iP);
                        if(output.difference<-1||( ((cornerDistCheck(i).distance < 10&&UI[iP].objectType==-1)&&cursorMode=="edit") || (( cornerDistCheck(iP).distance < 10&&UI[i].objectType==-1) &&cursorMode=="edit"))){
                            UI[i].collidedSAT=true;
                            UI[iP].collidedSAT=true;
                            collisionResponseUI(i,iP);
                            if(debug==true){
                                std::cout<<i<<" ("<<UI[i].X<<", "<<UI[i].Y<<") intersects with "<<iP<<" ("<<UI[iP].X<<", "<<UI[iP].Y<<") !(SAT)\n";
                            }
                        }
                    }
                }
            }

        }

    }
}
//respone to collision
void game::collisionResponseUI(int o1,int o2){

    //mouse pointer
    if(UI[o1].objectType==-1||UI[o2].objectType==-1){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            if(UI[o1].objectType==-1){
                UI[o2].clicked=true;
            }else{
                UI[o1].clicked=true;
            }
        }
    }
}
