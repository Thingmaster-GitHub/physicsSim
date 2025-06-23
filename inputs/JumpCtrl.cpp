#include "../headers/headers.cpp"
//jump countdown
//coyote time + jitter fix for jumping
void game::jumpDown(float timediff){
    if(jumpCountDown>=0){
        jumpCountDown-=1/timediff/20;
    }
    if(jumpCountDown<0){
        canJump=0;
    }
}
