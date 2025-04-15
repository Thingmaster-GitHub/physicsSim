void debuger(sf::RenderTarget& window,int i){
    sf::CircleShape pointNotButter(4,20);
    pointNotButter.setOrigin({4, 1});

    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    outline[0].position = sf::Vector2f((getMaxX(i))+W/2,(getMaxY(i))+H/2);
    outline[1].position = sf::Vector2f((getMinX(i))+W/2,(getMaxY(i))+H/2);
    outline[2].position = sf::Vector2f((getMinX(i))+W/2,(getMinY(i))+H/2);
    outline[3].position = sf::Vector2f((getMaxX(i))+W/2,(getMinY(i))+H/2);
    outline[4].position = sf::Vector2f((getMaxX(i))+W/2,(getMaxY(i))+H/2);


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
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(UI[i].objectType==3){
        for(int iP=0;iP<UI[i].points;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({(point.x+baseUnit*UI[i].X)+W/2, (point.y+baseUnit*UI[i].Y)+H/2});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(UI[i].objectType==-1||UI[i].objectType==-2){
        sf::CircleShape pointNotButter(10,20);
        pointNotButter.setOrigin({10, 10});


        pointNotButter.setPosition({(baseUnit*UI[i].X)+W/2, (baseUnit*UI[i].Y)+H/2});
        pointNotButter.setFillColor(sf::Color(255,0,0));

        window.draw(pointNotButter);

        pointNotButter.setPosition({W/2,H/2});

        window.draw(pointNotButter);

        pointNotButter.setPosition({-W/2,H/2});

        window.draw(pointNotButter);

        pointNotButter.setPosition({-W/2,-H/2});

        window.draw(pointNotButter);

        pointNotButter.setPosition({W/2,-H/2});

        window.draw(pointNotButter);
    }
    pointNotButter.setFillColor(sf::Color(0x00ff00ff));
    pointNotButter.setPosition({UI[i].pointProjected.x*baseUnit+W/2,UI[i].pointProjected.y*baseUnit+H/2});

    window.draw(pointNotButter);
    pointNotButter.setPosition({UI[i].pointProjected2.x*baseUnit+W/2,UI[i].pointProjected2.y*baseUnit+H/2});

    window.draw(pointNotButter);


}
