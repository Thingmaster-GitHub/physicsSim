#include "../headers/headers.cpp"
//transforms objects by velocity, gravity, and air resistance.
void game::transform(int i,float timediff){
    objects[i].X+=objects[i].velX/timediff*baseUnit/500;
    objects[i].Y+=objects[i].velY/timediff*baseUnit/500;
    objects[i].rotation+=objects[i].velRot/timediff/500;
    if(wrap==true){
        if(objects[i].X>W){
            objects[i].X-=W;
        }else if(objects[i].X<0){
            objects[i].X+=W;
        }
        if(objects[i].Y>H){

            objects[i].Y-=H;
            //objects[i].Y=H;
        }else if(objects[i].Y<0){
            objects[i].Y+=H;
        }
    }

    //objects[i].velX = don't know what to do for these yet
    //std::cout<<"timediff: "<<timediff<<"\noutput: "<<10/timediff<<"\n";
    if(objects[i].gravity==true){
        //gravity
        objects[i].velY+=(25/timediff);

    }
    if(objects[i].airRes==true){
        objects[i].velY-=(objects[i].velY/timediff/500/objects[i].mass);
        objects[i].velX-=(objects[i].velX/timediff/500/objects[i].mass);
        objects[i].velRot-=(objects[i].velRot/timediff/100/objects[i].mass);
    }

    //std::cout<<objects[i].velY<<"\n";

}
