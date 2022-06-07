#include "C_part.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t mutex1;
extern const char* PATH_Date_Base;
extern int port2;
extern int start_line;
extern const char* LOG_FILE_NAME;
char my_global_ip[BUF_SIZE];
extern const char* my_port_send_message;
extern const char* my_port_hash_blockain;
extern const char* use_static_tunnels;
extern int sending_file_flag;
int periodically_send_hello_message = 0;
//int periodically_send_hello_message = 1;



#if defined(__clang__) || defined (__GNUC__)
# define ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else
# define ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

int cmp(const void *a, const void *b) {
     return *(int*)a - *(int*)b;
 }

int write_to_file_c (char* buf, int size)
{

write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"write_to_file_c]\"\n");
pthread_mutex_lock(&mutex);


   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
  // printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen ("actual_blockchain.txt","a");

   // Проверка открытия файла
   if (mf == NULL) 
   {
   	printf ("ошибка\n");
   	exit(1);
   }
   else
   { 
   	//printf ("выполнено\n");
 	}
   //Запись данных в файл
   fwrite(buf, 1, size, mf);
  // printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   
   
pthread_mutex_unlock(&mutex);
   
  // printf ("Файл закрыт\n");
   
   return 0;
}





int write_to_log_file (const char* filename, const char* message)
{

 //pthread_mutex_lock(&mutex);


   int size = strlen(message);

   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
 //  printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"a");

   // Проверка открытия файла
   if (mf == NULL)
   { 
   	 printf ("ошибка\n");
   	 exit(1);
   }
   else 
   {
   	// printf ("выполнено\n");
   }
   
 
   //Запись данных в файл
   fwrite(message, 1, size, mf);
   //printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   
   
 //pthread_mutex_unlock(&mutex);
   
  // printf ("Файл закрыт\n");
   
   return 0;
}








int write_to_file_name_c (char* buf, int size, const char* filename)
{
write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"write_to_file_name_c]\"\n");
pthread_mutex_lock(&mutex);


   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
 //  printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"a");

   // Проверка открытия файла
   if (mf == NULL)
   { 
   	 printf ("ошибка\n");
   	 exit(1);
   }
   else 
   {
   	// printf ("выполнено\n");
   }
   
 
   //Запись данных в файл
   fwrite(buf, 1, size, mf);
   //printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   
   
pthread_mutex_unlock(&mutex);
   
  // printf ("Файл закрыт\n");
   
   return 0;
}


int read_from_file_to_buffer(const char* filename, char* buffer)
      {
      
      //pthread_mutex_lock(&mutex);
        write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"read_from_file_to_buffer]\"\n");
      
        FILE* fd;
        int  numread, count;
        int flag_opened = 0;
        while (flag_opened != 1)
        {
          pthread_mutex_lock(&mutex);
          if (( fd = fopen(filename, "rb")) == NULL)
          {
            perror("open failed on file file.dat");
            printf("open failed on file file.dat: '%s'.\n", filename);
            pthread_mutex_unlock(&mutex);
            sleep(2);
            continue;
            //exit(1);
          }
          flag_opened = 1;
          
        }
        
        for (count = 0; count < MAX_TRANSACTION_LENGTH; count++)
             buffer[count]='\0';
             
        numread = fread(buffer, 1, MAX_TRANSACTION_LENGTH, fd);
        //printf("/n Число считанных литер %d\n",numread);
        fclose(fd);
	pthread_mutex_unlock(&mutex);
        return numread;
      }


void create_empty_file(const char* filename)
{
	
	 write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"create_empty_file]\"\n");

	// Создаем файл-болванку

   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
   //printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"w");

   // Проверка открытия файла
   if (mf == NULL) 
   {
   	printf ("ошибка\n");
   	exit(1);
   }
   else 
   {
   	//printf ("выполнено\n");
   }
   
 
   //Запись данных в файл
   fprintf (mf,"Тест записи в файл");
   //printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   //printf ("Файл закрыт\n");
   return;
}




// Функция для вызова скриптов и других программ в Linux из C-кода

void call_Linux_programm(const char* programm_name, char* res_buf)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"call_Linux_programm]\"\n");

  create_empty_file("test.txt");

  FILE * uname;
  char result[80];
  int lastchar = 79;

  uname = popen(programm_name, "r");
  lastchar = fread(result, 1, 80, uname);
  result[lastchar] = '\0';
  //printf("RESULT is %s\n", result);
  pclose(uname);
  
  
  // Удаляем созданный файл
    if(remove("test.txt")) {
                printf("Error removing test.txt");
                exit(1);
                }
  //strncpy(res_buf,result,lastchar+1);
  return;	

}



int64_t getFileSize(const char* file_name){


write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"getFileSize]\"\n");

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


int blockchain_from_DB_to_file_c (const char* path_to_DB, int from_id, const char* path_to_buffer_file, char* buffer)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"blockchain_from_DB_to_file_c]\"\n");

	//printf("%s\n","check_1");

	// Запрашиваем последний id
	char last_id_c[BUF_SIZE];
	memset(last_id_c, 0, BUF_SIZE);
    	get_how_much_id (path_to_DB, last_id_c);
	
	int last_id = atoi((const char*) last_id_c);	
	int i = from_id;
	
	for (i; i <= last_id; i++)
		{
		
			// Запрашиваем TRANSACTION_BODY с ID == i
				
			char transaction_body_c[MAX_TRANSACTION_LENGTH];
			memset(transaction_body_c, 0, MAX_TRANSACTION_LENGTH);
			int length = snprintf( NULL, 0, "%d", i);
    			char id_str [length + 1];
    			snprintf(id_str, length + 1, "%d", i);
    			id_str[length] = '\0';
	 		const char* id_str_const = id_str;	
	 		
	 		//printf("id_str_const = %s\n",id_str_const);
						
			int res3 =  get_transaction_body_where_id_c(path_to_DB, id_str_const, transaction_body_c);
			if (res3 != 0)
				{
				
					printf("%s%d%s\n","При получении get_transaction_body_where_id_c произошла ошибка.(",i,")");
				 	exit(1);
				}
			else
				{
				
					int size_of_transaction = strlen(transaction_body_c);

					const char* buf1 = transaction_body_c;

					char buf_buf [size_of_transaction+2];
					memset(buf_buf,0,size_of_transaction+2);
					
					int l = 0;
					for (l;l<size_of_transaction+1;l++)
						{
						
							if (buf1[l] != '\0')
								buf_buf[l] = buf1[l];
							else 
								break;	
						
						}
					buf_buf[l] = '~';
					buf_buf[l+1] = '\0';	
					
					int new_size_of_transaction = l+1;
					
					const char* buf = buf_buf;
				
					int res_write = write_to_file_name_c (buf, new_size_of_transaction, path_to_buffer_file);
						if (res_write != 0)
							{
								printf("%s%d%s\n","При выполнении write_to_file_c-функции произошла ошибка.(",i,")");
								exit(1);
							}
						else
							{
							//	printf("%s%d%s\n","При выполнении write_to_file_c-функции ошибки не произошло.(",i,")");
							}
							
					//printf("%s%d%s%s|\n","transaction_body(",i,") = ",buf);	
					//printf("%s\n","check_10000");		
					
				}		
			
		}
	
	
	int size = read_from_file_to_buffer(path_to_buffer_file, buffer);
	
	//printf("%s\n","check_200000");

	// Удаление временного файла path_to_buffer_file

        //      if(remove(path_to_buffer_file)) {
        //       printf("Error removing path_to_buffer_file");
        //        exit(1);
        //        }
	
	//printf("%s\n","check_3000000");
	
	//printf("%s\n","Запись блокчейна в файл из БД успешно завершена.");
	return size;
}




int blockchain_from_DB_to_file_c_correct (const char* path_to_DB, int from_id, const char* path_to_buffer_file, char* buffer)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"blockchain_from_DB_to_file_c_correct]\"\n");
	//printf("%s\n","check_1");

	// Запрашиваем последний id
	
	char last_id_c[BUF_SIZE];
	memset(last_id_c, 0, BUF_SIZE);
    	get_how_much_id (path_to_DB, last_id_c);
	
	int last_id = atoi((const char*) last_id_c);	
	int i = from_id;

	for (i; i <= last_id; i++)
		{
		
			// Запрашиваем TRANSACTION_BODY с ID == i
				
			char transaction_body_c[MAX_TRANSACTION_LENGTH];
			memset(transaction_body_c, 0, MAX_TRANSACTION_LENGTH);

			int length = snprintf( NULL, 0, "%d", i);
    			char id_str [length + 1];
    			snprintf(id_str, length + 1, "%d", i);
    			id_str[length] = '\0';
	 		const char* id_str_const = id_str;	

	 		int length1 = snprintf( NULL, 0, "%d", i-1);
    			char id_str1 [length1 + 1];
    			snprintf(id_str1, length1 + 1, "%d", i-1);
    			id_str1[length1] = '\0';
	 		const char* id_str_const1 = id_str1;

	 		//printf("id_str_const = %s\n",id_str_const);
						
			int res3 =  get_transaction_body_where_id_c(path_to_DB, id_str_const, transaction_body_c);
			if (res3 != 0)
				{
				
					printf("%s%d%s\n","При получении get_transaction_body_where_id_c произошла ошибка.(",i,")");
				 	exit(1);
				}
			else
				{
				
					int size_of_transaction = strlen(transaction_body_c);
					//char* buf = transaction_body_c;
					// Запрашиваем hash_id_container
					// Функция получения хэша последнего id-контейнера
     					char hash_last_id_container[HEX_LENGTH];
     					memset(hash_last_id_container, 0, HEX_LENGTH);
     					get_hash_id_container (path_to_DB, id_str_const1, hash_last_id_container);

					//Складываем все вместе
					
					// Склеиваем 3 строки
    					int length_buf = strlen(id_str_const) + strlen(hash_last_id_container) + strlen(transaction_body_c) + 1 + 2; // +1 - это для '\0'-символа конца строки
    					char buf [length_buf];
    					memset(buf,0,length_buf);

    					snprintf(buf, sizeof buf, "%s;%s;%s", id_str_const, hash_last_id_container, transaction_body_c);
    
   					buf[length_buf] = '\0';
				
					int res_write = write_to_file_name_c (buf, length_buf, path_to_buffer_file);
						if (res_write != 0)
							{
								printf("%s%d%s\n","При выполнении write_to_file_c-функции произошла ошибка.(",i,")");
								exit(1);
							}
						else
							{
							//	printf("%s%d%s\n","При выполнении write_to_file_c-функции ошибки не произошло.(",i,")");
							}
							
				//	printf("%s%d%s%s\n","id_hash_before_transaction_body(",i,") = ",buf);	
				//	printf("%s\n","check_10001");		
					
				}		
			
		}
	
	sleep(1);
	int size = read_from_file_to_buffer(path_to_buffer_file, buffer);
	
	// printf("%s\n","check_2000012");
	
	
	// Удаление временного файла path_to_buffer_file

        //      if(remove(path_to_buffer_file)) {
        //       printf("Error removing path_to_buffer_file");
        //        exit(1);
        //        }
	
	// printf("%s\n","check_3000003");
	
	// printf("%s\n","Запись блокчейна в файл из БД успешно завершена.");
	return size;
}

static int callback_select_last_id(void *data, int argc, char **argv, char **azColName){
	
	
   write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"callback_select_last_id]\"\n");
   //int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   char* last_id_str = ( char*) data;
   
   int i = 0;
   while (argv[0][i] != '\0')
   	{
   		last_id_str[i] = argv[0][i];
   		i++;
   	}
   
   last_id_str[i] = '\0';
	
   // printf("last_id_str = %s|\n", last_id_str);	   

  // printf("\n");
   return 0;
}


static int callback_select_last_id_hash(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"callback_select_last_id_hash]\"\n");
   //int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   char* hash_last_id_container = (char*) data;
   int i = 0;
   while (argv[0][i] != '\0')
   	{
   		hash_last_id_container[i] = argv[0][i];
   		i++;
   	}
   
   hash_last_id_container[i] = '\0';
	
   // printf("Last_hash_id_container = %s|\n", hash_last_id_container);	   

   // printf("\n");
   return 0;
}


static int callback_select_transaction_body_where_id(void *data, int argc, char **argv, char **azColName){
   write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"callback_select_transaction_body_where_id]\"\n");
   //int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   char* transaction_body_id = (char*) data;
   int i = 0;
   while (argv[0][i] != '\0')
   	{
   		transaction_body_id[i] = argv[0][i];
   		i++;
   	}
   
   transaction_body_id[i] = '\0';
	
   // printf("transaction_body_id = %s|\n", transaction_body_id);	   

   // printf("\n");
   return 0;
}

