/*
    PURPOSE:
        (Custom log file writable by message_publish.)
    ASSUMPTIONS AND LIMITATIONS:
        (())
    PROGRAMMERS:
        (((Jackie Deans) (LinCom) (4/23) (--) (CACI))
*/

#ifndef MESSAGE_CUSTOM_FILE_HH
#define MESSAGE_CUSTOM_FILE_HH

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "trick/MessageFile.hh"

namespace Trick {

    /**
     * MessageCustomFile writes all messages its level to the given file
     */
    class MessageCustomFile : public MessageFile {

        public:

            /**
             @brief The constructor.
             */
            MessageCustomFile() ;
            // static int openCustomMessageFile(std::string filename, std::string subscriber_name = "");
            // static int shutdown_all();

            /**
             @brief Output message to the file.
             */
            virtual void update( unsigned int level , std::string header , std::string message );

            /**
             * @brief Get the level
             * 
             */
            virtual int get_level();

            virtual void set_name(std::string name);
            virtual void set_file_name(std::string file_name);


            /**
             @brief Register this object with the memory manager.
             */
            // virtual void register_with_mm();
            // virtual void unregister_with_mm();

            int this_level;

        protected:

            static int level_counter;
            // static std::vector<MessageCustomFile *>& all_instances();


    } ;

}

#endif

