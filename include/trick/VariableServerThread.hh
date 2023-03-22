/*
    PURPOSE:
        (VariableServerThread)
*/

#ifndef VARIABLESERVERTHREAD_HH
#define VARIABLESERVERTHREAD_HH

#include <string>
#include <iostream>
#include <pthread.h>
#include "trick/SysThread.hh"
#include "trick/VariableServerSession.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/variable_server_message_types.h"

#include "trick/ClientConnection.hh"
#include "trick/ClientListener.hh"

namespace Trick {

    class VariableServer ;

    /** Flag to indicate the connection has been made */
    enum ConnectionStatus { CONNECTION_PENDING, CONNECTION_SUCCESS, CONNECTION_FAIL };


/**
  The purpose of this class is to run a VariableServerSession.
    - Manages creation and cleanup of the network connection
    - Manages thread startup and shutdown
    - Invokes parts of the VariableServerSession that run asynchronously (reading from client, copying and writing to client if in applicable mode)

  @author Alex Lin
  @author Jackie Deans (2023)
 */
    class VariableServerThread : public Trick::SysThread {

        public:
            friend std::ostream& operator<< (std::ostream& s, Trick::VariableServerThread& vst);

            /**
             @brief Constructor.
            */
            VariableServerThread() ;
            VariableServerThread(VariableServerSession * session) ;
            
            virtual ~VariableServerThread() ;
            /**
             @brief static routine called from S_define to set the VariableServer pointer for all threads.
             @param in_vs - the master variable server object
            */
            static void set_vs_ptr(Trick::VariableServer * in_vs) ;

            void set_client_tag(std::string tag);

            /**
             @brief Set the connection pointer for this thread
            */
            void set_connection(ClientConnection * in_connection);

            /**
             @brief Block until thread has accepted connection
            */
            ConnectionStatus wait_for_accept() ;

            /**
             @brief The main loop of the variable server thread that reads and processes client commands.
             @return always 0
            */
            virtual void * thread_body() ;

            VariableServer * get_vs() ;

            void preload_checkpoint() ;

            void restart() ;

            void cleanup();

        protected:
            /** The Master variable server object. */
            static VariableServer * _vs ;

            /** Manages the variable list  */
            VariableServerSession * _session;       /**<  trick_io(**) */

            /** Connection to the client */
            ClientConnection * _connection;        /**<  trick_io(**) */

            /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
            int _debug ;                      /**<  trick_io(**) */

            ConnectionStatus _connection_status ;       /**<  trick_io(**) */
            pthread_mutex_t _connection_status_mutex;     /**<  trick_io(**) */
            pthread_cond_t _connection_status_cv;         /**<  trick_io(**) */

            /** The mutex pauses all processing during checkpoint restart */
            pthread_mutex_t _restart_pause ;     /**<  trick_io(**) */

            // bool pause_cmd;
            bool _saved_pause_cmd;
    } ;

    std::ostream& operator<< (std::ostream& s, VariableServerThread& vst);

}

#endif