int get_transaction_body_where_id_c(const char* path_to_DB, const char* id_str, char* transaction_body_id)
{
   write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"get_transaction_body_where_id_c]\"\n");
   // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
               // fprintf(stdout, "\nOpened database successfully\n");
            }


    const char* str1 = "SELECT TRANSACTION_BODY from BLOCKCHAIN_TABLE WHERE TRANSACTION_ID = ";
    
    
    /*
    int length = snprintf( NULL, 0, "%d", last_id);
    const char* last_id_str [length + 1];
    snprintf(last_id_str, length + 1, "%d", last_id);
    last_id_str[length] = '\0';
    */


    // Склеиваем 2 строки
    int id_hash_length = strlen(str1) + strlen(id_str) +1 ; // +1 - это для '\0'-символа конца строки
    char id_hash [id_hash_length];

    snprintf(id_hash, sizeof id_hash, "%s%s", str1, id_str);
    
    id_hash[id_hash_length-1] = '\0';
    
    
   /* Create SQL statement */
   sql = id_hash;


	  //  printf("%s\n", "check_0");	

         //   printf("%s%s\n","Вывод строки из BLOCKCHAIN_TABLE, где ID = ",id_str);      


	    // Execute SQL statement 
	    pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_transaction_body_where_id, (void*)transaction_body_id, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
                   }

            sqlite3_close(db);
         //   printf("%s\n","Файл БД закрыт");

return 0;

}



void get_how_much_id (const char* path_to_DB, char* last_id)
{

    write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"get_how_much_id\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
          //      fprintf(stdout, "\nOpened database successfully\n");
            }

            sql = "SELECT COUNT(*) from BLOCKCHAIN_TABLE";

            // Execute SQL statement
            pthread_mutex_lock(&mutex); 
            rc = sqlite3_exec(db, sql, callback_select_last_id, (void*)last_id, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                exit(1);
            } else {
             //   fprintf(stdout, "Operation done successfully\n");
                   }

            sqlite3_close(db);
        //    printf("%s\n","Файл БД закрыт");

return;

}


void get_hash_id_container (const char* path_to_DB, const char* id_str, char* hash_id_container)
{

    write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"get_hash_id_container\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;


    
            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
        //        fprintf(stdout, "\nOpened database successfully\n");
            }

    const char* str1 = "SELECT TRANSACTION_HASH from BLOCKCHAIN_TABLE WHERE TRANSACTION_ID = ";
    
    
    /*
    int length = snprintf( NULL, 0, "%d", last_id);
    const char* last_id_str [length + 1];
    snprintf(last_id_str, length + 1, "%d", last_id);
    last_id_str[length] = '\0';
    */


    // Склеиваем 2 строки
    int id_hash_length = strlen(str1) + strlen(id_str) +1 ; // +1 - это для '\0'-символа конца строки
    char id_hash [id_hash_length];

    snprintf(id_hash, sizeof id_hash, "%s%s", str1, id_str);
    
    id_hash[id_hash_length-1] = '\0';

   /* Create SQL statement */
   sql = id_hash;


	//    printf("%s\n","check-1");

        //    printf("%s%s\n","Вывод строки из BLOCKCHAIN_TABLE, где ID = ",id_str);      

	    // Execute SQL statement 
	    pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_last_id_hash, (void*)hash_id_container, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                exit(1);
            } else {
           //     fprintf(stdout, "Operation done successfully\n");
                   }

            sqlite3_close(db);
        //    printf("%s\n","Файл БД закрыт");

return;

}



int send_message (const char* path_to_db, const char* encrypt_message, int port, const char* path_to_buf_file)
{  

write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"send_message\"]\n");

 		
int len = strlen(encrypt_message);
 		
 		char enc_mes[len+1];
 		
 		//printf("encrypt_message = %s| len = %d\n",encrypt_message,len);
 		
 		memset(enc_mes,0,len+1);
 		
 		int p = 0;
 		for (p;p<len;p++)
 			{
 			
 				if (encrypt_message[p] != '\0')
 					{
 						enc_mes[p] = encrypt_message[p];
 					}
 				else 
					{break;}	
 			
 			}
 		
 		enc_mes[p] = '\0';
 		
 		//strncpy(enc_mes,encrypt_message,len);
 		
 		//printf("enc_mes_BEFORE_BEFOREEEE = |%s|\n",enc_mes);

// SEND_MESSAGE ------------------
char IP[MAX][INET_ADDRSTRLEN];
char PORTS[MAX][MAX_PORT_ADDRESS_LENGTH];
char ANSWER[MAX][ANSWER_LENGTH];

int Port = port;
// SEND_MESSAGE ------------------

// Первоначальное заполнение массивов

   int i = 0;
   for (i;i<MAX;i++)
   	{
   		memset(IP[i], 0, INET_ADDRSTRLEN);
   		memset(PORTS[i], 0, MAX_PORT_ADDRESS_LENGTH);
   		memset(ANSWER[i], 0, ANSWER_LENGTH);
   	}

      // Переменная, в которую будет помещен уsend_messageказатель на созданный
   // поток данных 
   FILE *mf;
   // Переменная, в которую поочередно будут помещаться считываемые строки
   char str[50];

   //Указатель, в который будет помещен адрес массива, в который считана 
   // строка, или NULL если достигнут коней файла или произошла ошибка
   char *estr;

   // Открытие файла с режимом доступа «только чтение» и привязка к нему 
   // потока данных
  // printf ("Открытие файла: ");


	pthread_mutex_lock(&mutex1);
            
            

   mf = fopen ("user_list_client.txt","r");

   // Проверка открытия файла
   if (mf == NULL) {
   	//printf ("ошибка\n"); 
   	return -1;
   }
   else 
   
   {
   	//printf ("выполнено\n");
	}
  // printf ("Считаны строки:\n");


   int count_IP = 0;


   //Чтение (построчно) данных из файла в бесконечном цикле
   while (1)
   {
      // Чтение одной строки  из файла
      estr = fgets (str,sizeof(str),mf);

      //Проверка на конец файла или ошибку чтения
      if (estr == NULL)
      {
         // Проверяем, что именно произошло: кончился файл
         // или это ошибка чтения
         if ( feof (mf) != 0)
         {  
            //Если файл закончился, выводим сообщение о завершении 
            //чтения и выходим из бесконечного цикла
          //  printf ("\nЧтение файла закончено\n");
            break;
         }
         else
         {
            //Если при чтении произошла ошибка, выводим сообщение 
            //об ошибке и выходим из бесконечного цикла
        //    printf ("\nОшибка чтения из файла\n");
            break;
         }
      }
      //Если файл не закончился, и не было ошибки чтения 
      //выводим считанную строку  на экран
     // printf ("     str = %s|",str);
      
      // Записываем полученную строку в массив IP
      strncpy (IP[count_IP], str, strlen(str));
      int size_IP_str = strlen(str);
      IP[count_IP][size_IP_str] = '\0';
      //printf ("     IP  = %s|",IP[count_IP]);
    //  printf ("     IP  = %s|",IP[count_IP]);
      
      count_IP++;
      
      
   }

   // Закрываем файл
  // printf ("Закрытие файла: ");
   if ( fclose (mf) == EOF)
   {
   	printf ("ошибка\n");
   	exit(1);
   	}
    
   else 
   {
   
   	//printf ("выполнено\n");
   }
   
	pthread_mutex_unlock(&mutex1);








// =====================================================================

	pthread_mutex_lock(&mutex1);
            
            

   mf = fopen ("user_list_client_port_send_message.txt","r");

   // Проверка открытия файла
   if (mf == NULL) {
   	//printf ("ошибка\n"); 
   	return -1;
   }
   else 
   
   {
   	//printf ("выполнено\n");
	}
  // printf ("Считаны строки:\n");


   int count_PORTS = 0;


   //Чтение (построчно) данных из файла в бесконечном цикле
   while (1)
   {
      // Чтение одной строки  из файла
      estr = fgets (str,sizeof(str),mf);

      //Проверка на конец файла или ошибку чтения
      if (estr == NULL)
      {
         // Проверяем, что именно произошло: кончился файл
         // или это ошибка чтения
         if ( feof (mf) != 0)
         {  
            //Если файл закончился, выводим сообщение о завершении 
            //чтения и выходим из бесконечного цикла
          //  printf ("\nЧтение файла закончено\n");
            break;
         }
         else
         {
            //Если при чтении произошла ошибка, выводим сообщение 
            //об ошибке и выходим из бесконечного цикла
        //    printf ("\nОшибка чтения из файла\n");
            break;
         }
      }
      //Если файл не закончился, и не было ошибки чтения 
      //выводим считанную строку  на экран
     // printf ("     str = %s|",str);
      
      // Записываем полученную строку в массив IP
      strncpy (PORTS[count_PORTS], str, strlen(str));
      int size_PORT_str = strlen(str);
      PORTS[count_PORTS][size_PORT_str - 1] = '\0';
      PORTS[count_PORTS][size_PORT_str] = '\0';
      //printf ("     IP  = %s|",IP[count_IP]);
    //  printf ("     IP  = %s|",IP[count_IP]);
      
      count_PORTS++;
      
      
   }

   // Закрываем файл
  // printf ("Закрытие файла: ");
   if ( fclose (mf) == EOF)
   {
   	printf ("ошибка\n");
   	exit(1);
   	}
    
   else 
   {
   
   	//printf ("выполнено\n");
   }
   
	pthread_mutex_unlock(&mutex1);


