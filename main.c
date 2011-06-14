// definicion de librerias
#include <stdio.h>
#include <unistd.h> /* requerido por getopt*/
#include <getopt.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>

// definicion de variables globales

const char* name = "flash_load";

int argc;

/*Opciones validas*/
const char* short_opts = "hl:d:b:f:a:";
/*
h : help
l : load
d : device
b : baud
f : binary file
a : address

hay que incluir los procecesos
*/

const struct option long_opts[]=
{
	{"help",	0,	NULL,	'h'},
	{"log", 	1,	NULL,	'l'},
	{"device",	1,	NULL,	'd'},
	{"baud",	1,	NULL,	'b'},
	{"file",	1,	NULL,	'f'},
	{"address",	1,	NULL,	'a'},
	{NULL,		0,	NULL,	0}
};


/* Funciones */

void print_help()
{
	printf("Usage: %s [OPTION] -f binary-file... \n", name);
	printf("Options:\n");
	printf("\t -h, --help \t Display this information\n");
	printf("\t -l, --log <file> \t Place information abuot the process in <file>\n");
	printf("\t -d, --device <arg> \t Specifies the UART port. /dev/ttyUSB0 is used by default\n");
	printf("\t -b, --baud <arg> \t Specifies the baud rate of the serial comunication. Possible baudrates are: 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 46080. By default the baud rate is set to 3800\n");

/*decir los posibles valores que puede tomar*/
/*
     B0  B50  B75  B110  B134  B150  B200
     B300  B600  B1200  B1800  B2400  B4800
     B9600  B19200  B38400  B57600  B115200
     B230400  B46080
*/
	printf("\t -a, --addres <arg> \t Specifies the addres for read, write or verify in the flash memory\n");
	printf("\t -f, --file <file> \t Specifies the binary file intended to load into the flash memory");
	printf("Created by Carlos Barreto. cabarretos@correo.udistrital.edu.co\n");
}

int convert_char_int(const char* arg)
{
	
}

int file_exists (const char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;   
}

  


write_data(int usbdev, char command)
{
	char c[5]; 
	c[0] = command;
	int wr = write(usbdev, c, 1);
	if (wr == -1)
	{
		printf("Eror: Error writing data at the UART port\n");
		exit(EXIT_SUCCESS);
	}
	return 0;
}

 /*
write_data_ptr(int usbdev, char *command)
{
	char c[5]; 
	c[0] = *command;
	int wr = write(usbdev, c, 1);
	if (wr == -1)
	{
		printf("Eror: Error writing data at the UART port\n");
		exit(EXIT_SUCCESS);
	}
	return 0;
}*/

delay(unsigned long int wait_time)
{
	struct timeval tv;
	struct timezone tz;	
	unsigned long int time, start,end;

	start = tv.tv_usec;

	gettimeofday(&tv, &tz);
	end = tv.tv_usec;
	int cont = 0;
	time = end-start;
	while (time < wait_time)
	{	
		cont = cont+1;
		gettimeofday(&tv, &tz);
		end = tv.tv_usec;
		time = end-start;
		
	}
	printf("time = %u, wait_time = %u, cont = %i\n",time, wait_time, cont);
}


char read_data_f(int usbdev,  unsigned long int wait_time, int n_wait_states)
{
	struct timeval tv;
	struct timezone tz;	
	unsigned long int time, start,end;
	start = tv.tv_usec;
	time = 0;
	int wait_ = 0;
	int rd = 0;
	int cont = 0;
	char response_[5] = {0,0,0,0,0};

	// establecer tiempo de incio
	gettimeofday(&tv, &tz);

	printf("n wait_states %d\n",n_wait_states);
	while(rd < 1)
	{
		rd = read(usbdev, response_,1);
		gettimeofday(&tv, &tz);
		end = tv.tv_usec;
		time = end-start;
		//printf("\nwait_=%d, time %u\nstart= %u, end = %u, wait_time = %d\n ",wait_,time,start, end, wait_time);
		cont = cont+1;
	
		if(time > wait_time)
		{
			printf("Error: Exceeded wait time \nSending null character '~'\n");
			write_data(usbdev, 0x7e);
			wait_ = wait_+1;
			time = 0;
			gettimeofday(&tv, &tz);
			start = tv.tv_usec;
			//gettimeofday(&tv, &tz);
			//end = tv.tv_usec;		
			if(wait_ == n_wait_states)
			{
				printf("Error: Esceeded wait states.. Exit\n");
				exit(EXIT_SUCCESS);
			}
		}
		
	}
	printf("response = %x, cont = %d\n",response_[0], cont );
	
	return response_[0];
} 


char read_data_b(int usbdev,  unsigned long int wait_time, int n_wait_states)
{
	struct timeval tv;
	struct timezone tz;	
	unsigned long int start,end;
	start = tv.tv_usec;
	int time = 0;
	int wait_ = 0;
	int rd = 0;
	int cont = 0;
	char response_[5] = {0,0,0,0,0};

	// establecer tiempo de incio
//	gettimeofday(&tv, &tz);
	

	rd = read(usbdev, response_,1);
	printf("n wait_states %d\n",n_wait_states);
	while(rd < 1)
	{
		delay(wait_time);
		rd = read(usbdev, response_,1);
		if (rd >= 1)
		{
			break;
		}
		else if(wait_ == n_wait_states)
		{
			printf("Error: Esceeded wait states.. Exit\n");
			exit(EXIT_SUCCESS);
		}
		else
		{
			write_data(usbdev, 0x3e);
			//delay(wait_time);
			printf("Error: Exceeded wait time \nSending null character '~'\n");
			wait_ = wait_+1;
		}
/*		gettimeofday(&tv, &tz);
		end = tv.tv_usec;
		time = end-start;
		//printf("\nwait_=%d, time %u\nstart= %u, end = %u, wait_time = %d\n ",wait_,time,start, end, wait_time);
*///		cont = cont+1;
	
//		if(time > wait_time)
//		{
//			printf("Error: Exceeded wait time \nSending null character '~'\n");
//			write_data(usbdev, 0x7e);
			
//			time = 0;
//			gettimeofday(&tv, &tz);
//			start = tv.tv_usec;
//			gettimeofday(&tv, &tz);
//			end = tv.tv_usec;		

		//}
		
	}
	printf("response = %x\n",response_[0] );
	
	return response_[0];
} 



