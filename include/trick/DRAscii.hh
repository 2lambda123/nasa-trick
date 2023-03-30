/*
PURPOSE:
    (Data Record Ascii class.)
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef DRASCII_HH
#define DRASCII_HH

#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>

#include <string>

#include "trick/DataRecordGroup.hh"

#ifdef SWIG
%feature("shadow") Trick::DRAscii::DRAscii(std::string in_name) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick {

    /**

    The DRAscii recording format is a comma separated value file named log_<group_name>.csv.  The contents
    of this file type are readable by the Trick Data Products packages, ascii editors, and Microsoft Excel.
    The format of the file follows.  Users are able to change the comma delimiter to another string.  Changing
    the delimiter will change the file extension from ".csv" to ".txt".

    @verbatim
    name_1 {units_1},name_2 {units_2},etc...
    value1,value2,etc...
    value1,value2,etc...
    value1,value2,etc...
    value1,value2,etc...
    @endverbatim
    */

    class DRAscii : public Trick::DataRecordGroup {

        public:

            /** 'C' printf format for floats.\n */
            std::string ascii_float_format;  /**< trick_units(--) */

            /** 'C' printf format for doubles.\n */
            std::string ascii_double_format; /**< trick_units(--) */

            /** Delimiter for separating ascii format fields.\n */
            std::string delimiter;           /**< trick_units(--) */

            #ifndef SWIG
            /**
             @brief DRAscii default constructor.
             */
            DRAscii() {}
            #endif
            ~DRAscii() {}

            /**
             @brief @userdesc Create a new Ascii data recording group.
             @par Python Usage:
             @code <my_drg> = trick.DRAscii("<in_name>") @endcode
             @copydoc Trick::DataRecordGroup::DataRecordGroup(string in_name)
             */
            DRAscii( std::string in_name) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_header
             */
            virtual int format_specific_header(std::fstream & outstream) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_init
             */
            virtual int format_specific_init() ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_write_data
             */
            virtual int format_specific_write_data(unsigned int writer_offset) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_shutdown
             */
            virtual int format_specific_shutdown() ;

            /**
             @brief @userdesc Command to set the printf format for the group's float variable values in the
             log file (default is "%20.8g").
             @par Python Usage:
             @code <dr_group>.set_ascii_float_format("<in_ascii_float_format>") @endcode
             @param in_ascii_float_format - printf format string
             @return always 0
            */
            int set_ascii_float_format(std::string in_ascii_float_format) ;

            /**
             @brief @userdesc Command to set the printf format for the group's double variable values in the
             log file (default is "%20.16g").  If single_prec_only is set, then default is "%20.8g".
             @par Python Usage:
             @code <dr_group>.set_ascii_double_format("<in_ascii_double_format>") @endcode
             @param in_ascii_double_format - printf format string
             @return always 0
            */
            int set_ascii_double_format(std::string in_ascii_double_format) ;

            /**
             @brief @userdesc Command to set the group's delimiter string printed between values in the log
             file (default is ",").  When the delimiter is a comma (","), the log file name
             extension will be .csv, otherwise the log file extension will be .txt.
             @par Python Usage:
             @code <dr_group>.set_delimiter("<in_delimiter>") @endcode
             @param in_delimiter - the delimiter string
             @return always 0
            */
            int set_delimiter(std::string in_delimiter) ;

            /**
             @brief @userdesc Command to print double variable values as single precision (float) in the log file
             to save space (default is false).
             @par Python Usage:
             @code <dr_group>.set_single_prec_only(<in_single_prec_only>) @endcode
             @param in_single_prec_only - boolean true indicates print doubles as single precision
             @return always 0
            */
            virtual int set_single_prec_only(bool in_single_prec_only) ;

            /** @section Friending DRAsciiTest and Related Methods */
            friend class DRAsciiTest;
            FRIEND_TEST(DRAsciiTest, CharToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedCharToASCII);
            FRIEND_TEST(DRAsciiTest, BooleanToASCII);
            FRIEND_TEST(DRAsciiTest, StringToASCII);
            FRIEND_TEST(DRAsciiTest, ShortToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedShortToASCII);
            FRIEND_TEST(DRAsciiTest, EnumeratedToASCII);
            FRIEND_TEST(DRAsciiTest, IntegerToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedIntegerToASCII);
            FRIEND_TEST(DRAsciiTest, LongToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedLongToASCII);
            FRIEND_TEST(DRAsciiTest, FloatToASCII);
            FRIEND_TEST(DRAsciiTest, DoubleToASCII);
            FRIEND_TEST(DRAsciiTest, BitfieldToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedBitfieldToASCII);
            FRIEND_TEST(DRAsciiTest, LongLongToASCII);
            FRIEND_TEST(DRAsciiTest, UnsignedLongLongToASCII);
            FRIEND_TEST(DRAsciiTest, IntArrayToASCII);
            FRIEND_TEST(DRAsciiTest, DoubleArrayToASCII);
            FRIEND_TEST(DRAsciiTest, StringArrayToASCII);
            FRIEND_TEST(DRAsciiTest, ArrayToASCII_IndexOutOfBounds);
                
        private:

            /**
             * @brief Copies the data referenced in a DataRecordBuffer to an ASCII buffer.
             * @param DI a DataRecordBuffer to be converted into ASCII
             * @param item_num ???
             * @param buf a C style string used to output the converted data
             * @return Always returns 0 barring any errors
             * 
             * @see include/trick/DataRecordGroup.hh::DataRecordBuffer()
             * @see include/trick/attributes.h::ATTRIBUTES
             */
            int copy_data_ascii_item( Trick::DataRecordBuffer * DI, int item_num, char *buf ) ;

            /** Output stream for the log file */
            std::fstream out_stream ; /**< trick_io(**)  */

    } ;

} ;

#endif
