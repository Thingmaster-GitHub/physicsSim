#include "../headers/headers.cpp"

void game::debuger(sf::RenderTarget& window,int i){
    sf::CircleShape pointNotButter(4,20);
    pointNotButter.setOrigin({4, 1});

    sf::VertexArray outline(sf::PrimitiveType::LineStrip,5);

    outline[0].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMaxY(i)+camOffsetY);
    outline[1].position = sf::Vector2f(getMinX(i)+camOffsetX,getMaxY(i)+camOffsetY);
    outline[2].position = sf::Vector2f(getMinX(i)+camOffsetX,getMinY(i)+camOffsetY);
    outline[3].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMinY(i)+camOffsetY);
    outline[4].position = sf::Vector2f(getMaxX(i)+camOffsetX,getMaxY(i)+camOffsetY);


    if(objects[i].collidedbox==true){
        outline[0].color = sf::Color(0xff0000ff);
        outline[1].color = sf::Color(0xff0000ff);
        outline[2].color = sf::Color(0xff0000ff);
        outline[3].color = sf::Color(0xff0000ff);
        outline[4].color = sf::Color(0xff0000ff);
        objects[i].collidedbox=false;
    }

    window.draw(outline);

    if((objects[i].objectType==0||objects[i].objectType==1)&&objects[i].sides<=10){

        for(int iP=0;iP<objects[i].sides;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==2){
        for(int iP=0;iP<4;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==3){
        for(int iP=0;iP<objects[i].points;iP++){
            sf::CircleShape pointNotButter(10,20);
            pointNotButter.setOrigin({10, 10});
            returnXY point = angleOffset(i,iP);

            pointNotButter.setPosition({point.x+objects[i].X+camOffsetX, point.y+objects[i].Y+camOffsetY});
            pointNotButter.setFillColor(sf::Color(255-iP*30,0,0));

            window.draw(pointNotButter);
        }
    }else if(objects[i].objectType==-1||objects[i].objectType==-2){
        sf::CircleShape pointNotButter(10,20);
        pointNotButter.setOrigin({10, 10});


        pointNotButter.setPosition({objects[i].X+camOffsetX, objects[i].Y+camOffsetY});
        pointNotButter.setFillColor(sf::Color(255,0,0));

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(W/2),static_cast<float>(H/2)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(-W/2),static_cast<float>(H/2)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(-W/2),static_cast<float>(-H/2)});

        window.draw(pointNotButter);

        pointNotButter.setPosition({static_cast<float>(W/2),static_cast<float>(-H/2)});

        window.draw(pointNotButter);
    }
    pointNotButter.setFillColor(sf::Color(0x00ff00ff));
    pointNotButter.setPosition({objects[i].pointProjected.x+camOffsetX,objects[i].pointProjected.y+camOffsetY});

    window.draw(pointNotButter);
    pointNotButter.setPosition({objects[i].pointProjected2.x+camOffsetX,objects[i].pointProjected2.y+camOffsetY});

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
            //std::cout<< "\nwa:"<<wa<<"\nha: "<<ha<<"\ncmb: "<<cmb<<"\nmlt: "<<mlt<<"\n";
            //std::cout<<"butter: \niX: "<<iX<<"\niY: "<<iY<<"\nout: \niX: "<<iX*baseUnit<<"\niY: "<<iY*baseUnit<<"\nreq: \nheight: "<<H<<"\nwidth: "<<W<<"\nunits: "<<mlt<<"\n";
        }
    }
}
