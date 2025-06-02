#include "../headers/game.hpp"
//draws points
void game::drawPoints(sf::RenderTarget& window,int o){
    sf::CircleShape pointNotButter(6,4);
    pointNotButter.setOrigin({6,6});
    pointNotButter.setOutlineColor(sf::Color(0,0,0));
    pointNotButter.setOutlineThickness(2.f);
    pointNotButter.setRotation(sf::degrees(45));

    for(int i=0;i<pointCount(o);i++){
        returnXY point = angleOffset(o,i);
        pointNotButter.setPosition({(point.x+baseUnit*objects[o].X+camOffsetX)/zoomAMT+mX*baseUnit , (point.y+baseUnit*objects[o].Y+camOffsetY)/zoomAMT+mY*baseUnit});



        returnXY position = {(baseUnit*objects[mouseObject].X+camOffsetX)/zoomAMT+W/2, (baseUnit*objects[mouseObject].Y+camOffsetY)/zoomAMT+H/2};

        if(sqrt(square((point.x+baseUnit*objects[o].X+camOffsetX)/zoomAMT+W/2-position.x)+square((point.y+baseUnit*objects[o].Y+camOffsetY)/zoomAMT+H/2-position.y))<10){
            pointNotButter.setFillColor(sf::Color(100,100,100));
        }else{
            pointNotButter.setFillColor(sf::Color(255,255,255));
        }
        //pointNotButter.setPosition({position.x , position.y});



        window.draw(pointNotButter);
    }
}
//draws UI
void game::drawUI(sf::RenderTarget& window,int i,std::vector<object>& scene){
    if(circleShapePoly(scene[i].objectType)){


        sf::CircleShape shape((baseUnit*scene[i].sizeModifier*2),scene[i].sides);

        shape.setOrigin({(baseUnit*scene[i].sizeModifier*2), (baseUnit*scene[i].sizeModifier*2)});

        shape.setFillColor(sf::Color(scene[i].color));

        sf::Angle angle = sf::degrees(scene[i].rotation);
        shape.setRotation(angle);

        shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y)+H/2});

        if(scene[i].collidedSAT==true&&debug==true){
            shape.setFillColor(sf::Color(0xff0000ff));
            scene[i].collidedSAT=false;
        }
        shape.setTexture(&scene[i].texture);

        window.draw(shape);
    }else if(TextShapePoly(scene[i].objectType)){
        sf::Text shape(scene[i].font);


        returnXY point = angleOffsetUI(i,4);

        shape.setPosition({(point.x+baseUnit*scene[i].X)+W/2, (point.y+baseUnit*scene[i].Y)+H/2});
        sf::Angle angle = sf::degrees(scene[i].rotation);
        shape.setRotation(angle);

        shape.setFillColor(sf::Color(scene[i].color));
        shape.setCharacterSize(scene[i].sizeModifier*baseUnit/12);
        if((std::rand()%100)<20&&scene[i].txtBoxSelected){
            shape.setString(scene[i].text+scene[i].textExtra+"|");
        }else{
            shape.setString(scene[i].text+scene[i].textExtra);
        }

        window.draw(shape);
    }else if(rectShapePoly(scene[i].objectType)){
        sf::RectangleShape shape(sf::Vector2f((scene[i].width*baseUnit), (scene[i].height*baseUnit)));

        shape.setOrigin({(scene[i].width*baseUnit/2), (scene[i].height*baseUnit/2)});

        shape.setFillColor(sf::Color(scene[i].color));

        sf::Angle angle = sf::degrees(scene[i].rotation);
        shape.setRotation(angle);

        shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y)+H/2});

        if(scene[i].collidedSAT==true&&debug==true){
            shape.setFillColor(sf::Color(0xff0000ff));
            scene[i].collidedSAT=false;
        }
        shape.setTexture(&scene[i].texture);

        window.draw(shape);
    }else if(convexShapePoly(scene[i].objectType)){
        sf::ConvexShape shape;
        shape.setPointCount(scene[i].points);

        for(int iP = 0; iP<scene[i].points;iP++){
            shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier), (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)));

        }


        shape.setPosition({(baseUnit*scene[i].X)+W/2, (baseUnit*scene[i].Y+camOffsetY)+H/2});

        sf::Angle angle = sf::degrees(scene[i].rotation);
        shape.setRotation(angle);

        shape.setFillColor(sf::Color(scene[i].color));

        if(scene[i].collidedSAT==true&&debug==true){
            shape.setFillColor(sf::Color(0xff0000ff));
            scene[i].collidedSAT=false;
        }
        shape.setTexture(&scene[i].texture);

        window.draw(shape);

    }
    if(debug==true){
        std::cout<<"DRAWN!: "<<i<<"\n";
    }

}
//draws shapes
void game::drawShape(sf::RenderTarget& window,int i,std::vector<object>& scene){
    if(!(getMinX(i)/zoomAMT > -camOffsetX/zoomAMT+W/2||getMaxX(i)/zoomAMT < -camOffsetX / zoomAMT-W/2)||getMinY(i)/zoomAMT > -camOffsetY/zoomAMT+H/2||getMaxY(i)/zoomAMT < -camOffsetY / zoomAMT-H/2){
        if(circleShapePoly(scene[i].objectType)){


            sf::CircleShape shape((baseUnit*scene[i].sizeModifier*2)/zoomAMT,scene[i].sides);

            shape.setOrigin({(baseUnit*scene[i].sizeModifier*2)/zoomAMT, (baseUnit*scene[i].sizeModifier*2)/zoomAMT});

            shape.setFillColor(sf::Color(scene[i].color));

            sf::Angle angle = sf::degrees(scene[i].rotation);
            shape.setRotation(angle);

            shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});


            if(scene[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                scene[i].collidedSAT=false;
            }
            shape.setTexture(&scene[i].texture);

            window.draw(shape);
        }else if(TextShapePoly(scene[i].objectType)){
            sf::Text shape(scene[i].font);


            //shape.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
            returnXY point = angleOffset(i,4);

            shape.setPosition({(point.x+baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (point.y+baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});
            sf::Angle angle = sf::degrees(scene[i].rotation);
            shape.setRotation(angle);

            shape.setFillColor(sf::Color(scene[i].color));
            shape.setCharacterSize(scene[i].sizeModifier/zoomAMT*baseUnit/12);
            if((std::rand()%100)<20&&scene[i].txtBoxSelected){
                shape.setString(scene[i].text+"|");
            }else{
                shape.setString(scene[i].text);
            }
            window.draw(shape);
        }else if(rectShapePoly(scene[i].objectType)){
            sf::RectangleShape shape(sf::Vector2f((scene[i].width*baseUnit)/zoomAMT, (scene[i].height*baseUnit)/zoomAMT));

            shape.setOrigin({(scene[i].width*baseUnit/2)/zoomAMT, (scene[i].height*baseUnit/2)/zoomAMT});

            shape.setFillColor(sf::Color(scene[i].color));

            sf::Angle angle = sf::degrees(scene[i].rotation);
            shape.setRotation(angle);

            shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});

            if(scene[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                scene[i].collidedSAT=false;
            }
            shape.setTexture(&scene[i].texture);

            window.draw(shape);
        }else if(convexShapePoly(scene[i].objectType)){
            sf::ConvexShape shape;
            shape.setPointCount(scene[i].points);

            for(int iP = 0; iP<scene[i].points;iP++){
                shape.setPoint(iP, sf::Vector2f((scene[i].pointList[iP*2]*baseUnit*scene[i].sizeModifier)/zoomAMT, (scene[i].pointList[iP*2+1]*baseUnit*scene[i].sizeModifier)/zoomAMT));

            }


            shape.setPosition({(baseUnit*scene[i].X+camOffsetX)/zoomAMT+mX*baseUnit, (baseUnit*scene[i].Y+camOffsetY)/zoomAMT+mY*baseUnit});

            sf::Angle angle = sf::degrees(scene[i].rotation);
            shape.setRotation(angle);

            shape.setFillColor(sf::Color(scene[i].color));

            if(scene[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                scene[i].collidedSAT=false;
            }
            shape.setTexture(&scene[i].texture);

            window.draw(shape);

        }
        if(debug==true){
            std::cout<<"DRAWN!: "<<i<<"\n";
        }
    }

}
//draws outline
void game::drawOutline(sf::RenderTarget& window,int i){
    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    if(cursorMode=="edit"){
        outline[0].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[1].position = sf::Vector2f((getMinX(i)+5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[2].position = sf::Vector2f((getMinX(i)+5+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+5+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[3].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+5+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[4].position = sf::Vector2f((getMaxX(i)-5+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)-5+camOffsetY)/zoomAMT+mY*baseUnit);
    }else{
        outline[0].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[1].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[2].position = sf::Vector2f((getMinX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[3].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMinY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
        outline[4].position = sf::Vector2f((getMaxX(i)+camOffsetX)/zoomAMT+mX*baseUnit,(getMaxY(i)+camOffsetY)/zoomAMT+mY*baseUnit);
    }

    window.draw(outline);
}
void game::winOutline(sf::RenderTarget& window){
    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    outline[0].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[1].position = sf::Vector2f((-W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[2].position = sf::Vector2f((-W/2+camOffsetX)/zoomAMT+mX*baseUnit,(-H/2+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[3].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(-H/2+camOffsetY)/zoomAMT+mY*baseUnit);
    outline[4].position = sf::Vector2f((W/2+camOffsetX)/zoomAMT+mX*baseUnit,(H/2+camOffsetY)/zoomAMT+mY*baseUnit);

    window.draw(outline);
}