// =====================================================================



















 //printf("count_IP = %d\n", count_IP);

    		// Запрос последнего id в моей локальной копии blockchain (отдельная прога будет возвращать из БД)
     
     		char last_id_c[10]; //BUF_SIZE
     		memset(last_id_c, 0, 10); // BUF_SIZE
     		get_how_much_id (path_to_db,last_id_c);
   		int current_last_id = atoi((const char*)last_id_c);
   		//printf("last_id = %d|\n", current_last_id);
   		int new_last_id = current_last_id + 1;

   		int length = snprintf( NULL, 0, "%d", new_last_id );
   		//printf("length = %d\n", length);
   		char last_id_str [length + 1];
   		snprintf(last_id_str, length + 1, "%d", new_last_id );
   		last_id_str[length] ='\0';
   		const char* last_id_str_const = (const char*)last_id_str;
   		   		   		
   		//printf("%s|\n",last_id_str_const);
   		
   		int length1 = snprintf( NULL, 0, "%d", current_last_id );
   		//printf("length = %d\n", length1);
   		char last_id_str1 [length1 + 1];
   		snprintf(last_id_str1, length + 1, "%d", current_last_id );
   		last_id_str1[length1] ='\0';
   		const char* last_id_str_const1 = (const char*)last_id_str1;
   		
   		// Запрос хэша последнего id-контейнера
   		
   		// Функция получения хэша последнего id-контейнера
     		char last_id_container_hash[HEX_LENGTH + 1];
     		memset(last_id_container_hash, 0, HEX_LENGTH + 1);
     		get_hash_id_container (path_to_db, last_id_str_const1, last_id_container_hash);
 
 		last_id_str[length] =';';
 		// Но когда будем работать с БД, то вот так нужно будет написать
 		
 		last_id_container_hash[HEX_LENGTH] = '\0';
 		
 		// В эту переменную помещаем зашифрованный блок (пока что так)
 		
 		
 		//printf(" enc_mes = %s|\n", enc_mes);
 
   int j = 0;
   int count_;
   
   if (strcmp(use_static_tunnels,"TRUE") == 0)
   	count_ = count_PORTS;
   else
   	count_ = count_IP;
   
   for (j; j < count_; j++)
   	{
   	
   		//printf(" enc_mes_in cicl = %s|\n", enc_mes);
   	
   		int strlen_1 = strlen(enc_mes) + 1; // без +1
   		
   		//printf("strlen_1 =%d", strlen_1);
   		
   		char encrypt_message_part[strlen_1];
  		memset(encrypt_message_part,0,strlen_1);
  		
  		//printf("enc_mes_BEFORE_1/2 = |%s|\n",enc_mes);
  		int pp = 0;
 		for (pp;pp<strlen_1;pp++)
 			{
 			
 			//printf("enc_mes[%d] = %c | symbol_code = %d\n",pp,enc_mes[pp],enc_mes[pp]);
 			
 			
 				if (enc_mes[pp] != '\0')
 					{

 						encrypt_message_part[pp] = enc_mes[pp];
 						//printf("encrypt_message_part[%d] = %c | symbol_code = %d\n",pp,encrypt_message_part[pp],encrypt_message_part[pp]);
 					}
 				else break;	
 			
 			}
 		
 		encrypt_message_part[pp] = '\0';
 		
 		//printf("encrypt_message_part_BEFORE = |%s|\n",encrypt_message_part);
 		//printf("enc_mes_BEFORE = |%s|\n",enc_mes);
 		//sleep(2);
 		
 		//printf(" encrypt_message_part = %s|\n", encrypt_message_part);
   	
   		/*
   		// Отсылаем запрос на хэш и принятый хэш записываем в массив HASH
   		const char* str1 = "../GET_LAST_ID_CONTAINER_HASH/netclient ";
   		const char* str2 = IP[j];
   		const char* str3 = Port;
   		*/
   		
		int length = snprintf( NULL, 0, "%d", new_last_id );
   		char str4 [length + 1];
   		memset(str4,0,length + 1);
   		snprintf( str4, length + 1, "%d", new_last_id );
   		//str4[length] = ';';
   		str4[length] = '\0';


		//printf(" encrypt_message_part = %s|\n", encrypt_message_part); 		
 		int id_hash_length = strlen(str4) + strlen(last_id_container_hash) +2 ; // +2- это для ';'-того, что между %s и %s '\0'-символа конца строки
   		char id_hash [id_hash_length];

		snprintf(id_hash, sizeof id_hash, "%s;%s", str4, last_id_container_hash);

		/*

   		int third_size = strlen(str1) + strlen(str2) +1 ; // +1 - это для '\0'-символа конца строки
   		char third [third_size];
   		
   		snprintf(third, sizeof third, "%s%s", str1, str2);
   		
  
   		int request_size = strlen(third) + strlen(str3) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name [request_size];
 
   		snprintf(get_hash_programm_name, sizeof get_hash_programm_name, "%s%s", third, str3);
   		
   		
   		int request_size_full = strlen(get_hash_programm_name) + strlen(id_hash) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name_full [request_size_full];
   		
   		snprintf(get_hash_programm_name_full, sizeof get_hash_programm_name_full, "%s%s", get_hash_programm_name, id_hash);
   		
   		*/
   		
   		//printf("id_hash = %s|\n",id_hash);
   		
   		
   		int request_size_full_2 = strlen(id_hash) + strlen(encrypt_message_part) + 1 + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name_full_2 [request_size_full_2];
   		memset(get_hash_programm_name_full_2,0,request_size_full_2);
   		
   		snprintf(get_hash_programm_name_full_2, sizeof get_hash_programm_name_full_2, "%s;%s", id_hash, encrypt_message_part);
   		
		//printf("id_hash = |%s|\n",id_hash);
		//printf("encrypt_message_part = |%s|\n",encrypt_message_part);
		
   		//printf("Вызов программы для ANSWER-запроса с сервера = {%s}",get_hash_programm_name_full_2);


		//printf("IP[j] = |%s|\n", IP[j]);
		//printf("my_global_ip = |%s|\n", my_global_ip);
   		//fflush(stdout);
   		//sleep(2);

		if (strcmp(use_static_tunnels,"TRUE") != 0)
		{
   		 if (strcmp(IP[j],my_global_ip) == 0)
   			{
  
     				strncpy (ANSWER[j], get_hash_programm_name_full_2, request_size_full_2);
   				ANSWER[j][request_size_full_2]='\0';
   				//printf("Принятый У СЕБЯ ЖЕ ANSWER = %s|\n", ANSWER[j]);
   				//fflush(stdout);
   				//sleep(2);
   				
     				continue;
     						
   			}
   		}
   		else
   		{
   			//printf("my_port_send_message = |%s|, PORTS[j] = |%s|\n", my_port_send_message, PORTS[j]);
   			//sleep(5);
   			if (strcmp(PORTS[j],my_port_send_message) == 0)
   			{
  				//printf("yes\n");
     				strncpy (ANSWER[j], get_hash_programm_name_full_2, request_size_full_2);
   				ANSWER[j][request_size_full_2]='\0';
   				//printf("Принятый У СЕБЯ ЖЕ ANSWER = %s|\n", ANSWER[j]);
   				//fflush(stdout);
   				//sleep(2);
   				
     				continue;
     						
   			}
   			else
   			{
   				//printf("no\n");
   			}
   		}
   			

   		char res_buf [ANSWER_LENGTH];
   		int c = 0;
   		for (c; c < ANSWER_LENGTH; c++)
   			res_buf[c] = '\0';
   			
   		//call_Linux_programm(get_hash_programm_name_full_2, res_buf);
   		
		//printf("IP[j]_1 = %s\n", IP[j]);
		//printf("get_hash_programm_name_full_2 = |%s|\n",get_hash_programm_name_full_2);
		if (strcmp(use_static_tunnels,"TRUE") != 0)
   			get_hash_or_blockchain(IP[j], Port, get_hash_programm_name_full_2, res_buf);
   		else
   			get_hash_or_blockchain("127.0.0.1", atoi((const char*)PORTS[j]), get_hash_programm_name_full_2, res_buf);
   		
   		if (strcmp(res_buf, "No_response_message") == 0)
   			{
   				// Копируем своей запрос (принимаем его как правильный)
   				strncpy (ANSWER[j], get_hash_programm_name_full_2, request_size_full_2);
   				ANSWER[j][request_size_full_2]='\0';


				memset(ANSWER[j],0,ANSWER_LENGTH);
				ANSWER[j][0] = 'G';
				ANSWER[j][1] = 'A';
				ANSWER[j][2] = 'G';
				if (strcmp(use_static_tunnels,"TRUE") != 0)
					printf("No_response_message from IP = %s\n", IP[j]);
				else
					printf("No_response_message from PORT = %d\n", atoi((const char*)PORTS[j]));
				sleep(2);
	
   				continue;
   			}
   		
   		int real_length = strlen(res_buf);
   		strncpy (ANSWER[j], res_buf,real_length);
   		ANSWER[j][real_length]='\0';
   		//printf("Принятый ANSWER = %s|\n", ANSWER[j]);
   		
   	}

    // А тут получается мы выбираем ANSWER по принципу "каких ответов больше, тот ответ и правильный"

   int ij = 0;
   int ji = 0;
   int frequence [count_];

   int y = 0;
   for (y; y < count_; y++)
   	frequence[y] = 0;


   // Считаем частоту встречания того или иного хэша
   //printf("count_ = %d\n", count_);
   for (ij; ij < count_; ij++)
   	{
   		for (ji; ji < count_; ji++)
   			{
   				if(strcmp(ANSWER[ij],ANSWER[ji]) == 0)
   					{
   						frequence[ij] = frequence[ij] + 1;
   					} 	
   			}
   		ji = 0;	
   	}
   

	int jjj = 0;
	for (jjj; jjj < count_; jjj++)
		{
		//printf("ANSWER[%d]=|%s|\n",jjj,ANSWER[jjj]);
		//printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);		
		}


    // Находим наибольший элемент среди frequence
    
    int ii = 0;
    int maximum = 0; 
    int maximum_ID = 0; // По умолчанию - 0-ый элемент массива самый наиболее встречающийся
    for (ii; ii < count_ - 1; ii++)
    	{
    	
    		if (frequence[ii] > maximum)//frequence[ii+1])
    			{
    				maximum = frequence[ii];
    				maximum_ID = ii;
    			}
    	}

    if (frequence[count_ -1] > maximum)
    {
    	maximum = frequence[count_ -1];
    	maximum_ID = count_ -1;
    }	

   //printf("%s%s|\n","Наиболее часто встречающийся ANSWER = ", ANSWER[maximum_ID]);
   



	// ---- 03-09-21 ----------------------------- BEGIN --------------------------------

    if (strcmp(ANSWER[maximum_ID],"GAG") == 0)
	{
		// Ищем новый максимум
		int max_flag = 0;	
		int new_max = maximum;
		int iii = 0;
		while (new_max != 0)
		{
			
			for (iii=0; iii<count_; iii++)
			{
				if ((frequence[iii] == new_max) && (strcmp(ANSWER[iii],"GAG") != 0))
				{
					maximum = new_max;
					maximum_ID = iii;
					max_flag = 1;
					break;
				}
			}
			if (max_flag == 1)
				break;
			new_max--;
			iii = 0;
		}
	}

	jjj = 0;
	for (jjj; jjj < count_; jjj++)
		{
		//printf("ANSWER[%d]=|%s|\n",jjj,ANSWER[jjj]);
		//printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);		
		}

	 //printf("%s%s|\n","Наиболее часто встречающийся ANSWER ТЕПЕРЬ = ", ANSWER[maximum_ID]);

// ---- 03-09-21 ----------------------------- END --------------------------------






    if (strcmp(ANSWER[maximum_ID],"NO") == 0)
    	{
    		/*
    		//printf("%s\n", "-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0--0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0");
    		//printf("%s\n", "Т.к большинство не согласно с вашей копией blockchain, то придется запросить actual_blockchain_copy.");
    		
    		// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети
		get_actual_blockchain_copy(port2);
	
		// Очищение старой копии BLOCKCHAIN_TABLE
		Clear_Blockchain_Table(PATH_Date_Base);
	
		// Заполнение БД из файла
		const char* path_to_buffer_file = "actual_blockchain.txt";
		blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
		analyse_blockchain_for_input_messages(PATH_Date_Base);
		*/
    		sleep(1);
    		//printf("%s\n", "-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0--0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0");
    		//printf("%s\n","\n\tВАШЕ СООБЩЕНИЕ НЕ ОТПРАВЛЕНО, НО БЛОКЧЕЙН ОБНОВЛЕН. ПОПЫТАЙТЕСЬ ЕЩЕ РАЗ.\n");
    		return 1;
    	
    	}
    
    
    // Просто дописываем этот самый часто встречающийся ANSWER[maximum_ID] (уже содержащий и наш блок данных в конце) в блокчейн-файл
    // Не совсем просто, а обрезаем вначале первые 2 ';'
    
    int i_count = 0;
    int number = 0;
    for (i_count;i_count<MAX_TRANSACTION_LENGTH; i_count++)
    	{
    		if (ANSWER[maximum_ID][i_count] == ';')
    			{
    				number++;
    				if (number == 2)
    					{
    						break;
    					}
    				
    			}
    	}
    
    int len_size = strlen(ANSWER[maximum_ID]) - i_count + 1;
    
    char for_writing [len_size];
    memset(for_writing,0,len_size);
    
    int c_i = 0;
    for (c_i;c_i<len_size; c_i++)
    	{
    	
    		if (ANSWER[maximum_ID][i_count + c_i + 1] != '\0')
    			for_writing[c_i] = ANSWER[maximum_ID][i_count + c_i + 1];
    		else
    			break;	
    	
    	}
    
    for_writing[c_i] = '~';
    for_writing[c_i + 1] = '\0';
    
    int new_leng = c_i + 1;
    
    i_count++;
    int res_write = write_to_file_name_c (for_writing, new_leng, path_to_buf_file);
    if (res_write != 0)
    	{
    		printf("%s\n", "При добавлении блока(-ов) к локальной копии блокчейн произошла ошибка.");
		printf("NOT NORMAL : for_writing = |%s|_|%d|\n", for_writing, new_leng);
		sleep(2);
    	}
    else 
    	{
		//printf("NORMAL : for_writing = |%s|_|%d|\n", for_writing, new_leng);
		//sleep(2);
    		//printf("%s\n", "Добавление блока(-ов) к локальной копии блокчейн успешно завершено.");
    	}
    	
    // Тут вызываем файл добавления в blockchain всх необходимых транзакций		

	// Тут алгоритм проверки нового блокчейн на предмет сообщений для нашего пользователя (как минимум должно найтись хотя бы одно сообщение - наше)
		// Будем его вызывать из основной программы, чтобы не переписывать зановго код 

		// Если поверка пройдет успешно (и в БД в открытом виде будет добавлена переписка, то сообщение можно считать отправленным)
	

   //printf("%s\n","Сообщение удачно отправлено.");

   sending_file_flag = 0;
   return 0;
} 


int get_actual_blockchain_copy (int port_number)
{  

write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"get_actual_blockchain_copy\"]\n");

// GET_ACTUAL_BLOCKCHAIN_COPY -----------------------

char IP[MAX][INET_ADDRSTRLEN];
char PORTS[MAX][MAX_PORT_ADDRESS_LENGTH];
char HASH[MAX][HEX_LENGTH];

int Port = port_number;

// GET_ACTUAL_BLOCKCHAIN_COPY -----------------------

// Первоначальное заполнение массивов

   int i = 0;
   for (i;i<MAX;i++)
   	{
   		memset(IP[i], 0, INET_ADDRSTRLEN);
   		memset(PORTS[i], 0, MAX_PORT_ADDRESS_LENGTH);
   		memset(HASH[i], 0, HEX_LENGTH);
   	}

      // Переменная, в которую будет помещен указатель на созданный
   // поток данных 
   FILE *mf;
   // Переменная, в которую поочередно будут помещаться считываемые строки
   char str[50];
   memset(str, 0, 50);

   //Указатель, в который будет помещен адрес массива, в который считана 
   // строка, или NULL если достигнут коней файла или произошла ошибка
   char *estr;

   // Открытие файла с режимом доступа «только чтение» и привязка к нему 
   // потока данных
   //printf ("Открытие файла: ");

   pthread_mutex_lock(&mutex1); 

   mf = fopen ("user_list_client.txt","r");

   // Проверка открытия файла
   if (mf == NULL) {
   //printf ("ошибка\n"); 
   return -1;
   }
   else 
   {
   	//printf ("выполнено\n");
   
   }
   
  // printf ("Считаны строки:\n");

   int count_IP = 0;


   //Чтение (построчно) данных из файла в бесконечном цикле
   while (1)
   {
      // Чтение одной строки  из файла
      estr = fgets (str,sizeof(str),mf);

      //Проверка на конец файла или ошибку чтения
      if (estr == NULL)
      {
         // Проверяем, что именно произошло: кончился файл
         // или это ошибка чтения
         if ( feof (mf) != 0)
         {  
            //Если файл закончился, выводим сообщение о завершении 
            //чтения и выходим из бесконечного цикла
           //printf ("\nЧтение файла закончено\n");
            break;
         }
         else
         {
            //Если при чтении произошла ошибка, выводим сообщение 
            //об ошибке и выходим из бесконечного цикла
           // printf ("\nОшибка чтения из файла\n");
            break;
         }
      }
      //Если файл не закончился, и не было ошибки чтения 
      //выводим считанную строку  на экран
      //printf ("     str = %s|",str);
      
      // Записываем полученную строку в массив IP
      strncpy (IP[count_IP], str, strlen(str)); // -1
      int size_IP_str = strlen(str);
      IP[count_IP][size_IP_str] = '\0';
      //printf ("     IP  = %s|",IP[count_IP]);
      
      count_IP++;
      
   }

   // Закрываем файл
  // printf ("Закрытие файла: ");
   if ( fclose (mf) == EOF)
   {
   	 printf ("ошибка\n");
   	 exit(1);
   }
   
   else 
   {
   	//printf ("выполнено\n");
   }
   

