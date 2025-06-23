#include "../headers/headers.cpp"
//trigger logic
void game::trigger(int o1,int o2){

    if(objects[o1].trigger.event=="destroy"){
        if(objects[o1].trigger.typeReq==objects[o2].objectType && (objects[o1].trigger.isIDReq==false || objects[o1].trigger.IDReq==objects[o2].trigger.id) ){
            for(int i=0;i<objects.size();i++){
                if(objects[o1].trigger.id==objects[i].trigger.id){
                    objects[i].objectType=-4;
                    objects[i].mass=0;
                    objects[i].gravity=false;
                    objects[i].airRes=false;
                    objects[i].solid=false;
                }
            }
        }
    }

}
