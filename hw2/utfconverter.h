
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdbool.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/stat.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <stdint.h>
    #include <getopt.h>

#ifdef CSE320
    #define DEBUG_HOST(name) do {  fprintf(stderr, "CSE320: Host: %s\n", name);}while(0)                            
    #define DEBUG_INPUT(name, inode, dev,size) do {  fprintf(stderr,"CSE320: Input: %s, %lu, %lu, %lu byte(s)\n",name, inode, dev, size);}while(0)                                          
    #define DEBUG_OUTPUT(msg) do{ fprintf(stderr,"CSE320: Output: %s\n", msg);}while(0)
    #define DEBUG_ENCODING_INPUT(msg) do{ fprintf(stderr,"CSE320: Input Encoding: %s\n", msg);}while(0)
    #define DEBUG_ENCODING_OUTPUT(msg) do{ fprintf(stderr,"CSE320: Output Encoding: %s\n", msg);}while(0)
    // #define debug_encoding(msg) printf("CSE320: Output encoding: %s", msg)
#else
    #define DEBUG_HOST(name) 
    #define DEBUG_INPUT(name, inode, dev,size)  
    #define DEBUG_OUTPUT(msg)  
    #define DEBUG_ENCODING_INPUT(msg)
    #define DEBUG_ENCODING_OUTPUT(msg)

#endif

    /* Constants for validate_args return values. */
    #define VALID_ARGS 0
    #define SAME_FILE  1
    #define FILE_DNE   2
    #define FAILED     3

    #define UTF8_4_BYTE 0xF0
    #define UTF8_3_BYTE 0xE0
    #define UTF8_2_BYTE 0xC0
    #define UTF8_CONT   0x80

    /* # of bytes a UTF-16 codepoint takes up */
    #define CODE_UNIT_SIZE 2

    #define SURROGATE_PAIR 0x10000

    #define SAFE_PARAM 0x0FA47E10
    
    /*Defines to know encoding*/
    #define UTF_8_BOM 0xBFBBEF
    #define UTF_16LE_BOM 0xFEFF
    #define UTF_16BE_BOM 0xFFFE

           struct option long_options[] =
        {
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"verbose",     no_argument,     0, 'v'},
          {"encoding",  required_argument, 0, 'e'},
          {"help",  no_argument,     0, 'h'},
          {0, 0}
        };

    typedef struct{
        unsigned char ascii;
        unsigned int codepoint;
        unsigned int input;
        unsigned int number_of_bytes;

    }table;
    /**
     * Checks to make sure the input arguments meet the following constraints.
     * 1. input_path is a path to an existing file.
     * 2. output_path is not the same path as the input path.
     * 3. output_format is a correct format as accepted by the program.
     * @param input_path Path to the input file being converted.
     * @param output_path Path to where the output file should be created.
     * @return Returns 0 if there was no errors, 1 if they are the same file, 2
     *         if the input file doesn't exist, 3 if something went wrong.
     */
    int validate_args(const char *input_path, const char *output_path);

    /**
     * Converts the input file UTF-8 file to UTF-16LE.
     * @param input_fd The input files file descriptor.
     * @param output_fd The output files file descriptor.
     * @return Returns true if the conversion was a success else false.
     */
    bool convert(const int input_fd, const int output_fd,unsigned int verbose, char *evalue);

    /**
     * Writes bytes to output_fd and reports the success of the operation.
     * @param value Value to be written to file.
     * @param size Size of the value in bytes to write.
     * @return Returns true if the write was a success, else false.
     */
    bool safe_write(int output_fd, void *value, size_t size, char *type_chosen);


    int reverseInt (int val, char *type);


    void printTable(int ascii, unsigned int number_of_bytes, unsigned int codepoint, 
    unsigned int input, unsigned int w1, unsigned int w2,int type, int verbose);


    void print_header(unsigned int verbose);

    void write_bom(int output_fd,char *type);

    bool convert_to_utf8(const int input_fd,const int output_fd,unsigned int verbose, char *evalue);
   
    unsigned int get_hex(unsigned int codepoint,unsigned int *bytes, unsigned int *bytes_to_write);

    bool is_big_endian();

    short flip_short(short val);

    int flip(int val);


    bool convert_to_UTF16LEBE_from_UTF16BELE(const int input_fd,const int output_fd,unsigned int verbose, char *evalue);
    
    bool convert_to_same(const int input_fd,const int output_fd,unsigned int verbose, char *evalue);
    /**
     * Print out the program usage string
     */
    #define USAGE(name) do {                                                                                                \
        fprintf(stderr,                                                                                                     \
            "\nUsage: %s [-h] [-v| -vv| -vvv] -e OUTPUT_ENCODING\n"                                                         \
            "INPUT_FILE OUTPUT_FILE \n"                                                                                     \
            "\n"                                                                                                            \
            "Command line utility for converting files to and from UTF-8, UTF-\n"                                           \
            "16LE, or UTF-16BE\n"                                                                                           \
            "\n"                                                                                                            \
            "Option arguments:\n\n"                                                                                         \
            "-h                             Displays this usage menu.\n\n"                                                  \
            "-v                             Enables verbose output.\n"                                                      \
            "                               this argument can be used\n"                                                    \
            "                               up to three times\n"                                                            \
            "                               for a noticeable effect.\n\n"                                                   \
            "-e OUTPUT_ENCODING             Format to encode the output file.\n"                                            \
            "                               Accepted values:\n"                                                             \
            "                                          UTF-8\n"                                                             \
            "                                          UTF-16LE\n"                                                          \
            "                                          UTF-16BE\n"                                                          \
            "                               If this flag is not provided\n"                                                 \
            "                               or an invalid value is\n"                                                       \
            "                               given the program should exit with\n"                                           \
            "                               the EXIT_FAILURE return code.\n"                                                \
            "\nPositional arguments:\n\n"                                                                                   \
            "INPUT_FILE                     File to convert. Must contain a\n"                                              \
            "                               valid BOM. If it does not contain a\n"                                          \
            "                               valid BOM the program should exit\n"                                            \
            "                               with the EXIT_FAILURE return code.\n"                                           \
            "\n"                                                                                                            \
            "OUTPUT_FILE                    Output file to create. If the file\n"                                           \
            "                               already exists and its not the input\n"                                         \
            "                               file, it should be overwritten. If\n"                                           \
            "                               the OUTPUT_FILE is the same as the\n"                                           \
            "                               INPUT_FILE the program should exit\n"                                           \
            "                               with the EXIT_FAILURE return code.\n"                                           \
            ,(name)                                                                                                         \
        );                                                                                                                  \
    } while(0)