char read_data_c(int usbdev,  unsigned long int wait_time, int n_wait_states)
{

	double start,end, time, clock_;
	clock_ = (double) clock();
	start = (double) clock() * (1000 / CLOCKS_PER_SEC);
	time = 0;
	int wait_ = 0;
	int rd = 0;
	int cont = 0;
	char response_[5] = {0,0,0,0,0};

	// establecer tiempo de incio
//	gettimeofday(&tv, &tz);

	printf("n wait_states %d, wait_time = %d, clock = %f, clocks_per_sec = %d\n",n_wait_states, wait_time, clock_, CLOCKS_PER_SEC);
	
	while(rd < 1)
	{
		rd = read(usbdev, response_,1);
//		gettimeofday(&tv, &tz);
		end = (double) clock() * (1000 / CLOCKS_PER_SEC);
		time = end-start;
		printf("\nwait_=%d, time %u\n start= %f, end = %f, wait_time = %d\n ",wait_,time,start, end, wait_time);
		cont = cont+1;
//exit(EXIT_SUCCESS);
		if(time > wait_time)
		{
			printf("Error: Exceeded wait time \nSending null character '~'\n");
			write_data(usbdev, 0x7e);
			wait_ = wait_+1;
			time = 0;
//			gettimeofday(&tv, &tz);
			start = (double) clock() * (1000 / CLOCKS_PER_SEC);
//			gettimeofday(&tv, &tz);
//			end = clock() * CLK_TCK/1000		
			if(wait_ == n_wait_states)
			{
				printf("Error: Esceeded wait states.. Exit\n");
				exit(EXIT_SUCCESS);
			}
		}
		
	}
	printf("response = %x, cont = %d\n",response_[0], cont );
	
	return response_[0];
} 



char read_data_d(int usbdev,  unsigned long int wait_time, int n_wait_states)
{

	struct timeval start, end, clock_;
	double time = 0;
	gettimeofday(&start,NULL);
	int wait_ = 0;
	int rd = 0;
	char response_[5] = {0,0,0,0,0};

//	printf("n wait_states %d, wait_time = %d, clocks_per_sec = %d\n",n_wait_states, wait_time, CLOCKS_PER_SEC);
//	printf("start_s = %d, start_us = %d\n",start.tv_sec, start.tv_usec);
	while(rd < 1)
	{
		rd = read(usbdev, response_,1);
		gettimeofday(&end,NULL);
		time = end.tv_sec-start.tv_sec;
		//printf()
		if(time >= wait_time)
		{
			//printf("send ..\n");
			if(wait_ == n_wait_states)
			{
				printf("Error: Esceeded wait states.. Exit\n");
				exit(EXIT_SUCCESS);
			}
			printf("Error: Exceeded wait time \nSending null character '~'\n");
			write_data(usbdev, 0x3e);
			wait_ = wait_+1;
			time = 0;
			gettimeofday(&start,NULL);			
		}
	}
	//printf("incremento sec ... end_s = %d, end_us = %d\ncont = %d, time = %d\n",end.tv_sec, end.tv_usec, cont, time);
	//printf("response = %x, cont = %d\n",response_[0], cont );
	return response_[0];
} 




/*	funcion que determina señales a enviar*/
// instructions
char determine_instr_signal(int data_width)
{
	switch(data_width)
	{
		case 1:		// i
			return 0x69;
			break;
		case 2:		// I
			return 0x49;
			break;
		case 4:		//^I
			return 0x09;
			break;
		default:
			printf("Error: %d bit width not supported\n", data_width);
			exit(EXIT_SUCCESS);
			break;
	}
	return 0;
}

// address
char determine_addr_signal(int data_width)
{
	switch(data_width)
	{
		case 1:		// i
			return 0x01;//0x61;
			break;
		case 2:		// I
			return 0x01;//0x41;
			break;
		case 4:		//^I
			return 0x01;//0x01
			break;
		default:
			printf("Error: %d bit width not supported\n", data_width);
			exit(EXIT_SUCCESS);
			break;
	}
	return 0;
}


extract_bytes(int data, char *data_array, int data_width)	// puede ser mas rapido si se da el numero de bytes a extraer
{
//	char *data;
//	data = 
	//int *addr = &prog_addr;
	//printf("data = %x\n",data);
	char *ptr_array;
	ptr_array = data_array;
	char byte = data;
	int i;
	for(i=0; i<data_width; i++)
	{
		byte = data;
		//printf("byte = %x\n", byte);
		*ptr_array = byte;
		ptr_array ++;
		data = data >> 8;
	}
	return 0;
}

/*######################################################################*/
/*				verify					*/
/*######################################################################*/

