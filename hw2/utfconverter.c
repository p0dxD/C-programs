#include "utfconverter.h"

char *type_chosen;


    
        
int main(int argc,char *argv[]){
/*

    get host information

*/
char hostname[128];

    gethostname(hostname, sizeof hostname);
    DEBUG_HOST(hostname);


    int opt, return_code = EXIT_FAILURE, verbose = 0;
    char *evalue = NULL;
    char *input_path = NULL;
    char *output_path = NULL;
    type_chosen = NULL;
    int option_index = 0;

    /* open output channel */
    FILE* standardout = fopen("stdout", "w");
    /* Parse short options */
    while((opt = getopt_long (argc, argv, "hve:",
                       long_options, &option_index)) != -1) {
        switch(opt) {
            case 'h':
                /* The help menu was selected */
                USAGE(argv[0]);
                fclose(standardout);
                exit(EXIT_SUCCESS);
                break;
            case 'v':
                verbose++;
            break;
            case 'e':
                evalue = optarg;
                if((strcmp(evalue,"UTF-8")==0)|
                    (strcmp(evalue,"UTF-16LE")==0)|
                    (strcmp(evalue,"UTF-16BE")==0)){
                type_chosen = evalue;

                }else{
                fclose(standardout);
                USAGE(argv[0]);
                exit(EXIT_FAILURE);
                }
            break;
            case '?':
                /* Let this case fall down to default;
                 * handled during bad option.*//*
                 */
            default:
                /* A bad option was provided. */
                USAGE(argv[0]);
                fclose(standardout);
                exit(EXIT_FAILURE);
                break;
        }
    }


    /* Get position arguments */
    if(optind < argc && (argc - optind) == 2) {
        input_path = argv[optind++];
        output_path = argv[optind++];
    } else {
        if((argc - optind) <= 0) {
            fprintf(standardout, "Missing INPUT_FILE and OUTPUT_FILE.\n");
        } else if((argc - optind) == 1) {
            fprintf(standardout, "Missing OUTPUT_FILE.\n");
        } else {
            fprintf(standardout, "Too many arguments provided.\n");
        }
        USAGE(argv[0]);
        fclose(standardout);
        exit(EXIT_FAILURE);
    }
    /* Make sure all the*/// arguments were provided */
    if(input_path != NULL || output_path != NULL) {
        int input_fd = -1, output_fd = -1;
        bool success = false;
        switch(validate_args(input_path, output_path)) {
                case VALID_ARGS:
                    /* Attempt to open the input file */
                    if((input_fd = open(input_path, O_RDONLY)) < 0) {
                        fprintf(standardout, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }
                    /* Delete the output file if it exists; Don't care about return code. */
                    unlink(output_path);
                    /* Attempt to create the file */
                    if((output_fd = open(output_path, O_CREAT | O_WRONLY,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
                        /* Tell the user that the file failed to be created */
                        fprintf(standardout, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }

                    /* Start the conversion */
                    success = convert(input_fd, output_fd, verbose, evalue);
conversion_done:
                    if(success) {
                        /* We got here so it must of worked right? */
                        fprintf(stderr, "The file %s was successfully created.\n", output_path);
                        return_code = EXIT_SUCCESS;
                    } else {
                        /* Conversion failed; clean up */
                        if(output_fd < 0 && input_fd >= 0) {
                            close(input_fd);
                        }
                        if(output_fd >= 0) {
                            close(output_fd);
                            unlink(output_path);
                        }
                        /* Just being pedantic... */
                        return_code = EXIT_FAILURE;
                        USAGE(argv[0]);
                
                    }
                    break;
                case SAME_FILE:
                    fprintf(standardout, "The output file %s was not created. Same as input file.\n", output_path);
                    break;
                case FILE_DNE:
                    fprintf(standardout, "The input file %s does not exist.\n", input_path);
                    break;
                default:
                    fprintf(standardout, "An unknown error occurred\n");
                    break;
        }
    } else {
        /* Alert the user*/// what was not set before quitting. */
        if((input_path = NULL) == NULL) {
            fprintf(standardout, "INPUT_FILE was not set.\n");
        }
        if((output_path = NULL) == NULL) {
            fprintf(standardout, "OUTPUT_FILE was not set.\n");
        }
        // Print out the program usage
        USAGE(argv[0]);
    }
    fclose(standardout);
    exit(return_code);
}

int validate_args(const char *input_path,const char *output_path){
    int return_code = FAILED;
    /* number of arguments */
   /* number of arguments */
    int vargs = 2;
    /* create reference */
    void* pvargs = &vargs;
    /* Make sure both strings are not NULL */
    if(input_path != NULL && output_path != NULL) {
        /* Check to see if the the input and output are two different files. */
        if(strcmp(input_path, output_path) != 0) {
            /* Check to see if the input file exists */
            struct stat sb;
            /* zero out the memory of one sb plus another */
            memset(&sb, 0, sizeof(sb) + 1);
              if(stat(input_path,&sb) < 0){    
                            exit(EXIT_FAILURE);
                        }
        if(lstat(input_path, &sb) < 0) {  /* if error occured */
                perror("calling stat()");
                exit(1);  /* end progam here */
            }


                DEBUG_INPUT(input_path, sb.st_ino,sb.st_dev, sb.st_size);
                DEBUG_OUTPUT(output_path);
            /* increment to second argument */
            pvargs++;
            /* now check to see if the file exists */
            if(stat(input_path, &sb) == -1) {
                /* something went wrong */
                if(errno == ENOENT) {
                    /* File does not exist. */
                    return_code = FILE_DNE;
                } else {
                    /* No idea what the error is. */
                    perror("NULL");
                }

            } else {
                return_code = VALID_ARGS;
            }
        }
    }
    /* Be good and free memory */
    /*free(pvargs);*/
    return return_code;
}

bool convert(const int input_fd,const int output_fd,unsigned int verbose, char *evalue){

    bool success = false;
        /*for table*/
    table get;
    if(input_fd >= 0 && output_fd >= 0) {
        /* UTF-8 encoded text can be @ most 4-bytes */
        unsigned char bytes['4'-'0'];
        unsigned char read_value;
        unsigned int count = 0;
        int safe_param = SAFE_PARAM;//, i = 0;// DO NOT DELETE, PROGRAM WILL BE UNSAFE //
        // unsigned char type[3];
        void* saftey_ptr = &safe_param;
        ssize_t bytes_read;
        bool encode = false;
        char *file_type = NULL;


        unsigned int l = 0;//found = 0;

        /*CHECK BOM*/
        /*printf("%s\n", "Checking bom");*/
        if((bytes_read = read(input_fd, &read_value, 1)) == 1){
                bytes[count] = read_value;
                if(bytes[count] == 0xEF){
                   /* printf("%s\n", "inside UTF");*/
                    /*if we got here then we check the other 2 for UTF-8*/
                    while(l < 2){
                        /* printf("%s\n", "inside while"); */
                    if((bytes_read = read(input_fd, &read_value, 1)) == 1){
                        bytes[++count] = read_value;
                    }else{

                        return success;/*Success false*/
                    }
                    l++;
                }/*end while*/
                    if(((bytes[1])==0xBB)&&((bytes[2])==0xBF)){
                    /*    printf("Yes it is %s\n", "UTF-8"); */
                        file_type = "UTF-8";
                    }else{
                        return success;/*Success false*/
                    }
                }else if((bytes[count] == 0xFF)||(bytes[count] == 0xFE)){
                   /* printf("%s\n", "inside UTF-16LE or utf-16BE");*/

                    if((bytes_read = read(input_fd, &read_value, 1)) == 1){
                        bytes[++count] = read_value;
                        if(bytes[count]==0xFE){
                        /*    printf("%s\n", "UTF-16LE");*/
                            file_type = "UTF-16LE";
                        }else if(bytes[count]==0xFF){
                            /* printf("%s\n", "UTF-16BE");*/
                            file_type = "UTF-16BE";
                        }else{
                            return success;/*Success false*/
                        }
                    }else{
                        return success;/*Success false*/
                    }

                }else{
                    return success;/*Success false*/
                }
        }else{
                return success;/*Success false*/
        }

        /*write the type of boom specified in input*/
        if(evalue != NULL){
            DEBUG_ENCODING_INPUT(evalue);
             write_bom(output_fd, evalue);
        }else{
            return success;/*Success false*/
        }
            DEBUG_ENCODING_INPUT(file_type);

        /*TABLE*/
        print_header(verbose);
        /*if they want utf-8, we do diff conversion*/
        if((strcmp(evalue,"UTF-8")==0)){
            if((strcmp(file_type,"UTF-16LE")==0)||(strcmp(file_type,"UTF-16BE")==0)){/*if file type is LE*/
                success = convert_to_utf8(input_fd, output_fd, verbose, file_type);
                goto conversion_done;
            }else{
                /*same file format*/
                success = convert_to_same(input_fd,output_fd,verbose, evalue);
                goto conversion_done;
                /*copy and display accordingly*/
            }
            return success;/*success is false*/
        }else if((strcmp(evalue,"UTF-16LE")==0)&&(strcmp(file_type,"UTF-16LE")==0)){
                /*same file format*/
                success = convert_to_same(input_fd,output_fd,verbose, evalue);
                goto conversion_done;  
                /*copy and display accordingly*/  
        }else if((strcmp(evalue,"UTF-16BE")==0)&&(strcmp(file_type,"UTF-16BE")==0)){
                /*same file format*/
                success = convert_to_same(input_fd,output_fd,verbose, evalue);
                goto conversion_done;   
                /*copy and display accordingly*/     
        }else if(((strcmp(evalue,"UTF-16LE")==0)&&(strcmp(file_type,"UTF-16BE")==0))){
            success = convert_to_UTF16LEBE_from_UTF16BELE(input_fd,output_fd,verbose, evalue);
            goto conversion_done;

        }else if((strcmp(evalue,"UTF-16BE")==0)&&(strcmp(file_type,"UTF-16LE")==0)){
            success = convert_to_UTF16LEBE_from_UTF16BELE(input_fd,output_fd,verbose, evalue);
            goto conversion_done;
        }
/*MAIN conversion to UTF-16BE/LE*/
    
    count = 0;
/**/
        /* Read in UTF-8 Bytes */
        while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
            /* Mask the most significant bit of the byte */
            unsigned char masked_value = read_value & 0x80;
            if(masked_value == 0x80) {
                if((read_value & UTF8_4_BYTE) == UTF8_4_BYTE ||
                    (read_value & UTF8_3_BYTE) == UTF8_3_BYTE ||
                   (read_value & UTF8_2_BYTE) == UTF8_2_BYTE) {
                    /* Check to see which byte we have encountered*/
                    if(count == 0) {
                        bytes[count++] = read_value;
                    } else {
                        /* Set the file position back 1 byte */
                        if((lseek(input_fd, -1, SEEK_CUR) < 0)) {
                            /*Unsafe action! Increment! */
                           safe_param = *((int*)++saftey_ptr);
                            /* failed to move the file pointer back */
                           perror("NULL");
                           goto conversion_done;
                        }
                        /* Encode the current values into UTF-16LE */
                        encode = true;
                        

                    }
                } else if((read_value & UTF8_CONT) == UTF8_CONT) {
                    /* continuation byte */
                    bytes[count++] = read_value;
                    /*Lets see if we reach end of file, if so lets encode else, seekback*/
                    if((bytes_read = read(input_fd, &read_value, 1)) == 0){
                        encode = true;
                    }else{
                        if((lseek(input_fd, -1, SEEK_CUR) < 0)) {
                            /*Unsafe action! Increment! */
                           safe_param = *((int*)++saftey_ptr);
                            /* failed to move the file pointer back */
                           perror("NULL");
                           goto conversion_done;
                        }                        
                    }

                }
            } else {
                if(count == 0) {
                    /* US-ASCII */
                    bytes[count++] = read_value;
                    encode = true;
                    get.ascii = read_value;/*ADDED*/
                    get.number_of_bytes = 1;

                } else {
                    /* Found an ASCII character but theres other characters
                     * in the buffer already.
                     * Set the file position back 1 byte.
                     */
                     // encode =true;
                    if(lseek(input_fd, -1, SEEK_CUR) < 0) {
                        /*Unsafe action! Increment! */
                        safe_param = *(int*) ++saftey_ptr;
                        /* failed to move the file pointer back */
                        perror("NULL");
                        goto conversion_done;
                    }
                    /* Encode the current values into UTF-16LE */
                    encode = true;
                    get.ascii = 0;
                }
                
            }
            /* If its time to encode do it here */
            if(encode) {
                get.number_of_bytes = count;
                unsigned int i, value = 0;
                bool isAscii = false;
                for(i = 0; i < count; i++) {
                    if(i == 0) {
                        if((bytes[i] & UTF8_4_BYTE) == UTF8_4_BYTE) {
                            value = bytes[i] & 0x7;
                        } else if((bytes[i] & UTF8_3_BYTE) == UTF8_3_BYTE) {
                            value =  bytes[i] & 0xF;
                        } else if((bytes[i] & UTF8_2_BYTE) == UTF8_2_BYTE) {
                            value =  bytes[i] & 0x1F;
                        } else if((bytes[i] & 0x80) == 0) {
                            /* Value is an ASCII character */
                            value = bytes[i];
                            isAscii = true;
                            get.input = value;
                        } else {
                            /* Marker byte is incorrect */
                            goto conversion_done;
                        }
                    } else {
                        if(!isAscii) {;
                            value = (value << 6) | (bytes[i] & 0x3F);
                        } else {
                            /* How is there more// bytes if we have an ascii char? */
                            goto conversion_done;
                        }
                    }
                }
                /*gets each input byte into an int to print out*/
                int j = 0, k = 0;
                get.input = 0;
                for(j = get.number_of_bytes-1, k = 0; j>=0 && k < get.number_of_bytes; j--,k++){
                    get.input |= (bytes[k]<<8*j );                    
                }

                get.codepoint = value;/*The answer at this point is the actual codepoint so we just pass it*/
                get.ascii = ((value <=0x7F)&&(value>=20))? value: '\0';/*if ascii itll get ascii, else it uses zero*/
                /* Handle //the value if its a surrogate pair*/
                if(value >= SURROGATE_PAIR) {
                    int vprime = value - SURROGATE_PAIR;
                    int w1 = ((vprime >> 10) + 0xD800);
                    int w2 = ((vprime & 0x3FF) + 0xDC00);
                    /* write the surrogate pair to file */
                    printTable(get.ascii,get.number_of_bytes ,get.codepoint, get.input, w1,w2,1,verbose);
                    if(!safe_write(output_fd, &w1, CODE_UNIT_SIZE,evalue)) {
                        /* Assembly for some super efficient coding */

                        goto conversion_done;
                    }
                    if(!safe_write(output_fd, &w2, CODE_UNIT_SIZE,evalue)) {
                        /* Assembly for some super efficient coding */

                        goto conversion_done;
                    }
                } else {

                    /* write the code point to file */
                    printTable(get.ascii,get.number_of_bytes ,get.codepoint, get.input, value,0,2,verbose);
                    if(!safe_write(output_fd, &value, CODE_UNIT_SIZE, evalue)) {
                        /* Assembly *///for some super efficient coding */

                        goto conversion_done;
                    }
                }
                /* Done encoding the value to UTF-16LE */
                encode = false;
                count = 0;

            }
        }
        /* If we got here the operation was a success! */
        success = true;
    }
conversion_done:
    return success;
}

bool safe_write(int output_fd,void *value,size_t size, char *type_chosen){

    bool success = true;
    ssize_t bytes_written;
    int val = reverseInt(*((int *)value), type_chosen);
    if((bytes_written = write(output_fd, &val, size)) != size) {
        /* The write operation failed */
        fprintf(stdout, "Write to file failed. Expected %zu bytes but got %zd\n", size, bytes_written);
    }
    return success;
}

void write_bom(int output_fd,char *type){
    int utf;

                if(strcmp(type,"UTF-8")==0){
                    utf = UTF_8_BOM;
                    if(!safe_write(output_fd, &utf, 3,"")){
                        exit(EXIT_FAILURE);
                    }
                }else if(strcmp(type,"UTF-16LE")==0){
                    utf = UTF_16LE_BOM;
                    if(!safe_write(output_fd, &utf, 2,"")){
                        exit(EXIT_FAILURE);
                    }
                   
                }else if (strcmp(type,"UTF-16BE")==0){
                    utf = UTF_16BE_BOM;
                    if(!safe_write(output_fd, &utf, 2,"")){
                        exit(EXIT_FAILURE);
                    }
                }else{
                    perror("Not an option");
                    exit(EXIT_FAILURE);
                }
}
/*in case of ascii w2 is optional*/
/*type 1 being surrogate, 2 other*/
void printTable(int ascii, unsigned int number_of_bytes, unsigned int codepoint, 
    unsigned int input, unsigned int w1, unsigned int w2,int type, int verbose){


    if((verbose == 1)&&(type == 1)){
    fprintf(stderr,"|%s\t|%d\t|U+%04X\t|\n","NONE",number_of_bytes ,codepoint);
    fprintf(stderr,"\n+-------+----------+---------+\n");
    }else if((verbose == 2)&&(type == 1)){
    fprintf(stderr,"|%s\t|%d\t|U+%04X\t|0x%X\t|\n","NONE",number_of_bytes ,codepoint, input);
    fprintf(stderr,"\n+-------+----------+---------+---------+\n");
    }else if((verbose >= 3)&&(type == 1)){
    fprintf(stderr,"|%s\t|%d\t|U+%04X\t|0x%X\t|0x%04X%X\t|\n","NONE",number_of_bytes ,codepoint, input, w1, w2);
    fprintf(stderr,"\n+-------+----------+---------+---------+---------+\n");
    }else if((verbose == 1)&&(type == 2)){
        if(ascii==0)
            fprintf(stderr,"|%s\t|%d\t|U+%04X\t|\n","NONE",number_of_bytes ,codepoint);
        else
            fprintf(stderr,"|%c\t|%d\t|U+%04X\t|\n",ascii,number_of_bytes ,codepoint);
    fprintf(stderr,"\n+-------+----------+---------+\n");
    }else if((verbose == 2)&&(type == 2)){
        if(ascii==0)
            fprintf(stderr,"|%s\t|%d\t|U+%04X\t|0x%X\t|\n","NONE",number_of_bytes ,codepoint, input);
        else
            fprintf(stderr,"|%c\t|%d\t|U+%04X\t|0x%X\t|\n",ascii,number_of_bytes ,codepoint, input);

        fprintf(stderr,"\n+-------+----------+---------+---------+\n");
    }else if((verbose >= 3)&&(type == 2)){
        if(ascii==0)
            fprintf(stderr,"|%s\t|%d\t|U+%04X\t|0x%X\t|0x%04X\t|\n","NONE",number_of_bytes ,codepoint, input, w1);
        else
            fprintf(stderr,"|%c\t|%d\t|U+%04X\t|0x%X\t|0x%04X\t|\n",ascii,number_of_bytes ,codepoint, input, w1);

        fprintf(stderr,"\n+-------+----------+---------+---------+---------+\n");
    }

}


void print_header(unsigned int verbose){
if(verbose != 0){

        if(verbose == 1){  
             fprintf(stderr,"+-------+----------+---------+\n"
                     "| ASCII   | # of bytes | codepoint |"
                     "\n+-------+----------+---------+\n");
        }else if(verbose == 2){
             fprintf(stderr,"+-------+----------+---------+---------+\n"
                     "| ASCII   | # of bytes | codepoint |    input    |"
                     "\n+-------+----------+---------+---------+\n");
        }else if(verbose >=3){
             fprintf(stderr,"+-------+----------+---------+---------+---------+---------+\n"
                     "| ASCII   | # of bytes | codepoint |    input    |    output    |"
                     "\n+-------+----------+---------+---------+---------+---------+\n");  
        }
    }
}
/*helped for diff archs*/
int reverseInt (int val, char *type) {
    unsigned char o[4];
    unsigned char c[4];
    unsigned int e = 1, answer = 0, answer_two = 0,i;
    char *c1 = (char*)&e;

    if (*c1) {
        if((type != NULL)&&(strcmp(type,"UTF-16BE")==0)){
           /* printf("%s\n", "Inside reverse int");*/
        for(i = 0; i < 4; i++){
            o[i] = (unsigned char)((val)>>(8*i));
        }
        answer_two |= (o[3]<< 16) | (o[2]<< 24)|(o[1]<< 0)|(o[0]<< 8);
        return answer_two;

        }else{
            return val;
        }
    } else {
        int i, j = 0;
        for(i = 0; i < 4;i++, j+=8){
            c[i] = (val >> j) & 255;

        }
        i = 0, j = 24;
        for(i = 0; i < 4;i++, j-=8){
            answer += c[i] << j;
        }
        /*printf("%s\n", "Big endian ubuntu");*/


        if((type != NULL)&&(strcmp(type,"UTF-16BE")==0)){
            /*printf("%s\n", "Inside reverse int");*/
        for(i = 0; i < 4; i++){
            o[i] = (unsigned char)((answer)>>(8*i));
        }
        answer_two |= (o[3]<< 16) | (o[2]<< 24)|(o[1]<< 0)|(o[0]<< 8);
        return answer_two;

        }else{
            return answer;
        }
    }

 }
bool convert_to_UTF16LEBE_from_UTF16BELE(const int input_fd,const int output_fd,unsigned int verbose, char *file_type){
    bool success = false;
    unsigned int bytes_write = 0;
    ssize_t bytes_read;
    unsigned short  read_value = 0;
        if(input_fd >= 0 && output_fd >= 0) {
            while((bytes_read = read(input_fd, &read_value, 2)) == 2){
                if(!is_big_endian())
                    bytes_write = flip_short(read_value);
                else
                   bytes_write = read_value;

               if((read_value<=0x7F)&&(read_value>=20)){
                    printTable(read_value,2, read_value, read_value, read_value, 0,2, verbose);
               }else{
                    printTable('\0',2, read_value, read_value, read_value, 0,1, verbose);

               }
            if(!safe_write(output_fd, &bytes_write,2, "")){
                goto encode_done;
            }

        }
        success = true;
    }
encode_done:
    return success;

}


bool convert_to_same(const int input_fd,const int output_fd,unsigned int verbose, char *evalue){
    bool success = false;
    unsigned int bytes_write = 0;
    ssize_t bytes_read;
        if(input_fd >= 0 && output_fd >= 0) {
            while((bytes_read = read(input_fd, &bytes_write, 1)) == 1){
                if(is_big_endian())
                    bytes_write = flip(bytes_write);

               if((bytes_write<=0x7F)&&(bytes_write>=20)){
                    printTable(bytes_write,2, bytes_write, bytes_write, bytes_write, 0,2, verbose);
               }else{
                    printTable('\0',2, bytes_write, bytes_write, bytes_write, 0,1, verbose);

               }
            if(!safe_write(output_fd, &bytes_write, 1, "")){
                goto encode_done;
            }

        }
        success = true;
    }
encode_done:
    return success;
}

bool convert_to_utf8(const int input_fd,const int output_fd,unsigned int verbose, char *file_type){
    /*check if input file is good*/
    bool success = false;
    unsigned int surrogate = 0, bytes =0, bytes_to_write = 0;
    ssize_t bytes_read;
    unsigned short  read_value = 0;

    if(input_fd >= 0 && output_fd >= 0) {


while((bytes_read = read(input_fd, &read_value, 2)) == 2){
            if(strcmp(file_type,"UTF-16BE")==0){
                if(is_big_endian()){
                }else{
                read_value = flip_short(read_value);
            }
            }else{
                read_value = flip_short(read_value);
                /*SO CONFUSING BUT FLIP BECAUSE SPARKY*/
                if(is_big_endian()){
                    read_value = flip_short(read_value);
                }
            }
             unsigned int masked_value = read_value & 0xD800;
        if(masked_value == 0xD800){
            unsigned int w1 = read_value;
           if((bytes_read = read(input_fd, &read_value, 2)) == 2){
            if(strcmp(file_type,"UTF-16BE")==0){
                if(is_big_endian()){
                }else{
                read_value = flip_short(read_value);
            }
            }else{
                read_value = flip_short(read_value);
                /*SO CONFUSING BUT FLIP BECAUSE SPARKY*/
                if(is_big_endian()){
                    read_value = flip_short(read_value);
                }
            }

                masked_value = read_value & 0xDC00;
                if(masked_value == 0xDC00){
                    unsigned int w2 = read_value;
                    /*w1 & w2 input*/
                    w1 = w1 - 0xD800;
                    w2 = w2 - 0xDC00;

                    w1 = (w1<<10);
                    surrogate = w1 | w2;

                    surrogate += 0x10000;

                    unsigned int output = get_hex(surrogate, &bytes, &bytes_to_write);
                    // while(bytes > 0){
                    output = flip(output);
                    safe_write(output_fd, &output,bytes_to_write, "");

                    printTable('o',bytes_to_write, surrogate, read_value, output, 0,2, verbose);

                    // bytes--;
                // }
                }else{
                    /*else it was a non surrogate, we store first*/
                    /*and put the second one back on file*/
                    if(!safe_write(output_fd, &w1, 2,"")) {
                        /* Assembly for some super efficient coding */
                    }
                    if((lseek(input_fd, -2, SEEK_CUR) < 0)) {
                            perror("NULL");
                        }

                }
            }else{
                           return false;
            }
        }else{
            /*already codepoint*/
            // read_value = reverseInt(read_value, "UTF-16BE");
            unsigned int output = get_hex(read_value, &bytes, &bytes_to_write);
            if((read_value <= 0x7F)&&(read_value >= 20)){
                    printTable(read_value,bytes_to_write, read_value, read_value, output, 0,2, verbose);
                }else{
                    printTable('\0',bytes_to_write, read_value, read_value, output, 0,2, verbose);
                }

            safe_write(output_fd, &output,bytes_to_write, "");
        }
    }

    success = true;
    }
// encode_done:
    return success;
}


unsigned int get_hex(unsigned int codepoint,unsigned int *bytes, unsigned int *bytes_to_write){
    unsigned int masked_value =0, temp = 0;
    /*ascii 1*/
    if(codepoint <=0x7F){
        *bytes = 1;
        *bytes_to_write = 1;
        return codepoint;
    }else if(codepoint <=0x7FF){
        if((codepoint&0xFF00)>0){
            *bytes = 2;
        }else{
            *bytes = 1;
        }
        temp = (codepoint&0x3F);
        temp |= 0x80;
        codepoint= (codepoint&0x7C0)<<2;
        codepoint |= temp|0xC000;
        *bytes_to_write = 2;
        return codepoint; 
    }else if(codepoint <= 0xFFFF){
        temp = (codepoint&0x3F);
        temp |= 0x80;
        masked_value = (codepoint&0xFC0)<<2;
        masked_value |= 0x8000;
        codepoint = (codepoint&0xF000)<<4;
        codepoint|=0xE00000;
        codepoint |= (temp|masked_value);
        *bytes = 2;
        *bytes_to_write = 3;

        codepoint =flip(codepoint);
        codepoint = codepoint>>8;
        return codepoint;
    }else if(codepoint <= 0x1FFFFF){/*11110x xx 10xx xxxx 10xxxx xx 10xx xxxx*/
        temp = (codepoint&0x3F);
        temp |= 0x80;
        masked_value = (codepoint&0xFC0)<<2;
        masked_value |= 0x8000;
        temp |= masked_value;
        masked_value = (codepoint&0x3F000)<<4;
        masked_value |= 0x800000;
        temp |= masked_value;
        codepoint = (codepoint&0x1F0000)<<4;
        codepoint |= (0xF0000000|temp);
        *bytes = 4;
        *bytes_to_write = 4;
        return codepoint; 
    }else{
        USAGE("");
        exit(1);
    }
}



bool is_big_endian(){
        unsigned int e = 1;
        char *c1 = (char*)&e;

    if (!*c1) {
        return true;
    }else{
        return false;
    }
}

short flip_short(short val){
    unsigned char c[2];
    unsigned int answer = 0;
        int i, j = 0;
        for(i = 0; i < 2;i++, j+=8){
            c[i] = (val >> j) & 255;

        }
        i = 0, j = 8;
        for(i = 0; i < 2;i++, j-=8){
            answer += c[i] << j;
        }

        return answer;
}
 int flip(int val){

    unsigned char c[4];
    unsigned int answer = 0;
        int i, j = 0;
        for(i = 0; i < 4;i++, j+=8){
            c[i] = (val >> j) & 255;

        }
        i = 0, j = 24;
        for(i = 0; i < 4;i++, j-=8){
            answer += c[i] << j;
        }

        return answer;
 }