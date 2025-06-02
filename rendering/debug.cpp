#include "../headers/game.hpp"
void game::debuger(sf::RenderTarget& window,int i){
    sf::CircleShape pointNotButter(4,20);
    pointNotButter.setOrigin({4, 1});

    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    outline[0].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[1].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[2].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[3].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[4].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);


    if(objects[i].collidedbox==true){
        outline[0].color = sf::Color(0xff0000ff);
        outline[1].color = sf::Color(0xff0000ff);
        outline[2].color = sf::Color(0xff0000ff);
        outline[3].color = sf::Color(0xff0000ff);
        outline[4].color = sf::Color(0xff0000ff);
        objects[i].collidedbox=false;
    }

    window.draw(outline);

    //drawing points
    if((objects[i].objectType==0||objects[i].objectType==1)&&objects[i].sides<=10){

        for(int iP=0;iP<objects[i].sides;iP++){
            sf::CircleShape pointNotButter(10/zoomAMT,20);
            pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==2||objects[i].objectType==4){
        for(int iP=0;iP<4;iP++){
            sf::CircleShape pointNotButter(10/zoomAMT,20);
            pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==3){
        for(int iP=0;iP<objects[i].points;iP++){
            sf::CircleShape pointNotButter(10/zoomAMT,20);
            pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==-1||objects[i].objectType==-2){
        sf::CircleShape pointNotButter(10/zoomAMT,20);
        pointNotButter.setOrigin({10/zoomAMT, 10/zoomAMT});


        pointNotButter.setPosition({(baseUnit*objects[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*objects[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
        pointNotButter.setFillColor(sf::Color(255,0,0));

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(mX*baseUnit),static_cast<float>(mY*baseUnit)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(-mX*baseUnit),static_cast<float>(mY*baseUnit)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(-mX*baseUnit),static_cast<float>(-mY*baseUnit)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(mX*baseUnit),static_cast<float>(-mY*baseUnit)});

        window.draw(pointNotButter);
    }
    pointNotButter.setFillColor(sf::Color(0x00ff00ff));
    pointNotButter.setPosition({objects[i].pointProjected.x*baseUnit+camOffsetX+mX*baseUnit,objects[i].pointProjected.y*baseUnit+camOffsetY+mY*baseUnit});

    window.draw(pointNotButter);
    pointNotButter.setPosition({objects[i].pointProjected2.x*baseUnit+camOffsetX+mX*baseUnit,objects[i].pointProjected2.y*baseUnit+camOffsetY+mY*baseUnit});

    window.draw(pointNotButter);


}
void game::debugerUI(sf::RenderTarget& window,int i){
    sf::CircleShape pointNotButter(4,20);
    pointNotButter.setOrigin({4, 1});

    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    outline[0].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMaxYUI(i))+H/2);
    outline[1].position = sf::Vector2f((getMinXUI(i))+W/2,(getMaxYUI(i))+H/2);
    outline[2].position = sf::Vector2f((getMinXUI(i))+W/2,(getMinYUI(i))+H/2);
    outline[3].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMinYUI(i))+H/2);
    outline[4].position = sf::Vector2f((getMaxXUI(i))+W/2,(getMaxYUI(i))+H/2);


    if(UI[i].collidedbox==true){
        outline[0].color = sf::Color(0xff0000ff);
        outline[1].color = sf::Color(0xff0000ff);
        outline[2].color = sf::Color(0xff0000ff);
        outline[3].color = sf::Color(0xff0000ff);
        outline[4].color = sf::Color(0xff0000ff);
        UI[i].collidedbox=false;
    }

    window.draw(outline);

    //drawing points
    if((UI[i].objectType==0||UI[i].objectType==1)&&UI[i].sides<=10){

        for(int iP=0;iP<UI[i].sides;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(UI[i].objectType==2||UI[i].objectType==4){
        for(int iP=0;iP<4;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffsetUI(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(UI[i].objectType==3){
        for(int iP=0;iP<UI[i].points;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffsetUI(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(UI[i].objectType==-1||UI[i].objectType==-2){
        sf::CircleShape pointNotButter(10,20);
        pointNotButter.setOrigin({10, 10});


        pointNotButter.setPosition({(baseUnit*UI[i].X)+W/2, (baseUnit*UI[i].Y)+H/2});
        if(UI[i].objectType==-1){
            pointNotButter.setFillColor(sf::Color(255,0,255));
        }else{
            pointNotButter.setFillColor(sf::Color(255,0,0));
        }


        window.draw(pointNotButter);
    }
    pointNotButter.setFillColor(sf::Color(0x00ff00ff));
    pointNotButter.setPosition({UI[i].pointProjected.x*baseUnit+W/2,UI[i].pointProjected.y*baseUnit});

    window.draw(pointNotButter);
    pointNotButter.setPosition({UI[i].pointProjected2.x*baseUnit+W/2,UI[i].pointProjected2.y*baseUnit});

    window.draw(pointNotButter);


}
//graphs points that are baseUnit appart in spacing
void game::testingLayoutInf(sf::RenderTarget& window){
    sf::CircleShape pointNotButter(1,20);
    pointNotButter.setOrigin({1, 1});
    for(int iX =0;iX*baseUnit<W;iX++){
        for(int iY =0;iY*baseUnit<H;iY++){
            pointNotButter.setPosition({iX*baseUnit, iY*baseUnit});

            window.draw(pointNotButter);

        }
    }
}
