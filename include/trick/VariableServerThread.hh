/*
    PURPOSE:
        (VariableServerThread)
*/

#ifndef VARIABLESERVERTHREAD_HH
#define VARIABLESERVERTHREAD_HH

#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>
#include "trick/tc.h"
#include "trick/TCConnection.hh"
#include "trick/ThreadBase.hh"
#include "trick/VariableServerSession.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/variable_server_message_types.h"


namespace Trick {

    class VariableServer ;

/**
  This class provides variable server command processing on a separate thread for each client.
  @author Alex Lin
 */
    class VariableServerThread : public Trick::ThreadBase {

        public:
            // Are UDP or MCAST ever actually used?

            // Is this ever actually used?
            friend std::ostream& operator<< (std::ostream& s, Trick::VariableServerThread& vst);

            /**
             @brief Constructor.
             @param listen_dev - the TCDevice set up in listen()
            */
            VariableServerThread(TCDevice * in_listen_dev ) ;

            virtual ~VariableServerThread() ;
            /**
             @brief static routine called from S_define to set the VariableServer pointer for all threads.
             @param in_vs - the master variable server object
            */
            static void set_vs_ptr(Trick::VariableServer * in_vs) ;

            void set_client_tag(std::string tag);

            /**
             @brief Block until thread has accepted connection
            */
            void wait_for_accept() ;

            /**
             @brief The main loop of the variable server thread that reads and processes client commands.
             @return always 0
            */
            virtual void * thread_body() ;

            VariableServer * get_vs() ;

            void preload_checkpoint() ;

            // Is this restarting thread itself or session?
            void restart() ;

        protected:

            /**
             @brief Called by send_sie commands to transmit files through the socket.
            */
            int transmit_file(std::string file_name);

            /** The Master variable server object. */
            static VariableServer * vs ;

            /** this is where a lot of this should happen now */
            VariableServerSession * session;

            /** The listen device from the variable server\n */
            // TODO: These should become ClientConnections or TCConnections
            TCDevice * listen_dev;          /**<  trick_io(**) */
            // TCDevice connection;          /**<  trick_io(**) */

            
            TCConnection connection;

            /** The type of connection we have.\n */
            // ConnectionType conn_type ;      /**<  trick_io(**) */

            /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
            int debug ;                      /**<  trick_io(**) */

            /** Toggle to enable/disable this variable server thread.\n */
            bool enabled ;                   /**<  trick_io(**) */

            pthread_mutex_t connection_accepted_mutex;
            pthread_cond_t connection_accepted_cv;

            /** The mutex pauses all processing during checkpoint restart */
            pthread_mutex_t restart_pause ;     /**<  trick_io(**) */

            bool pause_cmd;
            bool saved_pause_cmd;

            /** The simulation time converted to seconds\n */
            double time ;                    /**<  trick_units(s) */

            /** Toggle to tell variable server to send data multicast or point to point.\n */
            bool multicast ;                 /**<  trick_io(**) */

            /** Flag to indicate the connection has been made\n */
            bool connection_accepted ;       /**<  trick_io(**) */

    } ;
}

#endif