pthread_mutex_unlock(&mutex1); 








// =====================================================================

	pthread_mutex_lock(&mutex1);
            
            

   mf = fopen ("user_list_client_port_hash_blockchain.txt","r");

   // Проверка открытия файла
   if (mf == NULL) {
   	//printf ("ошибка\n"); 
   	return -1;
   }
   else 
   
   {
   	//printf ("выполнено\n");
	}
  // printf ("Считаны строки:\n");


   int count_PORTS = 0;


   //Чтение (построчно) данных из файла в бесконечном цикле
   while (1)
   {
      // Чтение одной строки  из файла
      estr = fgets (str,sizeof(str),mf);

      //Проверка на конец файла или ошибку чтения
      if (estr == NULL)
      {
         // Проверяем, что именно произошло: кончился файл
         // или это ошибка чтения
         if ( feof (mf) != 0)
         {  
            //Если файл закончился, выводим сообщение о завершении 
            //чтения и выходим из бесконечного цикла
          //  printf ("\nЧтение файла закончено\n");
            break;
         }
         else
         {
            //Если при чтении произошла ошибка, выводим сообщение 
            //об ошибке и выходим из бесконечного цикла
        //    printf ("\nОшибка чтения из файла\n");
            break;
         }
      }
      //Если файл не закончился, и не было ошибки чтения 
      //выводим считанную строку  на экран
     // printf ("     str = %s|",str);
      
      // Записываем полученную строку в массив IP
      strncpy (PORTS[count_PORTS], str, strlen(str));
      int size_PORT_str = strlen(str);
      PORTS[count_PORTS][size_PORT_str - 1] = '\0';
      PORTS[count_PORTS][size_PORT_str] = '\0';
      //printf ("     IP  = %s|",IP[count_IP]);
    //  printf ("     IP  = %s|",IP[count_IP]);
      
      count_PORTS++;
      
      
   }

   // Закрываем файл
  // printf ("Закрытие файла: ");
   if ( fclose (mf) == EOF)
   {
   	printf ("ошибка\n");
   	exit(1);
   	}
    
   else 
   {
   
   	//printf ("выполнено\n");
   }
   
	pthread_mutex_unlock(&mutex1);


// =====================================================================





 //printf("count_IP = %d\n", count_IP);

   
   int j = 0;
   int count_;
   if (strcmp(use_static_tunnels,"TRUE") == 0)
   	count_ = count_PORTS;
   else
   	count_ = count_IP;
   
   for (j; j < count_; j++)
   	{
   	
   		if (strcmp(use_static_tunnels,"TRUE") != 0)
		{
   			if (strcmp(IP[j],my_global_ip) == 0)
   			{
   				
   				
   				char last_id_c[BUF_SIZE];
     				memset(last_id_c, 0, BUF_SIZE);
     				get_how_much_id (PATH_Date_Base,last_id_c);
   				
   				
   				
   				// Функция получения хэша последнего id-контейнера
     				memset(HASH[j], 0, HEX_LENGTH);
     				get_hash_id_container (PATH_Date_Base, last_id_c, HASH[j]);	
     				
     				
     				//printf("Принятый У СЕБЯ ЖЕ HASH = %s|\n", HASH[j]);
     				continue;

   			}
   		}
   		else
   		{
   			//printf(" my_port_hash_blockain = |%s|, PORTS[j] = |%s|\n",  my_port_hash_blockain, PORTS[j]);
   			//printf("my_port_hash_blockain = |%s|\n", my_port_hash_blockain);
   			//sleep(5);
   			if (strcmp(PORTS[j],my_port_hash_blockain) == 0)
   			{
   				
   				//printf("yes\n");
   				char last_id_c[BUF_SIZE];
     				memset(last_id_c, 0, BUF_SIZE);
     				get_how_much_id (PATH_Date_Base,last_id_c);
   				
   				
   				
   				// Функция получения хэша последнего id-контейнера
     				memset(HASH[j], 0, HEX_LENGTH);
     				get_hash_id_container (PATH_Date_Base, last_id_c, HASH[j]);	
     				
     				
     				//printf("Принятый У СЕБЯ ЖЕ HASH = %s|\n", HASH[j]);
     				continue;

   			}
   			else
   			{
   				//printf("no\n");
   			}	
   		}
   	
   		//printf("%s\n","проверка ...");
   	
   		/*
   	
   		// Отсылаем запрос на хэш и принятый хэш записываем в массив HASH
   		const char* str1 = "../GET_LAST_ID_CONTAINER_HASH/netclient ";
   		const char* str2 = IP[j];
   		const char* str3 = Port;
   		const char* str4 = " hash";

   		
   		int third_size = strlen(str1) + strlen(str2) +1 ; // +1 - это для '\0'-символа конца строки
   		char third [third_size];
   		
   		
   		snprintf(third, sizeof third, "%s%s", str1, str2);
   		
   		int request_size = strlen(third) + strlen(str3) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name [request_size];
 
   		snprintf(get_hash_programm_name, sizeof get_hash_programm_name, "%s%s", third, str3);
   		
   		
   		
   		int request_size_full = strlen(get_hash_programm_name) + strlen(str4) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name_full [request_size_full];
   		
   		snprintf(get_hash_programm_name_full, sizeof get_hash_programm_name_full, "%s%s", get_hash_programm_name, str4);
   		
   		
   		printf("Вызов программы для HASH-запроса с сервера = {%s}",get_hash_programm_name_full);
   		
   		*/
   		
   		char res_buf [HEX_LENGTH];
   		int c = 0;
   		for (c; c < HEX_LENGTH; c++)
   			res_buf[c] = '\0';
   			
   		//call_Linux_programm(get_hash_programm_name_full, res_buf);
   		
		//printf("IP[j]_2 = %s\n", IP[j]);
		if (strcmp(use_static_tunnels,"TRUE") != 0)
   			get_hash_or_blockchain(IP[j], Port, "hash", res_buf);
   		else
   			get_hash_or_blockchain("127.0.0.1", atoi((const char*)PORTS[j]), "hash", res_buf);	
   		
   		if (strcmp(res_buf, "No_response_hash") == 0)
   		{	
   				char last_id_c[BUF_SIZE];
     				memset(last_id_c, 0, BUF_SIZE);
     				get_how_much_id (PATH_Date_Base,last_id_c);
   				
   				
   				
   				// Функция получения хэша последнего id-контейнера
     				memset(HASH[j], 0, HEX_LENGTH);
     				get_hash_id_container (PATH_Date_Base, last_id_c, HASH[j]);	
     				
				// Заполняю заглушкой
				memset(HASH[j], 0, HEX_LENGTH);
				HASH[j][0] = 'G';
				HASH[j][1] = 'A';
				HASH[j][2] = 'G';

				if (strcmp(use_static_tunnels,"TRUE") != 0)
     					printf("No_response_hash from IP = %s\n", IP[j]);
     				else
     					printf("No_response_hash from PORT = %d\n", atoi((const char*)PORTS[j]));	
				sleep(2);
     				continue;
   		}
   		
   		int real_length = strlen(res_buf);
   		
   		strncpy (HASH[j], res_buf,real_length);
   		HASH[j][real_length]='\0';
   		//printf("Принятый HASH = %s|\n", HASH[j]);
   		
   	}

    // А тут получается мы выбираем HASH по принципу "каких хэшей больше, тот хэш и правильный"

   int ij = 0;
   int ji = 0;
   int frequence [count_];

   int y = 0;
   for (y; y < count_; y++)
   	frequence[y] = 0;

   //printf("count_ = %d\n", count_);
   // Считаем частоту встречания того или иного хэша
   for (ij; ij < count_; ij++)
   {
   	for (ji; ji < count_; ji++)
   		{
   			if(strcmp(HASH[ij],HASH[ji]) == 0)
   				{
   					frequence[ij] = frequence[ij] + 1;
   				} 	
   		}
   		ji = 0;
   }

    int jjj = 0;
	for (jjj; jjj < count_; jjj++)
		{
		//printf("HASH[%d]=|%s|\n",jjj,HASH[jjj]);
		//printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);		
		}

    // Находим наибольший элемент среди frequence
    

    int maximum = 0; 
    int maximum_ID = 0; // По умолчанию - 0-ый элемент массива самый наиболее встречающийся
    int ii = 0;

    for (ii; ii < count_ - 1; ii++)
    	{
    	
    		if (frequence[ii] > maximum)//frequence[ii+1]))
    			{
    				maximum = frequence[ii];
    				maximum_ID = ii;
    			}
    	}

    if (frequence[count_ -1] > maximum)
    {
    	maximum = frequence[count_ -1];
    	maximum_ID = count_ -1;
    }	
    

	


    //printf("%s%s|\n","Наиболее часто встречающийся ХЭШ = ", HASH[maximum_ID]);


// ---- 03-09-21 ----------------------------- BEGIN --------------------------------

    if (strcmp(HASH[maximum_ID],"GAG") == 0)
	{
		// Ищем новый максимум
		int max_flag = 0;	
		int new_max = maximum;
		int iii = 0;
		while (new_max != 0)
		{
			for (iii=0; iii<count_; iii++)
			{
				if ((frequence[iii] == new_max) && (strcmp(HASH[iii],"GAG") != 0))
				{
					maximum = new_max;
					maximum_ID = iii;
					max_flag = 1;
					break;
				}
			}
			if (max_flag == 1)
				break;
			new_max--;
			iii = 0;
		}
	}

	 jjj = 0;
	 for (jjj; jjj < count_; jjj++)
		{
		//printf("HASH[%d]=|%s|\n",jjj,HASH[jjj]);
		//printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);		
		}
	 //printf("%s%s|\n","Наиболее часто встречающийся ХЭШ ТЕПЕРЬ = ", HASH[maximum_ID]);

// ---- 03-09-21 ----------------------------- END --------------------------------
 
    
    // Отправляем запрос к IP[maximum_ID] на получение полной актуальной копии blockchain-цепочки
    
   // printf("%s\n","Отправка запроса на получение акутальной копии блокчейн-цепочки");
    
    
    /*
    
    	// Отсылаем запрос на хэш и принятый хэш записываем в массив HASH
   		const char* str1 = "../GET_LAST_ID_CONTAINER_HASH/netclient ";
   		const char* str2 = IP[maximum_ID];
   		const char* str3 = Port;
   		const char* str4 = " blockchain";

   		
   		int third_size = strlen(str1) + strlen(str2) +1 ; // +1 - это для '\0'-символа конца строки
   		char third [third_size];
   		
   		
   		snprintf(third, sizeof third, "%s%s", str1, str2);
   		
   		int request_size = strlen(third) + strlen(str3) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name [request_size];
 
   		snprintf(get_hash_programm_name, sizeof get_hash_programm_name, "%s%s", third, str3);
   		
   		
   		
   		int request_size_full = strlen(get_hash_programm_name) + strlen(str4) + 1; // +1 - это для '\0'-символа конца строки
   		char get_hash_programm_name_full [request_size_full];
   		
   		snprintf(get_hash_programm_name_full, sizeof get_hash_programm_name_full, "%s%s", get_hash_programm_name, str4);
   		
   		
   		printf("Вызов программы для запроса blockchain-цепочки с сервера = {%s}",get_hash_programm_name_full);
   		
   		
   		*/
   		
   		
   		char res_buf [BUF_SIZE]; // HEX_LENGTH
   		int c = 0;
   		for (c; c < BUF_SIZE; c++)
   			res_buf[c] = '\0';
   			
   		//call_Linux_programm(get_hash_programm_name_full, res_buf);
   		
		//printf("IP[maximum_ID] = %s\n", IP[maximum_ID]);
		if (strcmp(use_static_tunnels,"TRUE") != 0)
   			get_hash_or_blockchain(IP[maximum_ID], Port, "blockchain", res_buf);
   		else
   			get_hash_or_blockchain("127.0.0.1",atoi((const char*)PORTS[maximum_ID]), "blockchain", res_buf);	
   		
   		if (strcmp(res_buf, "No_response_blockchain") == 0)
   			{
   				int io = 1;
   				while (strcmp(res_buf, "No_response_blockchain") == 0)
					{
						c = 0;
   						for (c; c < BUF_SIZE; c++)
   							res_buf[c] = '\0';
						//printf("IP[count_IP -io] = %s\n", IP[maximum_ID]);
						if (strcmp(use_static_tunnels,"TRUE") != 0)
							printf("No_response_blockchain from IP = %s\n", IP[count_ -io -1]);
						else
							printf("No_response_blockchain from PORT = %d\n", atoi((const char*)PORTS[count_ -io -1]));	
						sleep(1);
						if (strcmp(use_static_tunnels,"TRUE") != 0)
							get_hash_or_blockchain(IP[count_ -io], Port, "blockchain", res_buf);
						else
							get_hash_or_blockchain("127.0.0.1", atoi((const char*)PORTS[count_ -io]), "blockchain", res_buf);							
						io++;
					}
   			}
   		
   		int real_length = strlen(res_buf);
   		
   		char res_buf1 [real_length + 1];
   		int c1 = 0;
   		for (c1; c1 < real_length+1; c1++)
   			res_buf1[c1] = '\0';
   			
   		int c2 = 0;
   		for (c2; c2 < real_length+1; c2++)
   			if (res_buf[c2] != '\0')
   				res_buf1[c2] = res_buf[c2];
   			else 
   				break;		
   			
   		res_buf1[c2] = '~';
   		res_buf1[c2] = '\0'; // c2 + 1
   		
   		real_length = c2 ; // c2 + 1	
   		
   		// Запись полученного буфера в файл actual_blockchain.txt
   		
   		FILE* fd = fopen("actual_blockchain.txt", "wb");
			if (fd == NULL) 
    				printf("Error opening file for writing");
		fwrite(res_buf1, 1, real_length, fd);
		fclose(fd);
   		
   		//printf("Принятый blockchain = %s|\n", res_buf1);


  // printf("%s\n","Получение актуальной копии блокчейн-цепочки завершено успешно");
   return 0;
} 


