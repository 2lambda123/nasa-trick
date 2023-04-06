
#include <iostream>
#include <sys/time.h>

#include "trick/VariableServer.hh"

int Trick::VariableServer::copy_data_freeze() {

    pthread_mutex_lock(&map_mutex) ;
    for ( auto it = var_server_sessions.begin() ; it != var_server_sessions.end() ; it++ ) {
        (*it).second->copy_data_freeze() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    return 0 ;
}
