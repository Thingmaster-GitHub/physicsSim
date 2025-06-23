#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <SFML/Graphics.hpp>
#include "headers/game.hpp"
#include "headers/global.hpp"


    //controls inputs
    void game::inputs(int Object,float timediff){
        float mal=1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)&&objects[Object].objectType==1){
            mal=2;
        }

        if(canJump==0){
            mal/=4;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space)&&objects[Object].objectType==1&&canJump!=0){
            objects[Object].velY=-1000;
            canJump=0;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)&&objects[Object].objectType==1){
            if(objects[Object].velX>-200*mal){
                objects[Object].velX-=50*mal/timediff;
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)&&objects[Object].objectType==1){
            if(objects[Object].velX<200*mal){
                objects[Object].velX+=50*mal/timediff;
            }
        }
    }