int unsigned_char_array_4_to_int (unsigned char* uc_int)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"unsigned_char_array_4_to_int\"]\n");
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

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"int_to_unsigned_char_array_4\"]\n");
	uc_int[0] = digit & 0xFF;
	uc_int[1]=(digit >> 8) & 0xFF;
	uc_int[2]=(digit >> 16) & 0xFF;
	uc_int[3]=(digit >> 24) & 0xFF;
	
	return;
	
}


//ATTRIBUTE_NO_SANITIZE_ADDRESS
void * thread_func_send_message_server_part(void * arg)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"thread_func_send_message_server_part\"]\n");
//pthread_mutex_lock(&mutex1);

//printf("%s\n","check_1");

     char buf[BUF_SIZE];
     struct sockaddr_in cli_addr, my_addr;     
     struct str_thread structure;
     structure = *(struct str_thread*) arg;   
     cli_addr = structure.x;
     my_addr = structure.z;
     int newsock = structure.y;
     const char* path_to_db = structure.path_to_db;
     memset(buf, 0, BUF_SIZE);
     
     unsigned char buf_int [4];
     int total_recieve = 0;
     memset(buf_int,0,4);
     int real_read_from_newsock = 0;
     
     
	while (total_recieve != 4) // 4 байт 
		{
			real_read_from_newsock = 0;
			real_read_from_newsock = read(newsock, &buf_int[total_recieve], 4);
			//printf("real_read_from_newsock_1 = %d\n", real_read_from_newsock);
			//sleep(5);
			if (real_read_from_newsock <= 0)
				{
					char str[INET_ADDRSTRLEN];
     					// Where we sent hello-message ?
    					inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     					//printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     					printf("%s%s%s\n","[thread_func_send_message_server_part]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    					close(newsock);
    					return;
				}
			total_recieve = total_recieve + real_read_from_newsock;		
		}   
 	
     	
     	
     	
     	
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- BEGIN --------------------    
    
    // Тут должно прийти 4 байта
    
    int wait_bayte = unsigned_char_array_4_to_int(buf_int);
    total_recieve = 0;
    memset(buf, 0, BUF_SIZE);
    
    while(total_recieve != wait_bayte)
    	{
    		real_read_from_newsock = 0;
    		// memset(buf, 0, BUF_SIZE);
    		real_read_from_newsock = read (newsock, &buf[total_recieve], BUF_SIZE-1);
    		//printf("real_read_from_newsock_2 = %d\n", real_read_from_newsock);
		//sleep(5);
    		if (real_read_from_newsock <=0)
    			{
    				char str[INET_ADDRSTRLEN];
     				// Where we sent hello-message ?
    				inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     				//printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     				printf("%s%s%s\n","[thread_func_send_message_server_part]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    				close(newsock);
    				return;
    
   			}
   		
   		total_recieve = total_recieve + real_read_from_newsock;	
    	}
    
    
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- END --------------------
     	
     	
     	
     	
     	
     	
     
     
     
     
     buf[total_recieve] = '\0';
     
     //printf("MSG: %s|\n", buf);
     //printf("strlen(buf) = %d\n",strlen(buf));
     //sleep(5);

  
 // printf("%s\n","check_2");
  
  
     // Отделяем в отдельную переменную ID и в отдельную переменную HASH_of_ID-1
     
     int i = 0;
     for (i; i < BUF_SIZE; i++)
     	{
     		if (buf[i] == ';')
     			{
     				break;
     			}
     	}
 
     //printf("check_1:i = %d|\n",i);
     // Копируем считанное как ID и HASH
     
     
     int size_1 = i;
     char ID_str[size_1+1];
     memset(ID_str,0,size_1+1);
     strncpy (ID_str, buf, size_1);
     ID_str[size_1] = '\0';
     //printf ("     ID_str  = |%s|\n",ID_str);
     
     int ID = atoi((const char*)ID_str);
     
     
     //printf("check_2: ID = %d|\n", ID);
     
     i++;
     for (i; i < BUF_SIZE; i++)
     	{
     		if (buf[i] == ';')
     			{
     				break;
     			}
     	}
     
          
     int len_of_hash = i - 1 - size_1;     
     char HASH_of_before_ID[len_of_hash+1];	
     strncpy (HASH_of_before_ID, &buf[size_1 + 1], len_of_hash);
     HASH_of_before_ID[len_of_hash] = '\0';
 //    printf ("     HASH_of_before_ID  = |%s|\n", HASH_of_before_ID);
     
     i++;
     

     int len_of_enc_mess = total_recieve - size_1 - len_of_hash - 2;
     //printf ("len_of_enc_mess = %d\n",len_of_enc_mess);
     fflush(stdout);
     //sleep(10);	
     char encrypt_message_part[len_of_enc_mess + 1];
     memset(encrypt_message_part,0,len_of_enc_mess + 1);
     strncpy (encrypt_message_part, &buf[size_1 + len_of_hash + 2], len_of_enc_mess);
     encrypt_message_part[len_of_enc_mess] = '\0';
     //printf ("     ENC_MESS  = |%s|, len = %d\n",encrypt_message_part, strlen(encrypt_message_part));
     fflush(stdout);

	//printf("check_3:|\n");

     // Запросить сколько всего id  в БД
     
     char last_id_c[10]; //BUF_SIZE
     memset(last_id_c, 0, 10); //BUF_SIZE
     get_how_much_id (path_to_db,last_id_c);  
     int last_id = atoi((const char*)last_id_c);
  //   printf("last_id = %d|\n", last_id);
     
     // Если в БД last_id <= 10, то size_buf = last_id, иначе size_buf = 10; 
     int size_buf;
     
     size_buf = last_id;
     
     //if (last_id <= 10)
     	
     //else 
    //	size_buf = 10;	

     // Запрос из БД последних size_buf id
     int my_DB_last_ID_arr[size_buf];
     int ii = 0;
     for(ii; ii < size_buf; ii++)
	{
		my_DB_last_ID_arr[ii] = size_buf - ii;
	}
     
     
     //printf("check_4:|\n");
     
     // Запрос из БД hash последнеих 10-ти id-контейнеров
     
     char my_DB_last_ID_container_hash_arr [size_buf][HEX_LENGTH];
     
     ii = 0;
     for(ii;ii<size_buf;ii++)
     	{
     		
     		memset(my_DB_last_ID_container_hash_arr[ii], 0, HEX_LENGTH);
     		int length = snprintf( NULL, 0, "%d", my_DB_last_ID_arr[ii]);
    		const char* last_id_str [length + 1];
    		snprintf(last_id_str, length + 1, "%d", my_DB_last_ID_arr[ii]); // по идее в последнем символе и так уже ноль должен быть
    		last_id_str[length] = '\0';
     		
		get_hash_id_container (path_to_db, last_id_str, my_DB_last_ID_container_hash_arr[ii]);

	//	printf("my_DB_last_ID_container_hash_arr[%d] = %s|\n", ii, my_DB_last_ID_container_hash_arr[ii]);
     	}


      int my_DB_last_ID = my_DB_last_ID_arr[0];
      const char* my_DB_last_ID_container_hash = my_DB_last_ID_container_hash_arr[0];

     //  Проверяем совпадение hash-ей (какой посчету с конца совпадет с присланным)
     
     int count_hash = 0;
  //   printf("%s\n","checking ...");
     
     
     //printf("check_5:|\n");
     
     int flag = 0;
     for (count_hash; count_hash < size_buf; count_hash ++) // count_hash < size_buf поменяли на 3 (так надо для правильной работы алгоритмов)
     	{
     	
     	
     		if (count_hash > 0)
     			break;
     	//printf("check_6:|\n");
  //   		printf("%s\n","checking ...");
     		
  //   		printf("ID = %d\n",ID);
    // 		printf("my_DB_last_ID = %d\n",my_DB_last_ID);
     		
     	//	printf("HASH_of_before_ID = %s|\n",HASH_of_before_ID);
     	//	printf("my_DB_last_ID_container_hash = %s|\n",my_DB_last_ID_container_hash);
     		
     		     		
     		if (((ID - 1) == my_DB_last_ID) && (strcmp(HASH_of_before_ID, my_DB_last_ID_container_hash) == 0))
     			{
     		//	printf("check_7:|\n");
     		//	printf("%s\n","checking ...");
     			//sleep(2);
     				// Пересылаем этот же буфер обратно как ответ 
     				if (count_hash == 0)
     					{
     					//printf("check_8:|\n");
     						// Добавление BEGIN -----------------------
     						int len_buf = strlen(buf);
     						unsigned char uc_int [4];
     						memset(uc_int,0,4);
     						int_to_unsigned_char_array_4 (len_buf, uc_int);
     						write(newsock, uc_int, 4);
     						// Добавление END -------------------------
     						
     						write(newsock, buf, strlen(buf));
     					}
     					
     				else
     				
     				
     					//Это никогда не запуститься
     					{
     						//printf("check_9:|\n");
     						//Запросить все целиком контейнеры из БД (от "count_hash -1" до "0")	
     						
     						
     						// Сформировать из них большой буфер char[]
     						char big_buffer[MAX_TRANSACTION_LENGTH];
     						memset(big_buffer, 0, MAX_TRANSACTION_LENGTH);
     						
     						//printf("%s","checking...\n");
     						
     						const char* path_to_buffer_file = "buf_file.txt";
     						int from_id =  my_DB_last_ID + 1;      						
     						int size_of_buffer = blockchain_from_DB_to_file_c_correct (path_to_db, from_id, path_to_buffer_file, big_buffer);
     						
     						 if(remove(path_to_buffer_file)) 
     						 	{
        					       	printf("Error removing path_to_buffer_file");
        					        	exit(1);
        					        }
     						
     						//printf("Размер буфера = %d\n", size_of_buffer);
     						
     						// Добавить к этому всему тот контейнер, который прислали (изменив ID на новый, encrypt - не меняем, т.к. он в дальнейшем будет зашифрован)
     						
     						// Узнаем последний существующий ID и прибавляем 1
     						
     						
     						char last_id_c[10]; // BUF_SIZE
     						memset(last_id_c, 0, 10); // BUF_SIZE
     						get_how_much_id (path_to_db,last_id_c);  
     						int last_id = atoi((const char*)last_id_c) + 1;
     						//printf("new_last_id = %d|\n", last_id);
     						
     						
     						int length = snprintf( NULL, 0, "%d", last_id);
    						char id_str [length + 1];
    						snprintf(id_str, length + 1, "%d", last_id);
    						id_str[length] = '\0';
	 					const char* new_id = id_str;
	 		
	 					// Запрашиваем hash_id_container
						// Функция получения хэша последнего id-контейнера
     						char hash_last_id_container[HEX_LENGTH];
     						memset(hash_last_id_container, 0, HEX_LENGTH);
     						get_hash_id_container (path_to_db, last_id_c, hash_last_id_container);
	 				
	 					const char* new_hash_before = hash_last_id_container;
     					
     						//Складываем все вместе
					
						// Склеиваем 3 строки
    						int length_buf = strlen(new_id) + strlen(new_hash_before) + len_of_enc_mess + 1    + 1 + 2; // +1 - это для '\0'-символа конца строки
    						char buf [length_buf];
    						memset(buf,0,length_buf);

    						snprintf(buf, sizeof buf, "%s;%s;%s", new_id, new_hash_before, encrypt_message_part);
    
   						buf[length_buf] = '\0';
     						
     						//printf("FFFFFFIRST_part = %s|\n",big_buffer);
     						
     						//printf("SSSSSSECOND_part = %s|\n",encrypt_message_part);
     			
     						strncpy(&big_buffer[size_of_buffer-1], encrypt_message_part, len_of_enc_mess + 1);
     										
     						// и переслать уже это все в качестве ответа
     						
     						//printf("ТО ЧТО МЫ ПЕРЕДАЕМ = %s|\n",big_buffer);
     						
     						
     						// Добавление BEGIN -----------------------
     						int len_buf = size_of_buffer + len_of_enc_mess + 1;
     						unsigned char uc_int [4];
     						memset(uc_int,0,4);
     						int_to_unsigned_char_array_4 (len_buf, uc_int);
     						write(newsock, uc_int, 4);
     						// Добавление END -------------------------
     						
     						write(newsock, big_buffer, size_of_buffer + len_of_enc_mess + 1);
     						
     					}
     					
     				// Добавить присланную транзакцию в свою копию блокчейн
     				
     				//printf("check_10:|\n");
     					// Запросить сколько всего id  в БД
     
     					char last_id_c1[10]; // BUF_SIZE
     					memset(last_id_c1, 0, 10); // BUF_SIZE
     					get_how_much_id (path_to_db,last_id_c1);  
     					int last_id1 = atoi((const char*)last_id_c1);
     					//printf("last_id = %d|\n", last_id1);
     					
     					// Запросить hash предыдущего
					
					char last_id_container_hash_c1[HEX_LENGTH];
					memset(last_id_container_hash_c1, 0, HEX_LENGTH);
					get_hash_id_container (path_to_db, (const char*) last_id_c1, last_id_container_hash_c1);

					const char* hesh_before = last_id_container_hash_c1;

					int length0 = snprintf( NULL, 0, "%d", len_of_enc_mess);

					char hesh_size[length0+1];
					memset(hesh_size,0,length0+1);
					snprintf(hesh_size, length0 + 1, "%d", len_of_enc_mess); // по идее в последнем символе и так уже ноль должен быть
    					hesh_size[length0] = '\0';
					
					int last_id2 = last_id1 + 1;
					
					int length1 = snprintf( NULL, 0, "%d", last_id2);
    					const char* last_id_str1 [length1 + 1];
    					snprintf(last_id_str1, length1 + 1, "%d", last_id2); // по идее в последнем символе и так уже ноль должен быть
    					last_id_str1[length1] = '\0';
					
					const char* hash_str = "hash_";
					const char* of = "_of_";
					
					// Соединяем несколько строк вместе ...
				
					int union_size = strlen(hash_str) + strlen(hesh_size) + strlen(of) + strlen(last_id_str1) + 1; // +1 - это для '\0'-символа конца строки
   					char union_str [union_size];
   					snprintf(union_str, sizeof union_str, "%s%s%s%s", hash_str, hesh_size, of, last_id_str1);
   					union_str [union_size-1] = '\0';
					
					// Добавляем полученную транзакцию в БД в BLOCKCHAIN_TABLE
					
					// ТО ЧТО СВЕРХУ - ПОКА ОСТАВИМ (ВДРУГ ПРИГОДИТЬСЯ, чтобы ничего не сломалось)
					
					// А тут вычислим НОРМАЛЬНЫЙ HASH
					
					
					//printf("check_11:|\n");
					
					// ------ Вызов функции взятия ХЭШа от const char* ------ BEGIN -----------------
					
					int union_size_1 = strlen(encrypt_message_part) + strlen(hesh_before) + 1; // +1 - это для '\0'-символа конца строки
   					char string_for_hashing_0 [union_size_1];
   					snprintf(string_for_hashing_0, sizeof string_for_hashing_0, "%s%s", encrypt_message_part, hesh_before);
   					string_for_hashing_0 [union_size_1-1] = '\0';
					
					const char* string_for_hashing =  (const char*) string_for_hashing_0; //argv[1];
	
					char hash_from_string[HASH_LENGTH];
					memset(hash_from_string,0,HASH_LENGTH);
	
					get_hash_from_string (string_for_hashing, hash_from_string);
	
					const char* hash_from_string_result = (const char*)hash_from_string;
	
					//printf("hash_from_string_result = %s|\n",hash_from_string_result);
						
					// ------ Вызов функции взятия ХЭШа от const char* ------ END -----------------
					
					// Проверка на совпадение IP-адресов клиента и сервера (чтобы избежать двойного добавления)
					
					char client_IP[INET_ADDRSTRLEN];
     					// Where we sent hello-message ?
    		 			inet_ntop(AF_INET, &(cli_addr.sin_addr), client_IP, INET_ADDRSTRLEN);
    		 			
    		 			
    		 			char My_IP[INET_ADDRSTRLEN];
     					// Where we sent hello-message ?
    		 			inet_ntop(AF_INET, &(my_addr.sin_addr), My_IP, INET_ADDRSTRLEN);		
					
					//printf("check_12:|\n");
					
					
					// Об этом условии можно уже не беспокоиться, самому себе не отправляется транзации (проверка происходит при отправке)
					if (strcmp(client_IP, my_global_ip) == 0)
						{
						
							//printf("check_13:|\n");
							//printf("%s\n", "IP-адресс клиента == моему IP-адресу. Добавление транзакции произведено не будет.");
						}
					else
						{
							//printf("check_14:|\n");
							int res_add = add_record_to_local_blockchain(path_to_db, last_id2, (const char*) encrypt_message_part, hesh_before, hash_from_string_result);
							//printf("check_15:|\n");
							if (res_add != 0)
								{
									printf("%s\n", "При выполнении add_record_to_local_blockchain-функции произошла ошибка.");
									exit(1);
								}
							else
								{
									// Проверяем на заполнение глобальный блокчейн (свою локальную копию глобального блокчейна)
									
									//printf("%s\n", "При выполнении add_record_to_local_blockchain-функции ошибки не произошло.");
								}
     				
 							// Проанализировать нашу цепочку блокчайн
     							analyse_blockchain_for_input_messages(path_to_db);
     							check_clear_blockchain();
     							//printf("check_16:|\n");
						}	
						
				//printf("%s\n","Мое мнение совпало с мнением запрашивающего."); 
				flag = 1;
				break;    				
     			}
     		else 
     			{
     				//printf("check_17:|\n");
     				//printf("%s\n","ошибка ...");
     				if ((ID - 1) != my_DB_last_ID)
     					{
     						//printf("%s\n","ID ! = my_DB_last_ID"); 
     					}	
     				if (strcmp(HASH_of_before_ID, my_DB_last_ID_container_hash) != 0)
     					{
     						//printf("%s\n","HASH_of_before_ID ! = my_DB_last_ID_container_hash"); 
     					}
     					
     				count_hash++;	
     				// Переназначаем my_DB_last_ID и my_DB_last_ID_container_hash 
     					 my_DB_last_ID = my_DB_last_ID_arr[count_hash];
     					 my_DB_last_ID_container_hash = my_DB_last_ID_container_hash_arr[count_hash];
	
     			}	
     	}	

	//printf("check_18:|\n");

	// Проверяем нашлось ли совпадение
	if (flag == 0)
		{
			//printf("check_19:|\n");
			// Отправляем сообщение о том, что совпадений в последних 10-ти ячейках найдено не было
			
			// Добавление BEGIN -----------------------
     			int len_buf = 2;
     			unsigned char uc_int [4];
     			memset(uc_int,0,4);
     			int_to_unsigned_char_array_4 (len_buf, uc_int);
     			write(newsock, uc_int, 4);
     			// Добавление END -------------------------
			
					
			write(newsock, "NO", 2);
			
			// Не добавляем в свою копию локального блокчейн это сообщение
			//timeout(1); // Это чтобы другие члены сети успели обновить свои blockchain
			// Запрос на обновление своей копии blockchain
			
			//printf("%s\n","Совпадений id и hash не нашлось.");
		}
	else
		{
			//printf("check_20:|\n");
			// Добавляем в свою копию локального блокчейн это сообщение (запрос в БД делаем)
				// Дописываем в файл "blockchain.txt" то, что прислали - (ВМЕСТО ЭТОГО _ ЗАПРОС К БД)
				// Прогоняем через алгоритм проверки блокчейн на наличие входящих МНЕ сообщений
			//printf("%s%d\n","Совпадение ID и HASH было найдено на шаге: ", count_hash);
		}	

  			
  		// Закрываем соединение	
  		close(newsock);

  		//printf("%s\n","Виток участия во всеобщем алгоритме консенсуса завершен.");

	if (flag == 0)
		{
		

		/*

    		//printf("%s\n", "-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0--0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0");
    		//printf("%s\n", "Т.к большинство не согласно с вашей копией blockchain, то придется запросить actual_blockchain_copy.");
    		
    		// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети
		get_actual_blockchain_copy(port2);
	
		// Очищение старой копии BLOCKCHAIN_TABLE
		Clear_Blockchain_Table(PATH_Date_Base);
	
		// Заполнение БД из файла
		const char* path_to_buffer_file = "actual_blockchain.txt";
		blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
    		sleep(1);
    		//printf("%s\n", "-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0--0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0-0");
    		//printf("%s\n","\n\tВАШЕ СООБЩЕНИЕ НЕ ОТПРАВЛЕНО, НО БЛОКЧЕЙН ОБНОВЛЕН. ПОПЫТАЙТЕСЬ ЕЩЕ РАЗ.\n");
    	
		printf("%s\n","\n\tЗапрос глобального блокчейна в связи с тем, что дали ответ 'NO'.\n");

		*/

		}

	free(arg);

 // printf("%s","Очередной клиент обработан\n");   
  
  //pthread_mutex_unlock(&mutex1);
  
  //printf("check_21:|\n");

}



