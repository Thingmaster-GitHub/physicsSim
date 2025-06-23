#include "../headers/headers.cpp"
//draws shapes
void game::drawShape(sf::RenderTarget& window,int i){
    //object culling
    if(!(getMinX(i)>-camOffsetX+W||getMaxX(i)<-camOffsetX)||getMinY(i)>-camOffsetY+H||getMaxY(i)<-camOffsetY){
        if(circleShapePoly(i)){


            sf::CircleShape shape(baseUnit*objects[i].sizeModifier*2,objects[i].sides);

            shape.setOrigin({baseUnit*objects[i].sizeModifier*2, baseUnit*objects[i].sizeModifier*2});

            shape.setFillColor(sf::Color(objects[i].color));

            sf::Angle angle = sf::degrees(objects[i].rotation);
            shape.setRotation(angle);

            shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});


            if(objects[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                objects[i].collidedSAT=false;
            }
            shape.setTexture(&objects[i].texture);

            window.draw(shape);
        }else if(TextShapePoly(i)){
            sf::Text shape(objects[i].font);


            //shape.setPosition({(point.x+baseUnit*objects[i].X+camOffsetX)/zoomAMT+W/2, (point.y+baseUnit*objects[i].Y+camOffsetY)/zoomAMT+H/2});
            returnXY point = angleOffset(i,4);

            shape.setPosition({(point.x+objects[i].X+camOffsetX), (point.y+objects[i].Y+camOffsetY)});
            sf::Angle angle = sf::degrees(objects[i].rotation);
            shape.setRotation(angle);

            shape.setFillColor(sf::Color(objects[i].color));
            shape.setCharacterSize(objects[i].sizeModifier*baseUnit/12);

            shape.setString(objects[i].text);

            window.draw(shape);

            std::cout<<"X: "<<objects[i].X<<"\nY: "<<objects[i].Y<<"\n";
        }else if(rectShapePoly(i)&&objects[i].objectType!=-3){
            sf::RectangleShape shape(sf::Vector2f(objects[i].width*baseUnit, objects[i].height*baseUnit));

            shape.setOrigin({objects[i].width*baseUnit/2, objects[i].height*baseUnit/2});

            shape.setFillColor(sf::Color(objects[i].color));

            sf::Angle angle = sf::degrees(objects[i].rotation);
            shape.setRotation(angle);

            shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});

            if(objects[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                objects[i].collidedSAT=false;
            }
            shape.setTexture(&objects[i].texture);

            window.draw(shape);
        }else if(convexShapePoly(i)){
            sf::ConvexShape shape;
            shape.setPointCount(objects[i].points);

            for(int iP = 0; iP<objects[i].points;iP++){
                shape.setPoint(iP, sf::Vector2f(objects[i].pointList[iP*2]*baseUnit*objects[i].sizeModifier, objects[i].pointList[iP*2+1]*baseUnit*objects[i].sizeModifier));
                //std::cout<<"x: "<<objects[i].pointList[iP*2]<<"\ny: "<<objects[i].pointList[iP*2+1]<<"\n";
            }


            shape.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});

            sf::Angle angle = sf::degrees(objects[i].rotation);
            shape.setRotation(angle);

            shape.setFillColor(sf::Color(objects[i].color));

            if(objects[i].collidedSAT==true&&debug==true){
                shape.setFillColor(sf::Color(0xff0000ff));
                objects[i].collidedSAT=false;
            }
            shape.setTexture(&objects[i].texture);

            window.draw(shape);

        }
        if(debug==true){
            std::cout<<"DRAWN!: "<<i<<"\n";
        }

    }
}
