#include "hw1.h"


int main(int argc, char *argv[]){
	
	/*int help_menu = 0, message = 0;*/
	int option[2], i = 1, error = 1;

	/*printf("%d i %d\n", argc, i);*/
	option[0] = option[1] = -1;/*initialize var for options*/


	if(argc <= 3 && argc >= 2){
		/*inner*/
		while(argc > i){
		if(*argv[i] == '-'){
			/*printf("%s\n", "yes");*/
			if((strlen(argv[i]) == 2) && ((option[i-1] = set_option(*++argv[i])) != -1)){
				
			}else{
				fprintf(stderr, "usage: ./mstat [option] [-u]\n");
				return EXIT_FAILURE;/*exit with error*/
			}

		}else{
			fprintf(stderr, "usage: ./mstat [option] [-u]\n");
			return EXIT_FAILURE;/*exit with error*/
		}
		i++;
	}/*endwhile*/
		/*inner*/

	}/*end initial if*/
	if((option[1]!=3)&&(option[1]!=2)&&(option[1]!=1)&&(option[0]!= option[1])&&(option[0] != -1) 
	&& (option[0] != 4) && (option[1] != 0) && !((option[0] == 0) && (option[1] == 4))){/*check if we have a option in there*/
		switch(option[0]){
			case 0:
				display_help_information();
				break;
			case 1:
				error = display_instruction_information(option[1]);
				break;
			case 2:
				error = display_register_information(option[1]);
				break;
			case 3:
				error = display_opcode_information(option[1]);
				break;
			default:
				fprintf(stderr, "unknown error: ./mstat -h\n");
				return EXIT_FAILURE;
				break;
			}
	}else{
		fprintf(stderr, "usage: ./mstat [option] [-u]\n");
		return EXIT_FAILURE;
	}

	if(error==0)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

/*Display help menu*/
void display_help_information(void){
printf("Usage:\t./mstat [OPTION]\n\t./mstat -h\t\tDisplays this help menu.\n");
printf("\t./mstat -i [-u]\t\tDisplays statistics about instruction types.\n");
printf("\t./mstat -r [-u]\t\tDisplays information about the registers.\n");
printf("\t./mstat -o [-u]\t\tDisplays number and percentage of opcodes used.\n\n");
printf("Optional flags:\n-u          Displays human readable headers for the different outputs.\n");

}

double totalpercentage(double count, double total){
	return ((count/total)*100.0);
}

/*Returns a char depicting waht option of instruction it is*/
char type_of_instruction(unsigned int number){
	number = (number >> 26);

	if(number == 0)
		return 'r';
	else if((number == 2)||(number == 3))
		return 'j';
	else
		return 'i';

}

/*returns a number depending on option*/
int set_option(char option){

	switch(option){
		case 'h':
			return 0;
		case 'i':
			return 1;
		case 'r':
			return 2;
		case 'o':
			return 3;
		case 'u':
			return 4;
		default:
			return -1;
	}
}
/*displays info about instructions
 *Param u: status of u*/
int display_instruction_information(int u){
	unsigned int number = 0;
	char string[MAXSIZE];
	char *end;
	int c = 0;
	unsigned int countj = 0, counti = 0, countr = 0;
	double total = 0.0;
		while(scanf("%s", string) > 0){
			/*unsigned long long int test = 0;*/
			if((strlen(string) < 11)){
				number = strtoull(string, &end, 16);
			if(*end == '\0'){
				c = type_of_instruction(number);
				/*printf("%s\n", c);*/
				if(c == 'i')
					counti++;
				else if(c == 'j')
					countj++;
				else 
					countr++;
			}else{
				fprintf(stderr, "bad hex %s\n", string);
				return 0;
			}
		}else{
				fprintf(stderr, "bad hex %s\n", string);
			return 0;
		}/*for size*/
	}
		total = counti+countj+countr;
		/*printf("%f\n", total);*/
		if(u == 4)
			printf("TYPE\tCOUNT\tPERCENT\n");

		printf("I-Type\t%d\t%.1f%%\n", counti, totalpercentage(counti, total));
		printf("J-Type\t%d\t%.1f%%\n", countj, totalpercentage(countj, total));
		printf("R-Type\t%d\t%.1f%%\n", countr, totalpercentage(countr, total));

		return 1;
}
/*check register information 
*return 0 if there was an error*/
int display_register_information(int u){
	char *regNames[] = {"zero","at","v0","v1",
						"a0", "a1", "a2", "a3", "t0", 
						"t1", "t2", "t3", "t4", "t5", 
						"t6", "t7", "s0", "s1", "s2", 
						"s3", "s4", "s5", "s6", "s7", 
						"t8", "t9", "k0", "k1", "gp", 
						"sp", "fp", "ra"};
char string[MAXSIZE];
	unsigned int number = 0, temp = 0, shift = 6, i = 0, c =0, j = 0;
	double total = 0.0;
	int reg[2][32];
	char *end;
	/*init array*/
	for(i =0 ; i < 2; i++)
		for(j = 0; j < 32; j++)
			reg[i][j] = 0;


		while((scanf("%s", string) > 0)){
			/*The num is legit check here*/
			if((strlen(string) < 11)){
				number = strtoull(string, &end, 16);
				// printf("test %c\n", (*end == '\0')? 'y':'n');
			if(*end == '\0'){
				c = type_of_instruction(number);/*check type of instruction*/
				/*now we calculate depending on the type*/
				switch(c){
					case 'r':
						i = 3;
						j = 0;
						total++;
						break;
					case 'i':
						i = 2;
						j = 1;
						total++;
						break;
					case 'j':
					default:
						i = 0;
						total++;
						break;
				}
						while(i > 0){
							temp = number << (shift);
							temp = temp >> (27);
							(reg[j][temp])++;
							shift+= 5;
							i--;
						}
						shift = 6;

			}else{/**/
				fprintf(stderr, "bad hex %s\n", string);
				return 0;		
			}
		}else{/*if for check length*/
				fprintf(stderr, "bad hex %s\n", string);
				return 0;	
		}
	}
		/*if -u is set we show the bottom, else top*/
		if(u != 4){
		for(i = 0; i < 32; i++){
			printf("$%d\t%d\t",i, (reg[0][i] + reg[1][i]));
			for(j = 0; j < 2; j++){
				printf("%d\t",reg[j][i]);
			}
			printf("%d\t%.1f%%\n",0, totalpercentage(reg[0][i]+reg[1][i], total));
		}
	}else{
		printf("%s\n", "REG\tUSE\tR-TYPE\tI-TYPE\tJ-TYPE\tPERCENT");
		for(i = 0; i < 32; i++){
			printf("$%s\t%d\t",regNames[i], (reg[0][i] + reg[1][i]));
			for(j = 0; j < 2; j++){
				printf("%d\t",reg[j][i]);
			}
			printf("%d\t%.1f%%\n",0, totalpercentage(reg[0][i]+reg[1][i], total));
		}		
	}
			return 1;
}
/*check opcode information 
*return 0 if there was an error, else 1*/
int display_opcode_information(int u){
	unsigned int opcode[64],func[64], i = 0, number = 0;
	double totalOp =0.0, totalFun = 0.0;
	char string[MAXSIZE];
	char *end;

	for(i = 0; i < 64; i++)
		opcode[i] = func[i] = 0;

	while(scanf("%s", string) > 0){
			/*The num is legit check here*/
		if((strlen(string) < 11)){
				number = strtoull(string, &end, 16);
		if(*end == '\0'){
				opcode[(number>>26)]++;
				totalOp++;
				if(!(number >> 26)){
					func[number&0x3F]++;
					totalFun++;
				}
		}else{/*for bad char in num*/
				fprintf(stderr, "bad hex %s\n", string);
				return 0;
		}
	}else{/*for bad length*/
			fprintf(stderr, "bad hex %s\n", string);
			return 0;
	}
}

	/*Print information*/
	if(u == 4)
		printf("OPCODE\tCOUNT\tPERCENTAGE\n");

	for(i = 0; i < 64; i++)
		printf("0x%X\t%d\t%.1f%%\n",i, opcode[i], totalpercentage(opcode[i], totalOp));

	printf("\n");

	if(u == 4)
		printf("FUNC\tCOUNT\tPERCENTAGE\n");

	for(i = 0; i < 64; i++)
		printf("0x%X\t%d\t%.1f%%\n",i, func[i], totalpercentage(func[i], totalFun));

	return 1;
}