int check_id_hash_server_part(const char* path_to_db, int number_port)
{


write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"check_id_hash_server_part\"]\n");

// SEND_MESSAGE ----------------------

int sock, port;
int clen;     
struct sockaddr_in serv_addr, cli_addr_global;

//pthread_mutex_t mutex1;


// SEND_MESSAGE ----------------------


         // LINUX
	struct timeval tv;
	tv.tv_sec = 60; //200; //60; //timeout_in_seconds;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

     sock = socket(AF_INET, SOCK_STREAM, 0);
     if (sock < 0)
     	{
       	printf("socket() failed: %d\n", errno);
       	return EXIT_FAILURE;
     	}
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     port = number_port;
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(port);
     //printf("Send client listen port = %d\n",port);
     if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     	{
       	printf("bind() failed: %d\n", errno);
       	return EXIT_FAILURE;
     	}
     listen(sock, 1);
     clen = sizeof(cli_addr_global);
     
     while(1)
     	{
        	int id, result;
   		pthread_t thread1;
   		id = 1;
   		//printf("Wait clients ...\n");
   		//sleep(5);
     		int newsock = accept(sock, (struct sockaddr *) &cli_addr_global, &clen);
     		if (newsock < 0) 
     			{
       			printf("accept() failed: %d\n", errno);
       			return EXIT_FAILURE;
     			}

   		struct str_thread* struct1 = (struct str_thread*) malloc(sizeof(struct str_thread));
     		struct1->x = cli_addr_global;
     		struct1->y=newsock;
     		struct1->z=serv_addr;
     		struct1->path_to_db = path_to_db;
   	
   	
   	
   		result = pthread_create(&thread1, NULL, thread_func_send_message_server_part, struct1);
   		if (result != 0) 
   			{
     				perror("Creating the first thread");
     				return EXIT_FAILURE;
   			}
   		//printf("Create NEW THREAd!\n");
   		//sleep(10);
   	
     	}
     
     close(sock);
}


void * func_send_message_server_part_function(void * arg)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"func_send_message_server_part_function\"]\n");
	struct path_to_DB_and_port structure = *(struct path_to_DB_and_port*)arg;
	//printf("structure.port_2 = %d\n", structure.port);
	//sleep(5);
	check_id_hash_server_part(structure.path_to_db, structure.port);
}

/*
void run_send_message_server_part_function(int port_number)
{
	pthread_t thread_send_message;
	struct path_to_DB_and_port structure;
	structure.path_to_db = path_to_db;
	structure.port = port_number;
	int result = pthread_create(&thread_send_message, NULL, func_send_message_server_part_function, &structure);
   		if (result != 0) 
   			{
     				perror("Creating the run_send_message_server_part_function thread");
     				return EXIT_FAILURE;
   			}
}

*/


