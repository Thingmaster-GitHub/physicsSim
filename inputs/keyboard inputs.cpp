#include "../headers/game.hpp"
//handles keyboard inputs
void game::input(const sf::Keyboard::Scan key){
    if(key==sf::Keyboard::Scancode::Equal){
        createObject();
    }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl)||sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl)){
        //if(cursorMode=="select"){
        if(key==sf::Keyboard::Scancode::Num1){

            for(int i=0; i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].objectType=0;
                    if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                        objects[i].color=0xffffffff;
                    }
                    objects[i].sides=3;
                    objects[i].sizeModifier=2;
                }
            }
        }else if(key==sf::Keyboard::Scancode::Num2){
            for(int i=0; i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].objectType=1;
                    if(objects[i].color==16711935||objects[i].color==0xffffffff){
                        objects[i].color=0xff00ffff;
                    }
                    objects[i].sides=4;
                    objects[i].sizeModifier=2;
                }
            }
        }else if(key==sf::Keyboard::Scancode::Num3){
            for(int i=0; i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].objectType=2;
                    objects[i].width=20;
                    objects[i].height=10;

                    if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                        objects[i].color=0xffffffff;
                    }
                }
            }
        }else if(key==sf::Keyboard::Scancode::Num4){
            for(int i=0; i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].objectType=3;

                    objects[i].points=7;

                    objects[i].pointList[0]=1.8942855596542358;
                    objects[i].pointList[1]=4.548571586608887;
                    objects[i].pointList[2]=2.6942856311798096;
                    objects[i].pointList[3]=0.2485712170600891;
                    objects[i].pointList[4]=2.1942856311798096;
                    objects[i].pointList[5]=-1.3714287281036377;
                    objects[i].pointList[6]=-0.5857136845588684;
                    objects[i].pointList[7]=-3.4514284133911133;
                    objects[i].pointList[8]=-1.6557143926620483;
                    objects[i].pointList[9]=-2.4514284133911133;
                    objects[i].pointList[10]=-2.52571439743042;
                    objects[i].pointList[11]=0.26857125759124756;
                    objects[i].pointList[12]=-2.015714406967163;
                    objects[i].pointList[13]=2.208571434020996;


                    if(objects[i].color==0xffffffff||objects[i].color==0xff00ffff){
                        objects[i].color=16711935;
                    }
                    float X=0;
                    float Y=0;
                    for(int iP=0;iP<objects[i].points;iP++){
                        X += objects[i].pointList[iP*2];
                        Y += objects[i].pointList[iP*2+1];

                    }
                    X/=objects[i].points;
                    Y/=objects[i].points;

                    for(int iP=0;iP<objects[i].points;iP++){

                        objects[i].pointList[iP*2]-=X;
                        objects[i].pointList[iP*2+1]-=Y;

                    }
                    objects[i].sizeModifier=2;
                }
            }
        }else if(key==sf::Keyboard::Scancode::Num5){
            for(int i=0; i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].objectType=4;
                    if(objects[i].color==16711935||objects[i].color==0xff00ffff){
                        objects[i].color=0xffffffff;
                    }
                    objects[i].sizeModifier=24;
                    objects[i].width=20;
                    objects[i].height=10;
                }
            }
        }else if(key==sf::Keyboard::Scancode::I){
            for(int i=0;i<objectCount;i++){
                if(objects[i].grabbed==true){
                    objects[i].gravity=false;
                    objects[i].mass=-1;
                }
            }
        }else if(key==sf::Keyboard::Scancode::C){
            copy();
        }else if(key==sf::Keyboard::Scancode::V){
            paste();
        }else if(key==sf::Keyboard::Scancode::D){
            cursorMode="select";
        }else if(key==sf::Keyboard::Scancode::E){
            cursorMode="edit";
        }else if(key==sf::Keyboard::Scancode::B){
            if(debug){
                debug=false;
            }else{
                debug=true;
            }
        }else if(key==sf::Keyboard::Scancode::A){
            for(int i=0;i<objects.size();i++){
                if(objects[i].objectType!=-1||objects[i].objectType!=-2){
                    objects[i].selected=true;
                }
            }
        }
        //}
    }
}

