#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 256
#define MAX 100
char user[MAX][INET_ADDRSTRLEN];


int sock, port;
int clen;     
struct sockaddr_in serv_addr, cli_addr_global;
int count_of_user = 0;
pthread_mutex_t mutex;

struct str_thread {
    struct sockaddr_in x;
    int y;
}; //Тут стоит точка с запятой!


int64_t getFileSize(const char* file_name){

//pthread_mutex_lock(&mutex);
	int64_t _file_size = 0;
	FILE* fd = fopen(file_name, "rb");
	if(fd == NULL){
		_file_size = -1;
	}
	else{
		fseek(fd, 0, SEEK_END);
		_file_size = ftello(fd);
		fclose(fd);
	}
	
//pthread_mutex_unlock(&mutex);	
	return _file_size;
}

int write_to_file (char* str)
{

  pthread_mutex_lock(&mutex);


   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
   printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen ("user_list.txt","a");

   // Проверка открытия файла
   if (mf == NULL) printf ("ошибка\n");
   else printf ("выполнено\n");
 
   //Запись данных в файл
   fprintf (mf,str);
   printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   
   
  pthread_mutex_unlock(&mutex);
   
   printf ("Файл закрыт\n");
   
   return 0;
}






int unsigned_char_array_4_to_int (unsigned char* uc_int)
{

	unsigned int I;
	I = uc_int[3];
	I <<= 8;
	I |= uc_int[2];
	I <<= 8;
	I |= uc_int[1];
	I <<= 8;
	I |= uc_int[0];
	
	int int_ret = (int*)I;
	return int_ret;

}


void int_to_unsigned_char_array_4 (unsigned int digit, unsigned char* uc_int)
{

	uc_int[0] = digit & 0xFF;
	uc_int[1]=(digit >> 8) & 0xFF;
	uc_int[2]=(digit >> 16) & 0xFF;
	uc_int[4]=(digit >> 24) & 0xFF;
	
	return;
	
}





void * thread_func2(void * arg)
{
	while(1)
		{

		/*
			

			sleep(600);
			pthread_mutex_lock(&mutex);
			printf("%s\n", "mutex LOCK thread_func2");
			
			
			
			
			
			
			
			// Вот тут мне кажется что есть ошибка
			
			
			
			if(remove("user_list.txt")) 
				{
                			printf("Error removing 'user_list.txt'\n");
              				//  return;
                		}
       		// Очищаем список в памяти
       		int j=0;
       		for(j;j<count_of_user+1;j++)
       			{
       				strncpy(user[j], "_",1);
       			}
       		count_of_user = 0;
       		
       		
       		
       		
       		
       		
       		
       		
       		
       		
       		printf("%s","Файл со списком пользователей удален.\n"); 
       		 pthread_mutex_unlock(&mutex);      
       		printf("%s\n", "mutex UN_LOCK thread_func2"); 
		*/


       	}        
	return;
}