int get_hash_or_blockchain(const char* hostname, int port_number, const char* hash_or_blockchain, char* buffer) // В buffer будет записан результат, где buffer[BUF_SIZE]
{

    //printf("Отправляю запрос [%s] к IP: %s\n",hash_or_blockchain,hostname);
    write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"get_hash_or_blockchain\"]\n");
    int sock, port;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char hash[BUF_SIZE] = "get_last_id_hesh\n";
    char blockchain[BUF_SIZE] = "get_actual_blockchain\n";
    char buf[BUF_SIZE];

    port = port_number;
    int buf_size;
    if (strcmp(hash_or_blockchain,"hash") == 0)
    	{
    	
    		memcpy(buf, hash, sizeof hash);
    		//printf("%s\n","Запрос на получение last_id_hash");
    	}
    else if (strcmp(hash_or_blockchain,"blockchain") == 0)
    	{
    		memcpy(buf, blockchain, sizeof blockchain);
    		//printf("%s\n","Запрос на получение actual_blockchain");
    	}
    else 
    	{
    		int i = 0;
    		for (i;i<BUF_SIZE;i++){
    			if (hash_or_blockchain[i] == '\0')
    				{
    					break;
    				}
    			buf[i] = hash_or_blockchain[i];
    		}
    		buf[i] = '\0';
    		
    		
    		//memcpy(buf, hash_or_blockchain, sizeof blockchain);
    		//printf("%s%s%s\n","Попытка отправить сообщение: ", buf, "...");
    		//exit(1);
    	}		
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    	{
      		printf("socket() failed: %d", errno);
      		return EXIT_FAILURE;
    	}



	/*
    server = gethostbyname(hostname);
    if (server == NULL) 
    	{
      		printf("Host not found_1_trying one more _ 1 _...\n");
		sleep(4);
		server = gethostbyname(hostname);
		if (server == NULL) 
    		{
			printf("Host not found_1_trying one more _ 2 _...\n");
			sleep(4);
			server = gethostbyname(hostname);
			if (server == NULL) 
    			{	
				int errno_my = errno;
				int errno_my_h = h_errno;
				printf("Host not found_1_ALL\n");
				printf("%s%d\n","errno = ", errno_my);
				printf("%s%d\n","h_errno = ", errno_my_h);
				
				printf("%s%s\n","h_errno_str = ", hstrerror(errno_my_h));
				sleep(20);
				return EXIT_FAILURE;
			}
		}
      		
    	}
    */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);


    if (inet_aton(hostname, &serv_addr.sin_addr.s_addr) == 0) 
        {
        fprintf(stderr, "Invalid address\n");
        printf("hostname_addr: %s\n", hostname);
	sleep(2);
        exit(EXIT_FAILURE);
        }
   //server->h_addr = 4; // only for IPv4
/*
    printf("server->h_addr = %s\n",server->h_addr);
    printf("server->h_addr_NORM = %d\n", (int)server->h_addr[0]);
    printf("%d\n", (int)server->h_addr[1]);
    printf("%d\n", (int)server->h_addr[2]);
    printf("%d\n", (int)server->h_addr[3]);
    printf("hostname = %s\n",hostname);
    printf(" server->h_length = %d\n",  server->h_length);
    sleep(20);
*/

    serv_addr.sin_port = htons(port);
    
    
    
      
    
    if (connect(sock, &serv_addr, sizeof(serv_addr)) < 0) 
    	{
		int count_connect = 0;
		int MAX_COUNT_CONNECT = 1;
		while (connect(sock, &serv_addr, sizeof(serv_addr)) < 0)
		{
		if (count_connect >= MAX_COUNT_CONNECT)
		{




			// ------------- 03-09-21 --------------- BEGIN --------------

		
    					if (strcmp(hash_or_blockchain,"hash") == 0)
    						{
    							printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / hash, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_hash";
    							int recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
    						}
    					else if (strcmp(hash_or_blockchain,"blockchain") == 0)
    						{
    							printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / blockchain, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_blockchain";
    							int recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
    						}
    					else 
    						{
    							printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / message, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_message";
    							int recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
						}
    							



				// ------------- 03-09-21 --------------- END --------------



			//printf("connect() failed: %d\n", errno);
			//printf(" NO connectinon with another_client. Skip connection");
			//fflush(stdout);
			//return EXIT_FAILURE;
		}
		sleep(2); //5
		printf("%s%s ...\n","Trying connecting to another_client: ", hostname);
		fflush(stdout);
		count_connect++;
		}
		
    	}
    	
      // LINUX
	struct timeval tv;
	
	if (sending_file_flag == 1)
		tv.tv_sec = 200; //200; //60; //15; //timeout_in_seconds;
	else
		tv.tv_sec = 60;
		
	tv.tv_usec = 0;
	int res_sockopt = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    	int e = errno;
    //printf("Oh dear, something went wrong with read()! %s\n", strerror(e));
    //printf("res_sockopt = %d\n", res_sockopt);
    	
    //printf("connected to [%d]\n", port);	
    //sleep(10);	
    //printf(">");
    //memset(buf, 0, BUF_SIZE);
    //fgets(buf, BUF_SIZE-1, stdin);
    //memset(buf, 0, BUF_SIZE);
    //buf = "get_last_id_hesh\n";
    
    // Добавление BEGIN -----------------------
     int len_buf = strlen(buf);
     unsigned char uc_int [4];
     memset(uc_int,0,4);
     int_to_unsigned_char_array_4 (len_buf, uc_int);
     int wr_res = write(sock, uc_int, 4);
     //printf("wr_res = %d\n", wr_res);
     //sleep(5);
     // Добавление END -------------------------
    
    
    wr_res = write(sock, buf, strlen(buf));
    //printf("wr_res = %d\n", wr_res);
    //sleep(5);
    memset(buf, 0, BUF_SIZE);

    // Вот тут повесить наверное нужно timeout ???
    
    int recieve_bytes =  0;
    
    unsigned char buf_int [4];
    memset(buf_int,0,4);
    
    int total_recieve = 0;
    
    	while (total_recieve != 4) // 4 байт 
		{
			recieve_bytes = 0;
			recieve_bytes = read(sock, &buf_int[total_recieve], 4);
			int e = errno;
			//printf("recieve_bytes_1 = %d\n", recieve_bytes);
    			//printf("Oh dear, something went wrong with read()! %s\n", strerror(e));
    			
			
			//sleep(10);
			if (recieve_bytes <= 0)
    				{
    					if (strcmp(hash_or_blockchain,"hash") == 0)
    						{
    							printf("\n%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / hash, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_hash";
    							recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
    						}
    					else if (strcmp(hash_or_blockchain,"blockchain") == 0)
    						{
    							printf("\n%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / blockchain, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_blockchain";
    							recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
    						}
    					else 
    						{
    							printf("\n%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / message, IP-client: ", hostname);
    							fflush(stdout);
    							const char* buf_no = "No_response_message";
    							recieve_bytes = strlen (buf_no);
    							strncpy(buffer,buf_no,recieve_bytes);
    							close(sock);
    							return 0;
    						}			
    
    				}
			total_recieve = total_recieve + recieve_bytes;		
		} 
		
		
		  
    
    
    
    
    
    
    
    
    
    
    
    // ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- BEGIN --------------------    
    
    // Тут должно прийти 4 байта
    int wait_bayte = unsigned_char_array_4_to_int(buf_int);
    //printf("wait_bayte = %d\n", wait_bayte);
    total_recieve = 0;
    memset(buf, 0, BUF_SIZE);
    while(total_recieve != wait_bayte)
    	{
    		recieve_bytes = 0;
    		
    		recieve_bytes = read (sock, &buf[total_recieve], BUF_SIZE-1);
    		int e = errno;
		//printf("recieve_bytes_2 = %d\n", recieve_bytes);
    		//printf("Oh dear, something went wrong with read()! %s\n", strerror(e));
    		
		//sleep(10);
    		if (recieve_bytes <=0)
    			{
    				    
    				if (strcmp(hash_or_blockchain,"hash") == 0)
    					{
    						printf("%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / hash");
    						fflush(stdout);
    						const char* buf_no = "No_response_hash";
    						recieve_bytes = strlen (buf_no);
    						strncpy(buffer,buf_no,recieve_bytes);
    						close(sock);
    						return 0;
    					}
    				else if (strcmp(hash_or_blockchain,"blockchain") == 0)
    					{
    						printf("%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / blockchain");
    						fflush(stdout);
    						const char* buf_no = "No_response_blockchain";
    						recieve_bytes = strlen (buf_no);
    						strncpy(buffer,buf_no,recieve_bytes);
    						close(sock);
    						return 0;
    					}
    				else 
    					{
    						printf("%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / message");
    						fflush(stdout);
    						const char* buf_no = "No_response_message";
    						recieve_bytes = strlen (buf_no);
    						strncpy(buffer,buf_no,recieve_bytes);
    						close(sock);
    						return 0;
    					}
    
   			}
   		
   		total_recieve = total_recieve + recieve_bytes;	
    	}
    
    
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- END --------------------
    
    
    
    
    
    
    
    
    
    
    // Пока что уберем запись в файлы "blockchain_recieve.txt" и "Last_ID_container_HASH.txt" (за практической ненадобностью)
    /*
    FILE* file; 
    
    if (strcmp(hash_or_blockchain,"blockchain") == 0)
    	{}
      file = fopen("blockchain_recieve.txt","wb");
    else file = fopen("Last_ID_container_HASH.txt","wb");
    int real_write_byte = fwrite(buf,1,recieve_bytes,file);
    if (real_write_byte != recieve_bytes)
    	{
    		printf("%s","При записи принятых данных в файл произошла ошибка.\n");
    	}
    else
    	{
    	//	printf("%s","При записи принятых данных ошибок не обнаружено.\n");	
    	}	
    
    fclose(file);
    
    */
    
    //printf("%s%s\n","Last ID-conteiner HASH: ",buf);
    //printf("total_recieve = %d\n", total_recieve);
    strncpy(buffer, buf, total_recieve);  //recieve_bytes);
    //sleep(1);
   // printf("%sПринятый buffer = ",buf);
    close(sock);
    return 0;
}

//ATTRIBUTE_NO_SANITIZE_ADDRESS
void * thread_func_hash_and_blockchain_server_part(void * arg)
{

     write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"thread_func_hash_and_blockchain_server_part\"]\n");
     char buf[BUF_SIZE];
     struct sockaddr_in cli_addr;     
     struct str_thread structure;
     structure = *(struct str_thread*) arg;   
     cli_addr = structure.x;
     int newsock = structure.y;
     const char* path_to_db = structure.path_to_db;
     memset(buf, 0, BUF_SIZE);
     
     int recieve_bytes = 0;  
     unsigned char buf_int[4];
     memset(buf_int,0,4);
     int total_recieve = 0;
     
     	while (total_recieve != 4) // 4 байт 
		{
			recieve_bytes = 0;
			recieve_bytes = read(newsock, &buf_int[total_recieve], 4);
			if (recieve_bytes <= 0)
				{
					char str[INET_ADDRSTRLEN];
     					// Where we sent hello-message ?
    		 			inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     					//printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     					printf("%s%s%s\n","[thread_func_hash_and_blockchain_server_part]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    					close(newsock);
    					return;
				}
			total_recieve = total_recieve + recieve_bytes;		
		}  

     	
     	
     	
     	
     	
     	
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- BEGIN --------------------    
    
    // Тут должно прийти 4 байта
    int wait_bayte = unsigned_char_array_4_to_int(buf_int);
    total_recieve = 0;
    
    while(total_recieve != wait_bayte)
    	{
    		recieve_bytes = 0;
    		memset(buf, 0, BUF_SIZE);
    		recieve_bytes = read (newsock, &buf[total_recieve], BUF_SIZE-1);
    		if (recieve_bytes <=0)
    			{
    				char str[INET_ADDRSTRLEN];
     				// Where we sent hello-message ?
    				inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     				//printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     				printf("%s%s%s\n","[thread_func_hash_and_blockchain_server_part]. Не дождался ответа от клиента: ", str, " .Соединение с ним будет закрыто.");
    				close(newsock);
    				return;
    
   			}
   		
   		total_recieve = total_recieve + recieve_bytes;	
    	}
    
    
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- END --------------------
     	
     	
     	
     	
     	
     	
     	
     	
     	
     	
     	
     	
     
     buf[BUF_SIZE-1] = 0;
    // printf("MSG: %s|\n", buf);
    
     // Если пришло сообщение-приветсвие (пользователь извещает о том, что он доступен)
     if (strcmp(buf,"get_last_id_hesh\n") == 0)
     	{ 
     	
     //	pthread_mutex_lock(&mutex);
     		
     		// printf("path_to_db = %s|\n",path_to_db);
     		
     		// Функция получения номера последнего id
     		char last_id[BUF_SIZE];
     		memset(last_id, 0, BUF_SIZE);
     		get_how_much_id (path_to_db, last_id);
     		const char* last_id_str = last_id;
     		
     		//printf("last_id_str = %s|\n",last_id_str);
     		
     		// Функция получения хэша последнего id-контейнера
     		char hash_last_id_container[HEX_LENGTH];
     		memset(hash_last_id_container, 0, HEX_LENGTH);
     		get_hash_id_container (path_to_db, last_id_str, hash_last_id_container);
     		
     		
     		/*
     		char hash_last_id_container [HEX_LENGTH];
     		int res_get = get_hash_last_id_container(hash_last_id_container);
     		if (res_get != 0)
     			{
     				printf("%s\n","Выполнение get_hash_last_id_container-функции завершилось с ошибкой.");
     				exit(1);
     			}
     		else
     			{
     				printf("%s\n","Выполнение get_hash_last_id_container-функции завершилось без ошибки.");
     			}	
     			
     			
     		*/
     		
     			
     //	pthread_mutex_unlock(&mutex);
     	
     		//printf("Результат get_hash_last_id_container() = %s\n", hash_last_id_container);
     	
     	
     		// Добавление BEGIN -----------------------
     		int len_buf = strlen(hash_last_id_container);
     		unsigned char uc_int [4];
    		memset(uc_int,0,4);
     		int_to_unsigned_char_array_4 (len_buf, uc_int);
     		write(newsock, uc_int, 4);
     		// Добавление END -------------------------
     		write(newsock, hash_last_id_container,strlen(hash_last_id_container));
     		// INET_ADDRSTRLEN - это константа одного из заголовочных файлов
     		char str[INET_ADDRSTRLEN];
     		// Where we sent hello-message ?
    		 inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);
     		//printf("%s%s, port=%d\n","Hwo request HASH of last id-container: ", str, ntohs(cli_addr.sin_port) );
     		close(newsock);
     		}
  
  // Если пришло сообщение с копии блокчейн-цепочки
  else if (strcmp(buf,"get_actual_blockchain\n") == 0)
  	{
  	
  	// Считываем файл-блокчейн-цепочку в буффер
  	
  	  		// Узнаем размер файла
  		long size;
  		
  		const char* filename = "blockchain.txt";
  		
  		//printf("%s|\n", "CHECKIIING 101-check...");
  		
  		// Это нужно лишь для совместимости
  		char big_buffer[MAX_TRANSACTION_LENGTH];
     		memset(big_buffer, 0, MAX_TRANSACTION_LENGTH);
  		int ssize =blockchain_from_DB_to_file_c (path_to_db, 1, filename, big_buffer);
  		
  		
  		//printf("%s|\n", "CHECKIIING 102-check...");
  		
  		
        	size = getFileSize(filename);
        	//printf("Размер файла filename равен %d\n", size);
 
  		// Считаем его в массив байт
  		FILE* file;
  		file=fopen (filename,"rb");
  		if (file != NULL)
  			{
  				char byte_arr[size]; 
  				int real_read_size = fread(byte_arr, 1, size, file);
  				if (real_read_size != size)
  					{
  						//printf("%s","Считывание из файла завершилось неудачно!\n");
  					}
  				else 
  					{
  						//printf("%s","Считывание из файла завершилось удачно!\n");
  					}
  	
  				fclose(file);
  				// Отправляем клиенту
  				
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
  				//printf("%s","Невозможно открыть файл для передачи на запрос клиента.\n");
  				const char* error_string = "Error_reading_list_of_user. Try more time";
  				// Добавление BEGIN -----------------------
     				int len_buf = strlen(error_string);
     				unsigned char uc_int [4];
    				memset(uc_int,0,4);
     				int_to_unsigned_char_array_4 (len_buf, uc_int);
     				write(newsock, uc_int, 4);
     				// Добавление END -------------------------
     				
  				write(newsock, error_string, strlen(error_string));
  			}
  			
  		// Закрываем соединение	
  		close(newsock);
  		
  		//printf("%s|\n", "CHECKIIING 103-check...");
  		
  		
  		if(remove(filename)) 
  			{
               		printf("Error removing path_to_buffer_file");
                		exit(1);
                	}
  		
  		//printf("%s: %s\n","Вот эта актуальная копия блокчейн-цепочки отправлена клиенту.", buf);
  	}
  	
  else	{	
  		close(newsock);
  		//printf("%s: %s\n","Пришло не понятное сообщение", buf);
  	}

 free(arg);
  //printf("%s","Очередной клиент обработан\n");   
}



