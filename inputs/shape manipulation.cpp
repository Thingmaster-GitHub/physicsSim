#include "../headers/game.hpp"

//rotates selected objects
void game::rotateObj(float ammount){
    for(int i=0;i<objectCount;i++){
        if(objects[i].grabbed==true){
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LAlt)){
                objects[i].rotation+=ammount;
            }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)){
                objects[i].rotation+=ammount*20;
            }else{
                objects[i].rotation+=ammount*10;
            }
        }
    }
}
void game::scaleObj(float amt){

    for(int i=0;i<objectCount;i++){
        if(objects[i].selected){
            std::cout<<"amt: "<<amt<<"\nsize: "<<objects[i].sizeModifier<<"\n";
            if(objects[i].sizeModifier>1){
                objects[i].sizeModifier+=amt;
            }else{
                if(amt>0){
                    objects[i].sizeModifier*=amt*1.5;
                }else{
                    objects[i].sizeModifier/=fabs(amt*1.5);
                }
            }
        }
    }
}
//moves covexShape points
void game::convexMvPoint(int o){
    //removes rotation
    //get rid of this if statement for thousands of floating point impercision errors :3
    if(objects[o].rotation!=0){
        for(int i=0;i<pointCount(o);i++){
            returnXY point = angleOffset(o,i);

            objects[o].pointList[i*2]=point.x/objects[o].sizeModifier/baseUnit;
            objects[o].pointList[i*2+1]=point.y/objects[o].sizeModifier/baseUnit;
        }
        objects[o].rotation=0;
    }
    int point = objects[o].grabbedPoint;

    objects[o].pointList[point*2] = (objects[mouseObject].X-objects[o].X)/2;
    objects[o].pointList[point*2+1] = (objects[mouseObject].Y-objects[o].Y)/2;

    //recenter
    float X=0;
    float Y=0;
    if(objects[o].mass<=0){
        objects[o].mass=std::numeric_limits<float>::infinity();
    }
    for(int i=0;i<objects[o].points;i++){
        X += objects[o].pointList[i*2];
        Y += objects[o].pointList[i*2+1];

    }
    X/=objects[o].points;
    Y/=objects[o].points;

    for(int i=0;i<objects[o].points;i++){

        objects[o].pointList[i*2]-=X;
        objects[o].pointList[i*2+1]-=Y;

    }


    objects[o].X+=X*2;
    objects[o].Y+=Y*2;


}
//rectacle move point
void game::rectMvPoint(int o){
    float initW=objects[o].width;
    float initH=objects[o].height;
    //caclulate 2 normals of points from point in object
    returnXY p1;
    if(objects[o].grabbedPoint==4){
        p1 = angleOffset(o,1);
    }else if(objects[o].grabbedPoint==1){
        p1 = angleOffset(o,2);
    }else{
        p1 = angleOffset(o,objects[o].grabbedPoint+1);
    }

    returnXY p2 = {angleOffset(o,objects[o].grabbedPoint)};
    returnXY mousePoint = {objects[mouseObject].X,objects[mouseObject].Y};
    returnXY center = {objects[o].X,objects[o].Y};
    returnXY Opposite;

    if(objects[o].grabbedPoint<3){
        Opposite = {angleOffset(o,objects[o].grabbedPoint+2)};
    }else{
        Opposite = {angleOffset(o,objects[o].grabbedPoint-2)};
    }

    Opposite.x/=baseUnit;
    Opposite.y/=baseUnit;

    Opposite.x+=center.x;
    Opposite.y+=center.y;


    returnXY n1 = getNormal(p1,p2);
    returnXY n2 = invertNormal(n1);

    if(objects[o].grabbedPoint%2==0){
        n2=n1;
        n1 = invertNormal(n1);
    }
    float mouseProj1 = projectPointOntoNormal(mousePoint,n1);
    float mouseProj2 = projectPointOntoNormal(mousePoint,n2);

    float centerProj1 = projectPointOntoNormal(Opposite,n1);
    float centerProj2 = projectPointOntoNormal(Opposite,n2);

    float dist1 = (mouseProj1-centerProj1);
    float dist2 = (mouseProj2-centerProj2);
    //height
    if(dist1<0){
        dist1*=-1;


    }
    //width
    if(dist2<0){

        dist2*=-1;

    }


    float h=objects[o].height+(dist1-initH)/2;
    float w=objects[o].width+(dist2-initW)/2;
    if(h>0){

        objects[o].height = (h);
    }
    if(w>0){
        objects[o].width = (w);
    }
    //need to change position of rectangle so it stays in place visually

    objects[o].pointProjected=Opposite;

    returnXY OPost;
    if(objects[o].grabbedPoint<3){
        OPost = {angleOffset(o,objects[o].grabbedPoint+2)};
    }else{
        OPost = {angleOffset(o,objects[o].grabbedPoint-2)};
    }
    OPost.x/=baseUnit;
    OPost.y/=baseUnit;

    OPost.x+=center.x;
    OPost.y+=center.y;

    objects[o].X+=Opposite.x-OPost.x;
    objects[o].Y+=Opposite.y-OPost.y;

    if(!(objects[o].grabbedPoint == cornerDistCheck(o).point)){
        objects[o].grabbedPoint = cornerDistCheck(o).point;

        objects[o].X-=Opposite.x-OPost.x;
        objects[o].Y-=Opposite.y-OPost.y;


        if(objects[o].grabbedPoint<3){
            OPost = {angleOffset(o,objects[o].grabbedPoint+2)};
        }else{
            OPost = {angleOffset(o,objects[o].grabbedPoint-2)};
        }
        OPost.x/=baseUnit;
        OPost.y/=baseUnit;

        OPost.x+=center.x;
        OPost.y+=center.y;

        objects[o].X+=Opposite.x-OPost.x;
        objects[o].Y+=Opposite.y-OPost.y;
    }


}
//grabbed point movement
void game::grPointMV(int o){
    if(circleShapePoly(objects[o].objectType)){
        objects[o].sizeModifier = sqrt(square(objects[o].X-objects[mouseObject].X)+square(objects[o].Y-objects[mouseObject].Y))/2;
    }else if(rectShapePoly(objects[o].objectType)){
        rectMvPoint(o);
    }else if(convexShapePoly(objects[o].objectType)){
        convexMvPoint(o);
    }
}
//deletes object
void game::deleteObject(int i){

    objects.erase(objects.begin() + i);
    objectCount--;
}
//creates object
void game::createObject(){
    object tmpObj; //{objects[mouseObject].X,objects[mouseObject].Y,0,0,0,0,3,2,0,true,true,true,10,0xffffffff,0,5,0,{},false,false,false,{},{},1}
    tmpObj.sides=3;
    tmpObj.mass=10;
    tmpObj.gravity=true;
    tmpObj.X=objects[mouseObject].X;
    tmpObj.Y=objects[mouseObject].Y;
    objects.push_back(tmpObj);
    objectCount++;
    initialize();
}
