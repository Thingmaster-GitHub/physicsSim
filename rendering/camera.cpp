#include "../headers/headers.cpp"
//controls camera
returnXY game::Camera(float PX,float PY,float timediff){
    for(int i = 0; i<objects.size();i++){
        if(centerCamera==true&&objects[i].objectType==1){
            PX=objects[i].X;
            PY=objects[i].Y;
        }
    }
    for(int i = 0; i<objects.size();i++){
        if(centerCamera==true&&objects[i].objectType==-2){
            objects[i].X=(((objects[i].X)*(timediff*10-1)+(objects[mouseObject].X+PX)/2)/(timediff*10));
            objects[i].Y=(((objects[i].Y)*(timediff*1-1)+(objects[mouseObject].Y+PY)/2)/(timediff*1));
            camOffsetX=-objects[i].X+W/2;
            camOffsetY=-objects[i].Y+H/2;
            if(objects[i].Y==std::numeric_limits<float>::infinity()||objects[i].X==std::numeric_limits<float>::infinity()){
                objects[i].X=0;
                objects[i].Y=0;
                camOffsetX=0;
                camOffsetY=0;
            }
        }
    }
    returnXY output {PX,PY};
    return output;
}