void * thread_func(void * arg)
{

	//pthread_mutex_lock(&mutex);
	//printf("%s\n", "mutex LOCK thread_func"); 
     char buf[BUF_SIZE];
     struct sockaddr_in cli_addr;     
     struct str_thread structure;
     structure = *(struct str_thread*) arg;   
     cli_addr = structure.x;
     
     char str[INET_ADDRSTRLEN];
     memset(str,0,INET_ADDRSTRLEN);
    // Where we sent hello-message ?
    inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     printf("Передано в поток IP-адрес клиента = %s|\n",str);
     
     int newsock = structure.y;
     memset(buf, 0, BUF_SIZE);
     
     
     
     
     unsigned char buf_int [4];
     int total_recieve = 0;
     memset(buf_int,0,4);
     int real_read_from_newsock = 0;
     
     
	while (total_recieve != 4) // 4 байт 
		{
			real_read_from_newsock = 0;
			real_read_from_newsock = read(newsock, &buf_int[total_recieve], 4);
			if (real_read_from_newsock <= 0)
				{
					char str[INET_ADDRSTRLEN];
					memset(str,0,INET_ADDRSTRLEN);
     					// Where we sent hello-message ?
    					inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     					printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     					printf("%s%s%s\n","[thread_func]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    					close(newsock);
    					return;
				}
			total_recieve = total_recieve + real_read_from_newsock;		
		}   
 	
     	
     	
     	
     	
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- BEGIN --------------------    
    
    // Тут должно прийти 4 байта
    
    int wait_bayte = unsigned_char_array_4_to_int(buf_int);
    
    printf("Принял от клиента wait_bayte = %d|\n", wait_bayte);
    
    total_recieve = 0;
    
    while(total_recieve != wait_bayte)
    	{
    		real_read_from_newsock = 0;
    		memset(buf, 0, BUF_SIZE);
    		real_read_from_newsock = read (newsock, &buf[total_recieve], BUF_SIZE-1);
    		if (real_read_from_newsock <=0)
    			{
    				char str[INET_ADDRSTRLEN];
    				memset(str,0,INET_ADDRSTRLEN);
     				// Where we sent hello-message ?
    				inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     				printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     				printf("%s%s%s\n","[thread_func_send_message_server_part]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    				close(newsock);
    				return;
    
   			}
   		
   		total_recieve = total_recieve + real_read_from_newsock;	
    	}
    
    
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- END --------------------
     
     //read(newsock, buf, BUF_SIZE-1);
     buf[BUF_SIZE] = 0;
     printf("MSG: %s|\n", buf);
    
     // Если пришло сообщение-приветсвие (пользователь извещает о том, что он доступен)
     if (strcmp(buf,"hi\n") == 0)
     	{ 
     	
     	
     	//	pthread_mutex_lock(&mutex);
     	//	printf("%s ", "LOCK_HI");
     	
     		// INET_ADDRSTRLEN - это константа одного из заголовочных файлов
		char str[INET_ADDRSTRLEN];
		memset(str,0,INET_ADDRSTRLEN);
     		// Where we sent hello-message ?
    		 inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
    		
    		
    		 
    		// Добавление BEGIN -----------------------
     		int len_buf = INET_ADDRSTRLEN;
     		unsigned char uc_int [4];
     		memset(uc_int,0,4);
     		int_to_unsigned_char_array_4 (len_buf, uc_int);
     		write(newsock, uc_int, 4);
     		// Добавление END ------------------------- 
    		      	
     		write(newsock, str,INET_ADDRSTRLEN);
     		
     		printf("%s%s, port=%d\n","Hwo sent us the hello-message: ", str, ntohs(cli_addr.sin_port) );
     		close(newsock);
   		// Проверяем, есть ли в нашем списке такой IP
   		int i = 0;
   		printf("%s%d\n","count_of_user = ", count_of_user);
   		int flag = 1; // 0 - добавляются пользователи, 1 - не добавляются (тогда непобходимо убрать в начале main добавление фиксированного списка)
   		for (i; i < count_of_user+1; i++)
   			{
   				printf("user[%d] = |%s| ... str = |%s|\n",i,user[i], str);
   				// Если есть - игнорируем, если нет - добавляем, а после освобождаем ресур (файл, или список)
   				if(strcmp(user[i],str) == 0) 
   				{
   					printf("%s","Такой пользователь уже есть.\n");
   					flag  = 1;
   					break;
   					//return;
   					}
   			}
   		if (flag == 0)
   			{
   				strncpy (user[count_of_user], str,INET_ADDRSTRLEN);
   				
   				
   				
   				/*
   				
   				int res0  = write_to_file(str);
   				int res1  = write_to_file(":");
   				int x = ntohs(cli_addr.sin_port);
   				int length = snprintf( NULL, 0, "%d", x );
   				
   				char client_port [length + 1];
   				snprintf(client_port, length+1, "%d", x );
   				client_port[length] = '\0';
   				int res2  = write_to_file(client_port);
   				int res3  = write_to_file("\n");
   				*/
   				int res0  = write_to_file(str);
   				int res1  = write_to_file("\n");
   				
   				//if ((res0 == 0) && (res1 == 0) && (res2 == 0) && (res3 == 0))
   				if ((res0 == 0) && (res1 == 0))
   					{
   						printf("%s","Добавление записи в файл прошло успешно.\n");
   					}
   				else
   					{
   						printf("%s","Не удалось добавить запись в файл.\n"); 			
   					}
     				count_of_user++;
   			}

    		printf("%s","Список доступных польователей: \n");
       	int j=0;
     		for (j;j<count_of_user;j++)
     			{
     				printf("%d - %s\n",j,user[j]);
     			}
     			
     	//	pthread_mutex_unlock(&mutex);	
     	//	printf("%s ", "UN_LOCK_HI");
     		
  	}
  
  // Если пришло сообщение с запросом списка доступных пользователей
  else if (strcmp(buf,"list\n") == 0)
  	{	
  	
  		pthread_mutex_lock(&mutex);
     		printf("%s ", "LOCK_LIST");
  	
  		// Узнаем размер файла
  		long size;
        	size = getFileSize("user_list.txt");
        	printf("Размер файла user_list.txt равен %d\n", size);
        	
  		// Считаем его в массив байт
  		FILE* file;
  		file=fopen ("user_list.txt","rb");
  		if (file != NULL)
  			{
  				char byte_arr[size]; 
  				int real_read_size = fread(byte_arr, 1, size, file);
  				if (real_read_size != size)
  					{
  						printf("%s","Считывание из файла завершилось неудачно!\n");
  					}
  				else 
  					{
  						printf("%s","Считывание из файла завершилось удачно!\n");
  					}
  	
  				fclose(file);
  				
  				// Добавление BEGIN -----------------------
     				int len_buf = real_read_size;
     				unsigned char uc_int [4];
     				memset(uc_int,0,4);
     				int_to_unsigned_char_array_4 (len_buf, uc_int);
     				write(newsock, uc_int, 4);
     				// Добавление END -------------------------
  				
  				write(newsock, byte_arr, real_read_size);
  			}
  		else
  			{
  				printf("%s","Невозможно открыть файл для передачи на запрос клиента.\n");
  				
  				const char* ip_string = "127.0.0.1";
  				// Добавление BEGIN -----------------------
     				int len_buf = strlen(ip_string);
     				unsigned char uc_int [4];
     				memset(uc_int,0,4);
     				int_to_unsigned_char_array_4 (len_buf, uc_int);
     				write(newsock, uc_int, 4);
     				// Добавление END -------------------------
  				
  				
  				write(newsock, ip_string, strlen(ip_string));
  			}
  		close(newsock);
  		
  	     	pthread_mutex_unlock(&mutex);
     		printf("%s ", "UN_LOCK_LIST");	
  		
  	}

  // pthread_mutex_unlock(&mutex);
  //printf("%s\n", "mutex UNLOCK thread_func");
  printf("%s","Очередной клиент обработан\n");   
}






int main(int argc, char ** argv)
{

     if (argc < 2) 
    	{
         	fprintf(stderr,"usage: %s <port_number>\n", argv[0]);
         	return EXIT_FAILURE;
     	}
     	
memset(user[0],0,INET_ADDRSTRLEN);
memset(user[1],0,INET_ADDRSTRLEN);
memset(user[2],0,INET_ADDRSTRLEN);
memset(user[3],0,INET_ADDRSTRLEN);
memset(user[4],0,INET_ADDRSTRLEN);
memset(user[5],0,INET_ADDRSTRLEN);     	
strncpy(user[0], "192.168.0.1",11);
strncpy(user[1], "192.168.0.2",11);
strncpy(user[2], "192.168.0.3",11);
strncpy(user[3], "192.168.0.4",11);
strncpy(user[4], "192.168.0.5",11);
strncpy(user[5], "192.168.0.6",11);     	

     	
     	
	          // LINUX
	struct timeval tv;
	tv.tv_sec = 15; //timeout_in_seconds;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
     	
     sock = socket(AF_INET, SOCK_STREAM, 0);
     if (socket < 0)
     	{
       	printf("socket() failed: %d\n", errno);
       	return EXIT_FAILURE;
     	}
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     port = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(port);
     


     
     if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     	{
       	printf("bind() failed: %d\n", errno);
       	return EXIT_FAILURE;
     	}
     listen(sock, 1);
     clen = sizeof(cli_addr_global);
     
     int i=0;
     
     // 04-09-21 -- BEGIN (/**/) ---
     
     //for(i;i<MAX;i++)
     //	strncpy (user[i], "_",1);
     
     // 04-09-21 -- END ---
     
     
     // Поток отвечающий за периодическое удаление файла с пользователями
     pthread_t thread2;
     int id2 = 1;
     int result_2 = pthread_create(&thread2, NULL, thread_func2, &id2);
     if (result_2 != 0) 
     	{
     		perror("Creating the second thread");
     		return EXIT_FAILURE;
   	}
     
     while(1)
     	{
        	int id, result;
   		pthread_t thread1;
   		id = 1;
   	
     		int newsock = accept(sock, (struct sockaddr *) &cli_addr_global, &clen);
     		if (newsock < 0) 
     			{
       			printf("accept() failed: %d\n", errno);
       			return EXIT_FAILURE;
     			}

   		struct str_thread struct1;
     		struct1.x = cli_addr_global;
     		struct1.y=newsock;
   	
   		char str[INET_ADDRSTRLEN];
		memset(str,0,INET_ADDRSTRLEN);
     		// Where we sent hello-message ?
    		 inet_ntop(AF_INET, &(cli_addr_global.sin_addr), str, INET_ADDRSTRLEN);
    		 
    		printf("Подключился клиент с IP = %s|\n", str); 
   	
   		result = pthread_create(&thread1, NULL, thread_func, &struct1);
   		if (result != 0) 
   			{
     				perror("Creating the first thread");
     				return EXIT_FAILURE;
   			}
   	
     	}
     
     close(sock);
}



