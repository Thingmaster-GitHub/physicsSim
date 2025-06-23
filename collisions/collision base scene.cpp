#include "../headers/headers.cpp"

//checks bounding box collisions and runs SAT if intersects
void game::baseCollision(float timediff){
    float XMax;
    float XMin;
    float YMax;
    float YMin;

    float XMaxCh;
    float XMinCh;
    float YMaxCh;
    float YMinCh;
    SATout output;
    for(int i = 0;i<objects.size();i++){
        XMax = getMaxX(i);
        XMin = getMinX(i);
        YMax = getMaxY(i);
        YMin = getMinY(i);
        for(int iP=i+1;iP<objects.size();iP++){

            if(i!=iP){
                XMaxCh = getMaxX(iP);
                XMinCh = getMinX(iP);
                YMaxCh = getMaxY(iP);
                YMinCh = getMinY(iP);
                //std::cout<<"top Y 1: "<<YMin<<"\ntop Y 2: "<<YMinCh<<"\nbottom Y 1: "<<YMax<<"\nbottom Y 2: "<<YMaxCh<<"\n\n";
                if(!(XMin>XMaxCh||XMax<XMinCh||YMin>YMaxCh||YMax<YMinCh)){
                    if(objects[i].solid&&objects[iP].solid){
                        if(debug==true){
                            //std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(bounding box)\n";
                        }

                        objects[i].collidedbox=true;
                        objects[iP].collidedbox=true;
                        output = SAT(i,iP);//unfinished
                        if(output.difference<0-1){
                            objects[i].collidedSAT=true;
                            objects[iP].collidedSAT=true;
                            collisionResponse(i,iP,output,timediff);
                            if(debug==true){
                                std::cout<<i<<" ("<<objects[i].X<<", "<<objects[i].Y<<") intersects with "<<iP<<" ("<<objects[iP].X<<", "<<objects[iP].Y<<") !(SAT)\n";
                            }
                        }

                    }else if(objects[i].objectType==-3){
                        trigger(i,iP);
                    }if(objects[iP].objectType==-3){
                        trigger(iP,i);
                    }
                }
            }

        }

    }
}