verify_instructions(int usbdev, int address, int sys_addr, int prog_addr, int data_width, int long_read, char *data_array, unsigned long int wait_time, int n_wait_states, int offset_, const char* bin_file, const char* mod, int b_check, const char* log_file)
{
// es mejor poner apuntador a las variables

//printf("address = %x, sys_addr = %x\n",address, sys_addr);

	char *ptr_addr_array;
	char addr_array[3];
	char head_char;
//	char *ptr_head_char;
//	ptr_head_char = &head_char;
	int i;
	char read_data[5]={0,0,0,0,0};
	int temp;
	int temp_read[data_width-1];
	int data;
	int bytes_number = 0;
	int count_data = prog_addr & 0x0f;
	int print_addr = 1;
	int k = 0;
	int j = 0;
	ptr_addr_array =  addr_array;
	char *ptr_f_data_array;
	char f_data_array[3];
	int f_data_array_[3];
	int errors = 0;

/////////////////////////////////////////////////////////////////////////
// almacenar archivo en el buffer

FILE *log_f; 
log_f = fopen(log_file,"a+");

	FILE *f;
	unsigned long file_length;
	char* buff;//[8] = "00000000";
	int instruction;
	k = offset_;
//	address = address & (~0x1);

	f = fopen(bin_file, mod);
	if(!f)
	{
		printf("Error opening file %s", bin_file);
		return 1;
	}
	// file length
	fseek(f, 0, SEEK_END);
	file_length = ftell(f);
	fseek(f, 0, SEEK_SET);

	//Allocate memory
	buff=(char *)malloc(file_length+1);
	if (!buff)
	{
		fprintf(stderr, "Memory error!");
                                fclose(f);
		return;
	}	


	fread(buff, file_length, 1, f);
	fclose(f);



while(k < file_length)//(segment_size+offset)
	{
	

	read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
	//printf("data received_a: %x\n",read_data[0]);
		switch(read_data[0])
		{
			case 0x3c:
				printf("Error: Fail sending the instruction's address\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				//printf("data received: %x\n",read_data[0]);
	//			printf("data received: '$'\n");
				break;
			default:
				printf("Error: Fail sending the instruction's address\n");
				exit(EXIT_SUCCESS);
				break;	
		}




	for(j = 0; j < data_width; j ++)
	{
	//	printf("%02x",((char *)buffer)[k+j]);
		f_data_array[data_width-1-(j)] = ((char *)buff)[k+j];
	//	f_data_array_[data_width-1-(j)] = ((int *)buff)[k+j];
	//	printf("j = %d, f_data_array = %x",j, f_data_array[j]);
	}
	k = k + data_width;
	ptr_f_data_array =  f_data_array;
//	write_instructions(usbdev, address, sys_addr, prog_addr, data_width, ptr_data_array, wait_time, n_wait_states);
	//printf("\n");
	


//	while(bytes_number <= long_read)	// cambiar a contador de lecturas (bytes)
//	{
	
		

		if(sys_addr !=  prog_addr)	// write address
		{
			//printf("prog_addr = %x, sys_addr = %x\n",prog_addr, sys_addr);
			head_char = determine_addr_signal(data_width);
			write_data(usbdev, head_char);
			extract_bytes(prog_addr*1, ptr_addr_array, 4);
	
	//		ptr_command = command;
		
			for(i = 0; i<4; i++)		
			{
				write_data(usbdev, addr_array[i]);
				//ptr_addr_array ++;
			}
			read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
			//printf("data received_b: %x\n",read_data[0]);
			switch(read_data[0])
			{
				case 0x3c:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;
				case 0x24:
					sys_addr = prog_addr;
					//printf("data received: %x\n",read_data[0]);
					break;
				default:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;	
			}	

		//exit(EXIT_SUCCESS);
		}
//printf("prog_addr = %x, sys_addr = %x\n",prog_addr, sys_addr);
		head_char = determine_instr_signal(data_width);
		//printf("sending instruction signal %x...\n",head_char);
		write_data(usbdev, head_char);
	
	
	
		for (i=0; i< data_width; i++)	// read data from uart
		{
			read_data[data_width-1-i] = read_data_d(usbdev, wait_time, n_wait_states);
			//temp = 0x000000ff & ((int)read_data[i]);
			//data = temp;
			//printf("response %d = %x, data = %x\n",i,read_data[i],temp);
			
		}
		

		/*if( sys_addr == 6)
		{
			exit(EXIT_SUCCESS);
		}*/

			// print data

		/*if(print_addr == 1)	// print address
		{
			printf("%x \t",prog_addr);
			print_addr = 0;
		}*/

		for(i=0; i<data_width; i++)	// print data
		{
			temp_read[i] = 0x000000ff & ((int)read_data[i]);
//			printf("%02x",temp);
//			count_data ++;
//			if((count_data%2) == 0)
//			{
//				printf(" ");
//				count_data = 0;
//			}
		}

/*
	for(j = 0; j < data_width; j ++)
	{	
		//if ((f_data_array[j]) != temp_read[j]){
			printf("%02x-",( 0x000000ff & ((int)f_data_array[data_width-1-j])));//data_width-
		//	printf("Error on address: %x, Data mistmatch \n", prog_addr+j);
		//}
	}
printf("------");

	for(j = 0; j < data_width; j ++)
	{	
		//if ((f_data_array[j]) != temp_read[j]){
			printf("%02x-", temp_read[j]);//data_width-
		//	printf("Error on address: %x, Data mistmatch \n", prog_addr+j);
		//}//data_width-
	}
printf("------\n");
*/

 /* apend file (add text to 
a file or create a file if it does not exist.*/ 
//fprintf(file,"%s","This is just an example :)"); /*writes*/ 



//getchar(); /* pause and wait for key */ 
//return 0; 

for(j = 0; j < data_width; j ++)
	{	

		if(b_check == 1)
		{
			if (  0x000000ff  != temp_read[j]){
		
				printf("Error on address: %x, Data mistmatch \n", prog_addr+j);
			errors ++;
			fprintf(log_f,"Error on address: %x, Data mistmatch \n", prog_addr+j); /*writes*/ 
			}
		}
		else{
			if ( ( 0x000000ff & ((int)f_data_array[data_width-1-j])) != temp_read[j]){
				printf("Error on address: %x, Data mistmatch \n", prog_addr+j);
			errors ++;
			fprintf(log_f,"Error on address: %x, Data mistmatch \n", prog_addr+j); /*writes*/ 
			}

		}

	}
			//exit(EXIT_SUCCESS);
	/*
		for(i=0; i<data_width; i++)	// print data
		{
			temp = 0x000000ff & ((int)read_data[i]);
			printf("%02x",temp);
			count_data ++;
			if((count_data%2) == 0)
			{
				printf(" ");
//				count_data = 0;
			}
		}
	*/
	
//		count_data ++;
		//printf("\ncount_ = %d\n",count_data);
		prog_addr = prog_addr + data_width;
		sys_addr = sys_addr + data_width;



		//printf("\nprog_addr = %x\n",prog_addr);

/*		if(count_data >= 0x0f)//count_data  == 16
		{
			printf("\n");
			count_data = 0;	
			bytes_number ++;
			print_addr = 1;
		}
*/
//		exit(EXIT_SUCCESS);
/*
		while(scanf("%d",&data) != 1)
		{
		}
*/		
	}
	free(buff);
	fclose(log_f); /*done!*/ 
	printf("\nTotal errors in verify process:%d\n", errors);
	return 0;
}	


/*######################################################################*/
/*				read					*/
/*######################################################################*/

read_instructions(int usbdev, int address, int sys_addr, int prog_addr, int data_width, int long_read, char *data_array, unsigned long int wait_time, int n_wait_states)
{
// es mejor poner apuntador a las variables

//printf("address = %x, sys_addr = %x\n",address, sys_addr);

	char *ptr_addr_array;
	char addr_array[3];
	char head_char;
//	char *ptr_head_char;
//	ptr_head_char = &head_char;
	int i;
	char read_data[5]={0,0,0,0,0};
	int temp;
	int data;
	int bytes_number = 0;
	int count_data = prog_addr & 0x0f;
	int print_addr = 1;

	ptr_addr_array =  addr_array;

	while(bytes_number <= long_read)	// cambiar a contador de lecturas (bytes)
	{
	
		read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
	//printf("data received_a: %x\n",read_data[0]);
		switch(read_data[0])
		{
			case 0x3c:
				printf("Error: Fail sending the instruction's address\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				//printf("data received: %x\n",read_data[0]);
	//			printf("data received: '$'\n");
				break;
			default:
				printf("Error: Fail sending the instruction's address\n");
				exit(EXIT_SUCCESS);
				break;	
		}


		if(sys_addr !=  prog_addr)	// write address
		{
			//printf("prog_addr = %x, sys_addr = %x\n",prog_addr, sys_addr);
			head_char = determine_addr_signal(data_width);
			write_data(usbdev, head_char);
			extract_bytes(prog_addr*1, ptr_addr_array, 4);
	
	//		ptr_command = command;
		
			for(i = 0; i<4; i++)		
			{
				write_data(usbdev, addr_array[i]);
				//ptr_addr_array ++;
			}
			read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
			//printf("data received_b: %x\n",read_data[0]);
			switch(read_data[0])
			{
				case 0x3c:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;
				case 0x24:
					sys_addr = prog_addr;
					//printf("data received: %x\n",read_data[0]);
					break;
				default:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;	
			}	

		//exit(EXIT_SUCCESS);
		}
//printf("prog_addr = %x, sys_addr = %x\n",prog_addr, sys_addr);
		head_char = determine_instr_signal(data_width);
		//printf("sending instruction signal %x...\n",head_char);
		write_data(usbdev, head_char);
	
	
	
		for (i=0; i< data_width; i++)	// read data from uart
		{
			read_data[data_width-1-i] = read_data_d(usbdev, wait_time, n_wait_states);
			//temp = 0x000000ff & ((int)read_data[i]);
			//data = temp;
			//printf("response %d = %x, data = %x\n",i,read_data[i],temp);
			
		}
		
		/*if( sys_addr == 6)
		{
			exit(EXIT_SUCCESS);
		}*/

			// print data

		if(print_addr == 1)	// print address
		{
			printf("%x \t",prog_addr);
			print_addr = 0;
		}

	
		for(i=0; i<data_width; i++)	// print data
		{
			temp = 0x000000ff & ((int)read_data[i]);
			printf("%02x",temp);
			count_data ++;
			if((count_data%2) == 0)
			{
				printf(" ");
//				count_data = 0;
			}
		}
//		count_data ++;
		//printf("\ncount_ = %d\n",count_data);
		prog_addr = prog_addr + data_width;
		sys_addr = sys_addr + data_width;
		//printf("\nprog_addr = %x\n",prog_addr);
		if(count_data >= 0x0f)//count_data  == 16
		{
			printf("\n");
			count_data = 0;	
			bytes_number ++;
			print_addr = 1;
		}
//		exit(EXIT_SUCCESS);
/*
		while(scanf("%d",&data) != 1)
		{
		}
*/		
	}

	printf("\nend of read cycle\n");
	return 0;
}	




/*######################################################################*/
/*				write					*/
/*######################################################################*/

write_instructions(int usbdev, int address, int sys_addr, int prog_addr, int data_width, char *ptr_data_array, unsigned long int wait_time, int n_wait_states)
{
	char *ptr_addr_array;
	char data_a[3];
	char addr_array[3];
	char head_char;
	int i;
	char read_data[5]={0,0,0,0,0};
	int temp;
	int data;
	int bytes_number = 0;
	int count_data = prog_addr & 0x0f;
	int print_addr = 1;

	ptr_addr_array =  addr_array;
	//data_a = *ptr_data_array;
	//while(bytes_number < data_width)	// cambiar a contador de lecturas (bytes)
	//{
	


		if(sys_addr !=  prog_addr)	// write address
		{

			head_char = determine_addr_signal(data_width);
			write_data(usbdev, head_char);
			extract_bytes(prog_addr*1, ptr_addr_array, 4);
	
			for(i = 0; i<4; i++)		
			{
				write_data(usbdev, addr_array[i]);
//				ptr_addr_array ++;
			}
			read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
			//read_data[0] = 0x24;
			switch(read_data[0])
			{
				case 0x3c:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;
				case 0x24:
					sys_addr = prog_addr;
					break;
				default:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;	
			}	


		}

		head_char = determine_instr_signal(data_width);

		write_data(usbdev, head_char);	
		
		// write instriction
		//extract_bytes(0xabcd, ptr_data_array,data_width);
		//printf("     write: ");
		for (i=0; i< data_width; i++)	// read data from uart
		{
			//printf("%02x",*ptr_data_array);
			write_data(usbdev, *ptr_data_array);		
			ptr_data_array ++;	
		}
		//printf("     %02x%02x",data_a[0],data_a[1]);		
	//}
		//printf("\n");

		read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);

		switch(read_data[0])
		{
			case 0x3c:
				printf("Error: Fail sending the instruction, try again\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				//sys_addr = prog_addr;
				break;
			default:
				printf("Error: Fail sending the instruction\n");
				exit(EXIT_SUCCESS);
				break;	
		}	


//	printf("\nend of write cycle\n");
//free(ptr_data_array);
	return 0;
}




/*######################################################################*/
/*				erase					*/
/*######################################################################*/

erase_instructions(int usbdev, int address, int sys_addr, int prog_addr, unsigned long int wait_time, int n_wait_states, int data_width, char *data_array, int block_size, int long_erase)
{
	char *ptr_addr_array;
	char addr_array[3];
	
	char head_char;
	int i;
	char read_data[5]={0,0,0,0,0};
	int temp;
	int data;
	int erased_block = 0;
	int count_data = prog_addr & 0x0f;
	int print_addr = 1;


	ptr_addr_array =  addr_array;

	while(erased_block < long_erase)	// cambiar a contador de lecturas (bytes)
	{
	
		read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);

		switch(read_data[0])
		{
			case 0x3c:
				printf("Error: Fail sending instruction\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				break;
			default:
				printf("Error: Fail sending instruction\n");
				exit(EXIT_SUCCESS);
				break;	
		}

		if(sys_addr !=  prog_addr)	// write address
		{
			ptr_addr_array =  addr_array;
			head_char = determine_addr_signal(data_width);
			write_data(usbdev, head_char);
			extract_bytes(prog_addr*1, ptr_addr_array, 4);
	
			for(i = 0; i<4; i++)		
			{
				write_data(usbdev, addr_array[i]);
//				ptr_addr_array ++;
			}
			read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);
			//read_data[0] = 0x24;
			switch(read_data[0])
			{
				case 0x3c:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;
				case 0x24:
					sys_addr = prog_addr;
					break;
				default:
					printf("Error: Fail sending the instruction's address\n");
					exit(EXIT_SUCCESS);
					break;	
			}	


		}

		head_char = determine_instr_signal(data_width);
		printf("\n address = %x\n", prog_addr);
		write_data(usbdev, head_char);	// instruccin de borrar
		prog_addr = prog_addr+block_size;
		erased_block ++;
/*
		// write instriction
		extract_bytes(0x341256, data_array,data_width);

		for (i=0; i< data_width; i++)	// read data from uart
		{
			write_data(usbdev, data_array[i]);			
		}


*/
	}
		read_data[0] = read_data_d(usbdev, wait_time, n_wait_states);

		switch(read_data[0])
		{
			case 0x3c:
				printf("Error: Fail sending the instruction, try again\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				//sys_addr = prog_addr;
				break;
			default:
				printf("Error: Fail sending the instruction\n");
				exit(EXIT_SUCCESS);
				break;	
		}	


	printf("\nend of erase cycle\n");
	return 0;
}


speed_t determine_baud(int baud)
{
	switch(baud)
	{
		case 0:
			return B0;
		case 50:
			return B50 ; 
		case 75:
			return B75 ;
		case 110:
			return B110  ;
		case 134:
			return B134  ;
		case 150:
			return B150 ;
		case 200:
			return B200;
		case 300:
			return B300 ;
		case 600:
			return B600 ;
		case 1200:
			return B1200;
		case 1800:
			return B1800  ;
		case 2400:
			return B2400 ;
		case 4800:
			return B4800;
		case 9600:
			return B9600 ;
		case 19200:
			return B19200 ;
		case 38400:
			return B38400 ;
		case 57600:
			return B57600  ;
 
		case 115200:
			return B115200;
		case 230400:
			return B230400 ;
		case 460800:  // valida?????
			return B460800; ///
		default:
			printf("Invalid baudrate: %d\n", baud);
			exit(EXIT_SUCCESS);

	}
}




/* Our structure */
	struct rec
	{
		int x,y,z;
	};


/*
-------------------------------------------------------
-------------------------------------------------------
*/

//int getopt(argc, char * const argv[], );

// arreglar como se cuentan los bytes al escribir y leer los datos

int main(int argc, char **argv)
{
// definir variables

	int next_option;	
	const char* log = "program.log";
	const char* device = "/dev/ttyUSB0";
	int baud = 3800;
	speed_t baud_ = B38400;
	const char* baudrate = "3800";
	int address = 0x00000;//0x00001fff0;		// debe ser multiplo de 2
	const char* file = NULL;
	const char* mode = "rb";
	const char* address_ = NULL;	

	int usbdev;
	char command[5]; 			/*Buffer de datos para enviar*/
	char response[5] = {0,0,0,0,0}; 	/*Buffer de datos para recibir*/
/* queda pendiente revisar lo de las direcciones*/

	int rd = 0;
	int wr = 0;
	//clock_t start;		// take the start read time
	//float time;	
	unsigned long int wait_time;	// determine the wait states in each read.
	int n_wait_states = 10;	// wait states before read error
	unsigned char ready;
	int validations;


/* Establecer constantes de tiempo (segundos)*/
	wait_time = 2;
//	wait_time = 100*1000000/baud;
//	printf("wait_state = %i\n",wait_time);
//	exit(EXIT_SUCCESS);

/*######################################################*/
/*######################################################*/
/*		PROVISIONAL				*/
	int sys_addr 	= 0x0;
	int prog_addr	= 0x0;	// valor de addr que tiene el programa en cada momento
	int data_width 	= 2; 	// # bytes
	int long_read	= 0x20;
	int long_erase	= 1;//5
	char process 	= 'W';	// cambiarlo por r?????
	int blank_check = 0;	// 1: activado, 0 desactivado
	char process_;
	int block_size	= 0x20000;
	int k = 0;
	int j = 0;

	int offset = 0x0;//0x10000;
	int segment_size = 0x975fc;


		char *ptr_command; 


	unsigned long int pru_hex = 0x406c;



	if (argc == 1)
	{
		print_help();
		exit(EXIT_SUCCESS);
	}
	
	while(1)
	{	
		next_option = getopt_long(argc, argv, short_opts, long_opts, NULL);
		if(next_option == -1){ break;}
		
		switch(next_option)
		{	
			case 'h' :
				print_help();
				exit(EXIT_SUCCESS);
			case 'l' : 
				log = optarg;
				break;
			case 'd' :
				device = optarg;
				if(file_exists(device)==0)
				{
					printf("Device %s was not found \n", device);
					exit(EXIT_SUCCESS);
				}
				break;
			case 'b' : 
				baud = atof(optarg);
				baudrate = optarg;
				baud_ = determine_baud(baud);
				break;
			case 'f' : 
				file = optarg;
				if(file_exists(file)==0)
				{
					printf("File %s was not found \n", file);
					exit(EXIT_SUCCESS);
				}
				break;
			case 'a' : 
				address_ = optarg;// atof(optarg);
				break;
			default  :
				printf("%s: Invalid option.\n Try  '%s' --help\n", name, name);
				exit(EXIT_SUCCESS);			
		}
	}

/* verificar que se hayan ingresado las opciones obligatorias*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// proceso para abrir archivo

//	printf("a\n");


	FILE *fp;
	unsigned long file_len;
	char* buffer;//[8] = "00000000";
	int instr;

	address = address & (~0x1);
	







	//printf("log = %s, device=%s, baud = %d, file = %s, address = %d\n",log, device, baud, file, address);



/*Agregar una llamada al sistema para configurar la comunicacion*/
/*
	const char* part_a = "stty -F ";
	const char* part_b = " ";
	const char* part_c = " cs8 -cstopb -clocal -ignbrk -igncr";
//	const char* part_c = " cs8 -cstopb -parity -icanon min 1 time 1";
	//const char* part_ = "";
	//int a = strlen(part_a);
	//printf("a %d\n",a);


//	itoa(baud,baudrate,10);

	int long_sys_call = strlen(part_a)+strlen(device)+strlen(part_b)+strlen(baudrate)+strlen(part_c);
	//printf("b\n");

	char* system_call = malloc(long_sys_call);
	//char* sys_temp = malloc(long_sys_call);
	//Allocate memory
	buffer=(char *)malloc(fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(file);
		return;
	}
	strcpy(system_call, part_a);
	strcat(system_call, device);
	strcat(system_call, part_b);
	strcat(system_call, baudrate);
	strcat(system_call, part_c);

//	speed_t baud_ = malloc(strlen("B")+strlen(baud));
//	strcpy(baud_, "B");
//	strcat(baud_, baud);

	printf("system_call = %s\n", system_call);
	// "stty -F /dev/tty* 115200 cs8 -cstopb -parity -icanon min 1 time 1"

	//system(system_call);
*/
/*

//Abrir el dispositivo para lecturas/escrituras
	usbdev = open(device, O_RDWR| O_NOCTTY | O_NDELAY);
	if(usbdev == -1)
	{	
		printf("Error: Unable to open %s\n", device);
		exit(EXIT_SUCCESS);
	}
	fcntl(usbdev, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
//	fcntl(usbdev, F_SETFL,0);
	command[0] = 0x40;
//	command[1] = 0x57;


	rd = read(usbdev, response, 2);
	printf("rd = %d\n",rd);
	while(rd == -1)
	{
	rd = read(usbdev, response, 2);
	}
	printf("rd = %d\n",rd);
	printf("read : %s, %x\n %x, %x, %x, %x, %x\n",response,response,response[0],response[1],response[2],response[3],response[4]);
*/
/*

struct termios stdio;
        memset(&stdio,0,sizeof(stdio));
        stdio.c_iflag=0;
        stdio.c_oflag=0;
        stdio.c_cflag=0;
        stdio.c_lflag=0;
        stdio.c_cc[VMIN]=1;
        stdio.c_cc[VTIME]=0;
        tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
        tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
*/

//printf("baud_ = %s\n",baud_);



/*##########################################################
##########################################################*/
/* Inicializacion de la interfaz UART*/

	struct termios tio;
	memset(&tio, 0 , sizeof(tio));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;
	usbdev = open(device, O_RDWR|O_SYNC|O_NOCTTY| O_NONBLOCK);//| O_NONBLOCK |O_NDELAY  
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	fcntl(usbdev, F_SETFL, O_NONBLOCK);
	cfsetospeed(&tio, baud_);
	cfsetispeed(&tio, baud_);
	int setattr = tcsetattr(usbdev, TCSANOW, &tio);

	if(setattr == -1)
	{
		printf("Error: Error in tcsetattr\n");	
		exit(EXIT_SUCCESS);
		
	}



//speed_t in = cfgetispeed(&tio);
//speed_t out = cfgetospeed(&tio);

//printf("ispeed = %x\n ospeed = %d\n", in, out);

//int contador_read=0;

//while(1==1)
//{
//rd=0;
//	memset(&response,0,sizeof(response));//response = {0,0,0,0,0};
/*
	printf("esperando...\n");

	command[0] = 0x6c;
//	printf("rd = %d\n",rd);
	while(rd<1)
	{
		rd = read(usbdev, response, 1);
	}
//	contador_read += 1;
	printf("rd = %d\n",rd);

//	printf("rd = %d, wr = %d\n contador_read= %d\n  ",rd, wr, contador_read);

	printf("read : %s, %x\n",response,response[0]);
//}
	wr = write(usbdev, command, 1);
	if(wr == -1)
	{printf("error en escritura");}

	printf("write %s\n", command);

	rd=0;
memset(&response,0,sizeof(response));
	while(rd < 1)
	{
	rd = read(usbdev, response, 1);
//
	}

	printf("read : %s, %x\n %x, %x, %x, %x, %x\n",response,response,response[0],response[1],response[2],response[3],response[4]);
*/



/* Inicializar dispositivo*/

	ready = '0';
	validations = 0;
	while(ready == '0')
	{
		//Enviar comando de inicio
		printf("...............\nSe escribe '>' en el uart\n...............\n");
//		command[0] = 0x3e;

//		ptr_command = command;
		write_data(usbdev, 0x3e);
//		delay(wait_time);
		//printf("\nInicia la lectura.....\n");	
		
//command[0] = 0x3e;
//write(usbdev, command, 1);
/*
	rd = 0;
	int cont = 0;
	while(rd<1)
	{

		rd = read(usbdev, response, 1);
		cont = cont+1;
	}
	printf("read : %s, %x, cont = %i \n",response,response[0], cont);
*/
/*
	struct timeval tv;
	struct timezone tz;	
	unsigned long int time, start,end;

	start = tv.tv_usec;

		delay(wait_time);

	gettimeofday(&tv, &tz);
	end = tv.tv_usec;


	printf("start = %u, end = %u\n",start, end);
*/
	//	exit(EXIT_SUCCESS);


		response[0] = read_data_d(usbdev, wait_time, n_wait_states);
		//printf("response %s, %x\n", response, response[0]);
		switch(response[0])
		{	
			case 0x40 :	// @
				ready = '1';
				break;
			case 0x3c :	// <
				ready = '1';
				break;
			default :
				ready = '0';
				write_data(usbdev, 0x7e);
				validations = validations+1;
				if(validations == n_wait_states)
				{
					printf("Error: Constant bad response from the device\n");
					exit(EXIT_SUCCESS);
				}
				break;

		}
	
	}



	//printf("fin, \nresponse = %s\n",response);


	if(process == 'v')
	{
		process_ = 'l';
	}
	else{
		process_ = process;
	}


	printf("escribiendo %c\n", process_);
	write_data(usbdev, process_);

	// inicializar variables
	//printf("inicializa variables \n");
	sys_addr 	= 0x0;
	prog_addr	= address;
	char *ptr_data_array;
	char data_array[3];
/*
	char var = 0x41;
	
	char pru;	

	int pru_hex_b = 0x11121314;
	int i;

	ptr_data_array =  data_array;
	
	for(i = 0; i<4; i++)
	{
		pru = pru_hex_b;	
		printf("pru = %x\n",pru);
		pru_hex_b = pru_hex_b >> 8;
		*ptr_data_array = pru;
		ptr_data_array ++;

	}
	
	printf("data_array = %x,%x,%x,%x\n",data_array[0],data_array[1],data_array[2],data_array[3]);




	pru_hex_b = 0x01020304;


	ptr_data_array =  data_array;

	for(i = 0; i<4; i++)
	{
		pru = pru_hex_b;	
		printf("pru = %x\n",pru);
		pru_hex_b = pru_hex_b/16/16;
		*ptr_data_array = pru;
		ptr_data_array ++;

	}

//	char *arry = new char[3];
//	arry[0] = 0x3c;
//	arry = pru_hex_b;

//	printf("arry[0] = %c\n",arry[0]);
//	printf("hexa = %x\ndecimal = %d\nstring = %c\n", pru_hex, pru_hex, pru);

	printf("data_array = %x,%x,%x,%x\n",data_array[0],data_array[1],data_array[2],data_array[3]);

	int *ptr_prog_addr = &prog_addr;

	prog_addr = 0x40414243;






	printf("data_array = %x,%x,%x,%x\n",data_array[0],data_array[1],data_array[2],data_array[3]);
*/
/*
	ptr_data_array =  data_array;
	extract_bytes(prog_addr, ptr_data_array);
*/



	
//	printf("char[1] = %s",pru[0]);

	/*verificar que el systema este listo para aceptar comandos*/
//	response[0] = read_data_d(usbdev, wait_time, n_wait_states);



	/*decidir que proceso realizar*/
	
	//printf("decide que proceso realizar\n");




	struct timeval start, end, clock_;
	long int time = 0;
	gettimeofday(&start,NULL);








	switch(process)
	{
		case 'l':
			printf("Reconoce proceso de lectura\n");
			
			//printf("a\n");
			read_instructions(usbdev, address, sys_addr, prog_addr, data_width, long_read, ptr_data_array, wait_time, n_wait_states);
			//printf("b\n");
			break;
		case 'W':

	if (file == NULL)
	{
		printf("Error: The file was not specified\n");
		exit(EXIT_SUCCESS);
	}



////////////////////////////////////////////////////////////////////////
/*
file = fopen("flash.bin", w);
	if(!file)
	{
		printf("Error opening file %s", file);
		return 1;
	}
file_length = 1000000;
	buffer=(char *)malloc(file_len+1);
fwite()
*/
/////////////////////////////////////////////////////////////////////////



	fp = fopen(file, mode);
	if(!fp)
	{
		printf("Error opening file %s", file);
		return 1;
	}
	// file length
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

//	printf("b\n");
//	fprintf(fp,"opening..\n");

//	struct rec instr;
//	fread(&instr, sizeof(struct rec),1,fp);  //sizeof(instr)/sizeof(instr[0])


	//Allocate memory
	buffer=(char *)malloc(file_len+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(fp);
		return;
	}	


	fread(buffer, file_len, 1, fp);//sizeof(instr)/sizeof(instr[0])

	//printf("file: %p\n",fp);
/*
*/
	fclose(fp);


///////////////////////////////////////////////////////////

/* Extraer informacion del archivo  --- solo si es tipo elf??*/

//Extraer magic number:
k = 0x0;
char mag[4] = {buffer[k],buffer[k+1], buffer[k+2],buffer[k+2]}	;

// extract file class
k = 0x4;
char class = buffer[k];

// extract data encoding
k = 0x5;
char data = buffer[k];


// extraer id
k = 0x10;
char id[2] = {buffer[k],buffer[k+1]};


// extract architexture
k = 0x12;
char machine[2] = {buffer[k], buffer[k+1]};


// extract entry point
k = 0x14;
char entry[4] = {buffer[k], buffer[k+1], buffer[k+2], buffer[k+3]};


// extract program header table's offset
k = 0x1c;
char phoff[4] = {buffer[k], buffer[k+1], buffer[k+2], buffer[k+3]};


// extract program section table's offset
k = 0x20;
char shoff[4] = {buffer[k], buffer[k+1], buffer[k+2], buffer[k+3]};

// elf header size
k = 0x28;
char ehsize[2] = {buffer[k], buffer[k+1]};



// size of one entry in the file program header table
k = 0x2a;
char phentsize[2] = {buffer[k], buffer[k+1]};

// number of entries in the program header table
k = 0x2c;
char phnum[2] = {buffer[k], buffer[k+1]};


// size of one entry in the file section header table
k = 0x2e;
char shentsize[2] = {buffer[k], buffer[k+1]};

// number of entries in the program header table
k = 0x30;
char shnum[2] = {buffer[k], buffer[k+1]};




	k = offset;
		



	printf("Reconoce proceso de escritura\ntamaño del archivo = %d bytes\n",file_len);

		response[0] = read_data_d(usbdev, wait_time, n_wait_states);

		switch(response[0])
		{
			case 0x3c:
				printf("Error: Fail sending process id\n");
				exit(EXIT_SUCCESS);
				break;
			case 0x24:
				break;
			default:
				printf("Error: Fail sending process id\n");
				exit(EXIT_SUCCESS);
				break;	
		}
	printf("\n");
	while(k < file_len)//(segment_size+offset)
	{
	
	for(j = 0; j < data_width; j ++)
	{
	//	printf("%02x",((char *)buffer)[k+j]);
		data_array[data_width-1-(j)] = ((char *)buffer)[k+j];
	}
	k = k + data_width;
	ptr_data_array =  data_array;
	write_instructions(usbdev, address, sys_addr, prog_addr, data_width, ptr_data_array, wait_time, n_wait_states);
	prog_addr = prog_addr+data_width;
	sys_addr = prog_addr;
	//printf("Completed: %d \ŗ",((k*100)/file_len));
	}
	
		free(buffer);

			//	exit(EXIT_SUCCESS);


			break;
		case 'e':
			ptr_data_array =  data_array;
			printf("Reconoce proceso de borrado\n");
			erase_instructions(usbdev, address, sys_addr, prog_addr, wait_time, n_wait_states, data_width, ptr_data_array, block_size, long_erase);
			break;

		case 'v': 
	printf("Reconoce proceso de verificacion\n");

		if (file == NULL)
	{
		printf("Error: The file was not specified\n");
		exit(EXIT_SUCCESS);
	}		




// iniciar proceso de lectura:
		verify_instructions(usbdev, address, sys_addr, prog_addr, data_width, long_read, ptr_data_array, wait_time, n_wait_states, offset, file, mode, blank_check, log);


			break;


/////////////////////////////////////////////////////////////////////////////////////////
		default:
			printf("Error: Invalid process: %s\n", process);
			exit(EXIT_SUCCESS);
			break;
	}



		gettimeofday(&end,NULL);
		time = end.tv_sec-start.tv_sec;
		printf("tiempo empleado: %d segundos\n", time);
		printf("tiempo empleado: %d segundos\nend = %d, start = %d\n", time, end.tv_sec, start.tv_sec);	

	close(usbdev);
	return 0;
}