void * hash_and_blockchain_server_part_function(void * arg)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"hash_and_blockchain_server_part_function\"]\n");
	struct port_and_path_to_DB struct1 = *(struct port_and_path_to_DB*) arg;

	int number_port = struct1.port;
	const char* path_to_db = struct1.path_to_db;
	//printf("ПЕРЕДАННЫЙ path_to_db = %s|\n", path_to_db);
	hash_and_blockchain_server_part(number_port, path_to_db);
}



void * periodically_get_actually_blockchain_copy_function(void * arg)
{
	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"periodically_get_actually_blockchain_copy_function\"]\n");
	struct port_and_path_to_DB struct1 = *(struct port_and_path_to_DB*) arg;

	int number_port = struct1.port;
	const char* path_to_db = struct1.path_to_db;
	//printf("ПЕРЕДАННЫЙ path_to_db = %s|\n", path_to_db);
	periodically_get_actually_blockchain_copy(number_port, path_to_db);
}

int periodically_get_actually_blockchain_copy(int number_port, const char* path_to_db)
{
    	
	while (1)
	{
		/*
		sleep(600); // 600 сек (10 минут) - периодичность обновления локального блокчейна
		set_cursor_position (start_line + 1, 4);
    		start_line++;
    		// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети
		get_actual_blockchain_copy(number_port);
	
		// Очищение старой копии BLOCKCHAIN_TABLE
		Clear_Blockchain_Table(path_to_db);
	
		// Заполнение БД из файла
		const char* path_to_buffer_file = "actual_blockchain.txt";
		blockchain_from_file_to_DB (path_to_db, path_to_buffer_file);
		analyse_blockchain_for_input_messages(path_to_db);
    		sleep(1);
		printf("%s\n","\n\tШтатное обновление локального блокчейна произведено успешно.\n");
		*/
	}

	return 0;
}


// Эту функцию необходимо в потоке один раз запустить

int hash_and_blockchain_server_part(int number_port, const char* path_to_db)
{
	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"hash_and_blockchain_server_part\"]\n");
	// GET_LAST_ID_CONTAINER_HASH ----------------------

	int sock, port;
	int clen;     
	struct sockaddr_in serv_addr, cli_addr_global;


	pthread_mutex_t mutex;

	// GET_LAST_ID_CONTAINER_HASH ----------------------




     //printf("%s","hash_and_blockchain_server_part - ЗАПУЩЕН");

    // LINUX
	struct timeval tv;
	tv.tv_sec = 60; //200;//60; //timeout_in_seconds;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);


     sock = socket(AF_INET, SOCK_STREAM, 0);
     if (sock < 0)
     	{
       	printf("socket() failed: %d\n", errno);
       	return EXIT_FAILURE;
     	}
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     port = number_port;
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

   		struct str_thread* struct1 = (struct str_thread*) malloc(sizeof(struct str_thread));
     		struct1->x = cli_addr_global;
     		struct1->y=newsock;
     		struct1->path_to_db=path_to_db;
     		
     		//printf("ПЕРЕДАННЫЙ path_to_db = %s|\n", path_to_db);
   	
   		result = pthread_create(&thread1, NULL, thread_func_hash_and_blockchain_server_part, struct1);
   		if (result != 0) 
   			{
     				perror("Creating the first thread");
     				return EXIT_FAILURE;
   			}
   	
     	}
     
     close(sock);
}

void * func_say_hello_and_get_user_list_function(void * arg)
{

	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"func_say_hello_and_get_user_list_function\"]\n");
	struct IP_and_PORT struct_ip_port = *(struct IP_and_PORT*)arg;
	while(1)
		{
		
			if (periodically_send_hello_message == 0)
			{
				sleep(4); //4  //60
				//delay(100);
				//printf("%s\n","la1");
				//printf("hostname = %s\n",struct_ip_port.ip);
				//printf("port = %d\n",struct_ip_port.port);
				int res = say_hello_and_get_user_list(struct_ip_port.ip,struct_ip_port.port, "hi\n");
				if (res == EXIT_FAILURE)
				{
					continue;
				}
				say_hello_and_get_user_list(struct_ip_port.ip,struct_ip_port.port, "list\n");
				
				// Эту строку необходимо комментировать/раскомментировать если хотим 1 раз/всегда отправлять hello-message и обновлять лист активных пользователей. 
				periodically_send_hello_message = 1;
				
				//sleep_for(10);
			 }
			
		}
	
}


// На данный момент уже не используется
int say_hello_and_get_user_list(const char* hostname, int port_number, const char* request_string)
{


	write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"say_hello_and_get_user_list\"]\n");
    if (strcmp(request_string,"hi\n") == 0)
    	{}
    	//printf("%s: %s\n","Отправка приветственного сообщения на сервер. ", request_string);
    else if (strcmp(request_string,"list\n") == 0)
    	{}
    	//printf("%s: %s\n","Запрос доступных пользователей у сервера ", request_string);
    else
    	{
    		printf("%s%s\n","Ошибка.Недопустимый параметр <request_string> = ", request_string);
    		exit(1);
    	}	



    int sock, port;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int i = 0;
    for (i;i<BUF_SIZE;i++)
    	{
    		if (request_string[i] == '\0')
    			{
    				break;
    			}
    	}
    
    strncpy(buf,request_string,i);

    port = port_number;
    

    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    	{
      		printf("socket() failed: %d", errno);
      		return EXIT_FAILURE;
    	}

    /*
    server = gethostbyname(hostname);
    if (server == NULL) 
    	{
      		printf("Host not found_2_trying one more _ 1 _...\n");
		sleep(4);
		server = gethostbyname(hostname);
		if (server == NULL) 
    		{
			printf("Host not found_2_trying one more _ 2 _...\n");
			sleep(4);
			server = gethostbyname(hostname);
			if (server == NULL) 
    			{
				int errno_my = errno;
				int errno_my_h = h_errno;
				printf("Host not found_2_ALL\n");
				printf("%s%d\n","errno = ", errno_my);
				printf("%s%d\n","h_errno = ", errno_my_h);
				
				printf("%s%s\n","h_errno_str = ", hstrerror(errno_my_h));
				sleep(20);
				return EXIT_FAILURE;
			}
		}
      		
    	}

	*/

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    if (inet_aton(hostname, &serv_addr.sin_addr.s_addr) == 0) 
        {
        fprintf(stderr, "Invalid address\n");
        printf("hostname_addr: %s\n", hostname);
	sleep(2);
        exit(EXIT_FAILURE);
        }

    //server->h_addr = 4; // only for IPv4

    //memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    
        
    
    if (connect(sock, &serv_addr, sizeof(serv_addr)) < 0) 
    	{
    		int count_trying =0;
		while ((connect(sock, &serv_addr, sizeof(serv_addr)) < 0) && (count_trying < 3))
		{
			sleep(5);
			printf("Trying connecting to server ...");
			fflush(stdout);
			count_trying++;
		}
		if (count_trying == 3)
		{
			printf("Trying connecting to server finished FAILD.");
			printf("Next connection will start after 45 sec.");
			return EXIT_FAILURE;
		}
		else
		{
			printf("Trying connecting to server finished SUCCESFULL");
		}
		
    	}
    
          // LINUX
	struct timeval tv;
	tv.tv_sec = 60; //200; //60; //timeout_in_seconds;
	tv.tv_usec = 0;
	int res_sockopt = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	int e = errno;
    printf("Oh dear, something went wrong with read()! %s\n", strerror(e));
    printf("res_sockopt = %d\n", res_sockopt);
    
    
    // Добавление BEGIN -----------------------
    int len_buf = strlen(buf);
    unsigned char uc_int [4];
    memset(uc_int,0,4);
    int_to_unsigned_char_array_4 (len_buf, uc_int);
    write(sock, uc_int, 4);
    // Добавление END -------------------------
        
    //printf("Отправляю len_buf = %d|\n",len_buf); 
        
    write(sock, buf, strlen(buf));
    memset(buf, 0, BUF_SIZE);
    int recieve_bytes = 0;
    int total_recieve = 0;
    unsigned char buf_int[4];
    memset(buf_int,0,4);
    
    while (total_recieve != 4) // 4 байт 
		{
			recieve_bytes = 0;
			recieve_bytes = read(sock, &buf_int[total_recieve], 4);
			if (recieve_bytes <=0)
    				{
    					printf("%s\n","Не дождался ответа от сервера.");
    					close(sock);
    					return 0;
    
    				}
			total_recieve = total_recieve + recieve_bytes;		
		} 


// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- BEGIN --------------------    
    
    // Тут должно прийти 4 байта
    int wait_bayte = unsigned_char_array_4_to_int(buf_int);
    //printf("Принял ответ от сервера = %d|\n",wait_bayte);
    total_recieve = 0;
    
    while(total_recieve != wait_bayte)
    	{
    		recieve_bytes = 0;
    		memset(buf, 0, BUF_SIZE);
    		recieve_bytes = read (sock, &buf[total_recieve], BUF_SIZE-1);
    		if (recieve_bytes <=0)
    			{
    				printf("%s\n","Не дождался ответа от сервера.");
    				close(sock);
    				return 0;
    
   			}
   		
   		total_recieve = total_recieve + recieve_bytes;	
    	}
    
    
// ------------------------------ Добавленный алгоритм приема сообщений (более безопасный) -------- END --------------------    
    
    
    



    pthread_mutex_lock(&mutex1);
    FILE* file;   
    file = fopen("user_list_client.txt","wb");
    int real_write_byte = fwrite(buf,1,recieve_bytes,file);
    if (real_write_byte != recieve_bytes)
    	{
    		printf("%s","При записи принятых данных в файл произошла ошибка.\n");
    	}
    else
    	{
    	//	printf("%s","При записи принятых данных ошибок не обнаружено.\n");	
    	}	
    
    //printf("buf = %s\n",buf);

    fclose(file);
    pthread_mutex_unlock(&mutex1);
    
 

    if (strcmp(request_string,"hi\n") == 0)
    	{
    		memset(my_global_ip,0,BUF_SIZE);
    		strncpy(my_global_ip,buf,recieve_bytes);
    		//printf("%s: %s|\n","Ответ сервера. Your_Global_IP", buf);
    	}
    	
    	
    	// Записываем его в определенную переменную
    	
    else if (strcmp(request_string,"list\n") == 0)
    	{}
    	//printf("%s:%s\n","Доступные пользователи ",buf);	
    	
    close(sock);
    
      
    return 0;
}





