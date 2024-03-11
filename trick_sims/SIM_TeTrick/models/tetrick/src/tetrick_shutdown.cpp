/************************************************************************
PURPOSE: (Shutdown the sim)
*************************************************************************/
#include <stdio.h>
#include "../include/waterclock.h"
#include "trick/exec_proto.h"

int tetrick_shutdown( TETRICK* WC) {
    double t = exec_get_sim_time();
    printf( "========================================\n");
    printf( "      Water Clock Shutdown     \n");
    printf( "========================================\n");
    return 0 ;
}
