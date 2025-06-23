#include "../headers/headers.cpp"
//respone to collision
void game::collisionResponse(int o1,int o2,SATout input,float timediff){

    float massDiff1=1,massDiff2=1;
    //if(objects[o1].sides>10&&objects[o2].sides>10){
    //object 1
    if(objects[o1].mass!=std::numeric_limits<float>::infinity()&&objects[o2].mass!=std::numeric_limits<float>::infinity()){
        float massTotal = objects[o1].mass+objects[o2].mass;

        massDiff1=((massTotal-objects[o1].mass)/massTotal);
        massDiff2=((massTotal-objects[o2].mass)/massTotal);
    }else if(objects[o1].mass==std::numeric_limits<float>::infinity()&&objects[o2].mass!=std::numeric_limits<float>::infinity()){
        massDiff1=0;
        massDiff2=1;
    }else if(objects[o2].mass==std::numeric_limits<float>::infinity()&&objects[o1].mass!=std::numeric_limits<float>::infinity()){
        massDiff1=1;
        massDiff2=0;
    }else{
        massDiff1=0;
        massDiff2=0;
    }

    if((objects[o1].X - objects[o2].X)*input.normal.x+(objects[o1].Y - objects[o2].Y)*input.normal.y<0){
        input.normal.x=-input.normal.x;
        input.normal.y=-input.normal.y;
    }

    //mouse pointer
    if(objects[o1].objectType==-1||objects[o2].objectType==-1){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            if(objects[o1].objectType==-1){
                objects[o2].grabbed=true;
                objects[o2].velX+=objects[mouseObject].velX;
                objects[o2].velY+=objects[mouseObject].velY;
            }else{
                objects[o1].grabbed=true;
                objects[o1].velX+=objects[mouseObject].velX;
                objects[o1].velY+=objects[mouseObject].velY;
            }
        }
    }else/*literally everything else*/{


        //removes intersection.
        objects[o1].X+=(-input.difference*input.normal.x)*massDiff1;

        objects[o1].Y+=(-input.difference*input.normal.y)*massDiff1;



        objects[o2].X-=(-input.difference*input.normal.x)*massDiff2;

        objects[o2].Y-=(-input.difference*input.normal.y)*massDiff2;


        if(objects[o1].objectType==1&&(-input.difference*input.normal.y)<0){
            canJump=1;
            jumpCountDown=1;
        }else if(objects[o2].objectType==1&&-(-input.difference*input.normal.y)<0){
            canJump=1;
            jumpCountDown=1;
        }
        float direction =(-input.difference)/fabs(input.difference);




        float Vel;
        float velX1;
        float velY1;
        float velX2;
        float velY2;
        float rot1;
        float rot2;


        //really need to redo this
        //violates laws of thermodynamics
        if(massDiff1!=0&&massDiff2!=0){
            Vel= fabs(((objects[o1].velX)*objects[o1].mass+fabs(objects[o2].velX)*objects[o2].mass)*input.normal.x+((objects[o1].velY)*objects[o1].mass+(objects[o2].velY)*objects[o2].mass)*input.normal.y);
            velX1=Vel*input.normal.x;
            velY1=Vel*input.normal.y;

            velX2=Vel*input.normal.x;
            velY2=Vel*input.normal.y;

            rot1=VelRotCalc(o1,o2,input);
            rot2=VelRotCalc(o2,o1,input);
        }else if(massDiff2!=0){
            Vel= fabs(((objects[o2].velX)*objects[o2].mass)*input.normal.x+((objects[o2].velY)*objects[o2].mass)*input.normal.y);
            velX2=Vel*input.normal.x;
            velY2=Vel*input.normal.y;

            rot2=VelRotCalc(o2,o1,input);
        }else if(massDiff1!=0){
            Vel= fabs(((objects[o1].velX)*objects[o1].mass)*input.normal.x+((objects[o1].velY)*objects[o1].mass)*input.normal.y);
            velX1=Vel*input.normal.x;
            velY1=Vel*input.normal.y;

            rot1=VelRotCalc(o1,o2,input);
        }

        if(massDiff1!=0){
            if(debug==true){
                std::cout<<"rotation: "<<o1<<" "<<rot1<<"\n";

            }
            //Vel-=abs(rot1*Vel/objects[o2].mass*500);
            objects[o1].velRot+=rot1*coefficientOfRestitution*Vel/objects[o1].mass*massDiff1;
            objects[o1].velX+=direction*velX1/objects[o1].mass*coefficientOfRestitution/baseUnit*massDiff1;

            objects[o1].velY+=direction*velY1/objects[o1].mass*coefficientOfRestitution/baseUnit*massDiff1;

        }
        if(massDiff2!=0){
            if(debug==true){

                std::cout<<"rotation: "<<o2<<" "<<rot2<<"\n";
            }
            //Vel-=abs(rot2*Vel/objects[o2].mass*500);
            objects[o2].velRot+=rot2*coefficientOfRestitution*Vel/objects[o2].mass*massDiff2;
            objects[o2].velX+=-direction*velX2/objects[o2].mass*coefficientOfRestitution/baseUnit*massDiff2;

            objects[o2].velY+=-direction*velY2/objects[o2].mass*coefficientOfRestitution/baseUnit*massDiff2;
        }

    }
    friction(o1,o2,input.normal,timediff);
    friction(o2,o1,input.normal,timediff);


}
//don't input an obect besides ones for this calculated SAT input
//calculated projected offset of shape for rotation
float game::VelRotCalc(int o1,int o2,SATout input){
    //choses point
    float Xaverage = (objects[o1].X+objects[o2].X)/2;
    float Yaverage = (objects[o1].Y+objects[o2].Y)/2;
    returnXY pointAverage = {Xaverage,Yaverage};
    returnXY inPoint;
    if(distance(input.point1,pointAverage)<distance(input.point2,pointAverage)){
        inPoint = input.point1;
    }else{
        inPoint = input.point2;

    }
    if(debug==true){
        objects[o1].pointProjected2=input.point2;
        objects[o1].pointProjected=input.point1;
    }
    returnXY normal = invertNormal(input.normal);

    float projectedOffset = projectPointOntoNormal(inPoint,normal);

    returnXY center;
    center.x=objects[o1].X;
    center.y=objects[o1].Y;

    float projectedCenter = projectPointOntoNormal(center,normal);

    float polarityOffset = projectPointOntoNormal(inPoint,input.normal);
    float polarityCenter = projectPointOntoNormal(center,input.normal);

    float output = projectedCenter-projectedOffset;
    //
    if(polarityCenter-polarityOffset>0){
        output*=-1;
    }


    //if(objects[o2].mass==std::numeric_limits<float>::infinity()&&objects[o1].mass!=std::numeric_limits<float>::infinity()){

    //output*=-1;
    //std::cout<<"polarityOffset: "<<polarityOffset<<"\npolarityCenter: "<<polarityCenter<<"\n";
    /*if(objects[o1].sides%2==0&&objects[o1].sides!=0){
     *                    output*=-1;
}*/
    //}
    if(output<0){
        //std::cout<<pol<<"\n";
        output=-1*sqrt(fabs(output));
    }else{
        output=sqrt(fabs(output));
    }

    return output;


}



void game::friction(int o1,int o2,returnXY normal,float timediff){
    returnXY inverted = invertNormal(normal);
    returnXY vel = {objects[o1].velX,objects[o1].velY};
    returnXY velNormal = vel;

    float len = sqrt(square(velNormal.x)+square(velNormal.y));
    if(len!=0&&objects[o1].objectType!=-1&&objects[o2].objectType!=-1){




        velNormal.x/=len;
        velNormal.y/=len;

        float frictDist = projectPointOntoNormal(vel,inverted);
        frictDist = frictDist*(objects[o1].coefficentOfFriction+objects[o2].coefficentOfFriction)/2/timediff;
        if(frictDist!=std::numeric_limits<float>::infinity()&&frictDist!=-std::numeric_limits<float>::infinity()){
            objects[o1].velX-=frictDist*inverted.x;
            objects[o1].velY-=frictDist*inverted.y;

            objects[o1].velRot*=objects[o1].coefficentOfFriction;

        }
    }


}
