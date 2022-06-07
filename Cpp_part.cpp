#include "Cpp_part.h"
#include "base64.h"

extern "C" {
#include "C_part.h"
}

#if defined(__clang__) || defined (__GNUC__)
# define ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#else
# define ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

extern const char* PATH_Date_Base;
extern std::string MY_NAME;
extern std::string my_public_dest_hash_and_blockchain; 
extern std::string my_public_send_message; 
extern int start_line;
extern int port2;
extern const char* LOG_FILE_NAME;
pthread_mutex_t mutex;
pthread_mutex_t mutex1;
extern int limit_number_of_transaction;
const int PUBLIC_KEY_LENGTH = 550; //516;
const int MAX_count_of_users = 2;
extern std::shared_ptr<SAM::StreamSession> session_ptr_hash_blockchain;
extern std::shared_ptr<SAM::StreamSession> session_ptr_send_message;
extern SAM::I2pSocket* socket_send_message[MAX_count_of_users];
extern SAM::I2pSocket* socket_hash_and_blockchain[MAX_count_of_users];


std::string get_path()
{
        char arg1[20];
        char exepath[PATH_MAX + 1] = {0};

        sprintf( arg1, "/proc/%d/exe", getpid() );
        readlink( arg1, exepath, 1024 );
        return std::string(exepath);
}

int bin_file_to_string(const char* filename, std::string* string_file)
{
    int file_size = filesize(filename);
    unsigned char buffer [file_size];
    memset(buffer,0,file_size);
    read_file_into_memory(filename, buffer, file_size);
    std::vector<BYTE> myData;
    for(int i=0; i<file_size; ++i)
        myData.push_back(buffer[i]);
    std::string encodedData = base64_encode(&myData[0], myData.size());
    *string_file = encodedData;
    return 1;
}

int string_to_bin_file(std::string* string_file, const char* filename)
{
    std::string encodedData = *string_file;
    std::vector<BYTE> decodedData = base64_decode(encodedData);
    FILE *mf;
    mf=fopen (filename,"wb");
    if (mf == NULL) 
    {
        printf ("ошибка открытия файла: %s", filename);
        exit(1);
    }
    else 
    {
    }
    fwrite(decodedData.data(), 1, decodedData.size(), mf);
    fclose (mf);
    return 1;
}

Timer_Callback::Timer_Callback(SAM::I2pSocket* socket_ptr, int* int_ptr, int time_delay_sec)
	: socket_ptr(socket_ptr), int_ptr(int_ptr), time_delay_sec(time_delay_sec), flag(0)
{
	printf("Timer создан.");
	fflush(stdout);
	this->thread_func();
}

void Timer_Callback::thread_func(void) 
{
	int step = 0;
	while (step != this->time_delay_sec)
	{
		sleep(1);
		printf("[%d] *(this->int_ptr) = %d\n", step, *(this->int_ptr));
		if (*(this->int_ptr) == INT_SIZE)
		{
			this->flag = 1;
			break;
		}
		step++;	
	}
	checkRes();
}

void Timer_Callback::checkRes(void)
{
if (flag == 0)
	{
		printf("Не дождался ответа от реального i2p-сервера.");
		printf("Закрытие i2p-сокета...");
		fflush(stdout);
		sleep(5);
		socket_ptr->release();
		socket_ptr->close();
		socket_ptr = nullptr;
		printf("i2p-сокет закрыт.");
		fflush(stdout);
	}
	else
	{
		printf("i2p-сервер ДОСТУПЕН.");
		fflush(stdout);
	}
}

Timer_Callback::~Timer_Callback()
{
	printf("Timer уничтожен.");
	fflush(stdout);
}

std::string random_str(int size)
{
    srand (time(NULL));
    std::string random_str = "";
    for (int i=0; i < size; i++)
    {
        int rnd = rand() % 9 + 1;
        random_str += std::to_string(rnd);
    }
    return random_str;
}

int read_from_file_to_buffer_1(const char* filename, char* buffer, int max_data_size)
      {
        FILE* fd;
        int  numread, count;
        if (( fd = fopen(filename, "rb")) == NULL)
          {
            perror("open failed on file file.dat");
            exit(1);
          }
        
        for (count = 0; count < max_data_size; count++)
             buffer[count]='\0';
             
        numread = fread(buffer, 1, max_data_size, fd);
        //printf("/n Число считанных литер %d\n",numread);
        fclose(fd);
        return numread;
      }

void * hash_and_blockchain_server_part_function_i2pd(void * arg)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"hash_and_blockchain_server_part_function_i2pd\"]\n");
    struct i2p_session_str structure;
    structure = *(i2p_session_str*) arg;
    std::shared_ptr<SAM::StreamSession> session = structure.session_ptr;
    int number = structure.number;
    //printf("threads_hash_blockchain[%d] have been started!\n", number);
    while (true) 
    {
        SAM::RequestResult<std::unique_ptr<SAM::I2pSocket> > session_accept;
        SAM::I2pSocket* socket;
        session_accept = session->accept(false);
            if (!session_accept.isOk)
        {
                printf("session_accept[%d] failed! Trying more time...\n", number);
            session_accept = session->accept(false);
            if (!session_accept.isOk)
            {
                printf("session_accept[%d] failed! \n", number);
                continue;
            }
        }
        socket = session_accept.value.get();
        socket_hash_and_blockchain[number] = socket;
        unsigned char another_dest_ch [PUBLIC_KEY_LENGTH];
        int recieve_len = 0;
        while (true)
        	{
        		unsigned char ch[1];
        		memset(ch,0,1);
        		int one = socket->read(1, &ch[0]);
        		if (ch[0] == '\n')
        		{
        			another_dest_ch[recieve_len] = ch[0];
        			recieve_len++;
        			break;
        		}
        		another_dest_ch[recieve_len] = ch[0];
        		recieve_len++;	
        	}
        	recieve_len++;
			another_dest_ch[recieve_len] = '\0';        	
        std::string another_dest_ (const_cast<const char*>(reinterpret_cast<char*>(another_dest_ch)));
        const std::string& another_dest = another_dest_;
        unsigned char buf_int [INT_SIZE];
        memset(buf_int,0,INT_SIZE);
        int res_rec = 0;
        while (res_rec != INT_SIZE)
        {
            int res = socket->read(INT_SIZE, buf_int);
            res_rec += res;
        }
        int data_size = unsigned_char_array_4_to_int(buf_int);
        unsigned char indata[data_size];
        memset(indata,0,data_size);
        int recieved_total = 0;
        int inverse_size_have = data_size;
        std::string filename_data_str = "from_client_hash_blockchain_" + std::to_string(number) + ".txt";
        const char* filename_data = filename_data_str.c_str();
        pthread_mutex_lock(&mutex);
        FILE *mf;
        mf=fopen (filename_data,"w");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
        }
        while (recieved_total != data_size)
        {
            int recv_bytes = socket->read(512, &indata[recieved_total]);
            printf("recv_bytes = %d\n", recv_bytes);
            printf("recieved_total = |%d|\n", recieved_total);
            fflush(stdout);
            fwrite(&indata[recieved_total], 1, recv_bytes, mf);
            printf("chacking...");
            recieved_total = recieved_total + recv_bytes;
            inverse_size_have = inverse_size_have - recv_bytes;
            printf("recieved_total = %d\n", recieved_total);
            if ((inverse_size_have < 512) && (inverse_size_have > 0))
            {
                int recv_bytes_last = socket->read(inverse_size_have, &indata[recieved_total]);
                if (recv_bytes_last != inverse_size_have)
                {
                    printf("Error_receive_data[%d] !\n", number);
                    exit(1);
                }
                fwrite(reinterpret_cast<unsigned char*>(&indata[recieved_total]), 1, recv_bytes_last, mf);
                inverse_size_have = inverse_size_have - recv_bytes_last;
                recieved_total = recieved_total + recv_bytes_last;
                printf("recv_bytes_last = %d\n", recv_bytes_last);
                printf("recieved_total = %d\n", recieved_total);
                break;
            }
        }
        fclose (mf);
        char buffer_data_real [data_size+1];
        memset(buffer_data_real,0,data_size+1);
        mf=fopen (filename_data,"r");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
        }
        int result = fread (buffer_data_real,1,data_size,mf);
        if (result != data_size) 
        {
            fputs ("Reading error",stderr); exit (3);
        }
        fclose (mf);
        pthread_mutex_unlock(&mutex);
     if (strcmp(buffer_data_real,"get_last_id_hesh\n") == 0)
        { 
            char last_id[BUF_SIZE];
            memset(last_id, 0, BUF_SIZE);
            get_how_much_id (PATH_Date_Base, last_id);
            const char* last_id_str = last_id;
            char hash_last_id_container[HEX_LENGTH];
            memset(hash_last_id_container, 0, HEX_LENGTH);
            get_hash_id_container (PATH_Date_Base, last_id_str, hash_last_id_container);
            int len_buf = strlen(hash_last_id_container);
            unsigned char uc_int [4];
            memset(uc_int,0,4);
            int_to_unsigned_char_array_4 (len_buf, uc_int);
            socket->write(uc_int, INT_SIZE);
            printf("write len_buf = |%d| ...\n", len_buf);
            socket->write(reinterpret_cast<unsigned char*>(hash_last_id_container), strlen(hash_last_id_container));
            socket->close();
            sleep(5);
        }

    else if (strcmp(buffer_data_real,"get_actual_blockchain\n") == 0)
    {
        long size;
        const char* filename = "blockchain.txt";
        char big_buffer[MAX_TRANSACTION_LENGTH];
        memset(big_buffer, 0, MAX_TRANSACTION_LENGTH);
        int ssize =blockchain_from_DB_to_file_c (PATH_Date_Base, 1, filename, big_buffer);
        size = getFileSize(filename);
        FILE* file;
        file=fopen (filename,"rb");
        if (file != NULL)
            {
                char byte_arr[size]; 
                int real_read_size = fread(byte_arr, 1, size, file);
                if (real_read_size != size)
                    {
                    }
                else 
                    {
                    }
                fclose(file);
                    int len_buf = real_read_size;
                    unsigned char uc_int [4];
                    memset(uc_int,0,4);
                    int_to_unsigned_char_array_4 (len_buf, uc_int);
                    socket->write(uc_int, INT_SIZE);
                    socket->write(reinterpret_cast<unsigned char*>(byte_arr), real_read_size);
            }
        else
            {
                const char* error_string = "Error_reading_list_of_user. Try more time";
                int len_buf = strlen(error_string);
                unsigned char uc_int [4];
                memset(uc_int,0,4);
                int_to_unsigned_char_array_4 (len_buf, uc_int);
                socket->write(uc_int, INT_SIZE);
                socket->write(reinterpret_cast<unsigned char*>(const_cast<char*>(error_string)), strlen(error_string));
                    
            }
        socket->close();
        sleep(5);
        if(remove(filename)) 
            {
                    printf("Error removing path_to_buffer_file");
                    exit(1);
            }
    }
    else    
    {   
        socket->close();
        sleep(5);
    }
    }
}

int send_message_i2pd (const char* encrypt_message, const char* path_to_buf_file)
{  
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"send_message_i2pd\"]\n");
    char I2P[MAX][MAX_DESTINATION_LENGTH];
    char ANSWER[MAX][ANSWER_LENGTH];
    int i = 0;
   for (i;i<MAX;i++)
    {
        memset(I2P[i], 0, MAX_DESTINATION_LENGTH);
        memset(ANSWER[i], 0, ANSWER_LENGTH);
    }
   FILE *mf;
   char str[1000];
   memset(str,0,1000);
   char *estr;
   pthread_mutex_lock(&mutex1);
   mf = fopen ("Keys/user_list_client_i2pd_2.txt","r");
   if (mf == NULL) {
    return -1;
   }
   else 
   {}
   int count_I2P = 0;
   while (1)
   {
      estr = fgets (str,sizeof(str),mf);
      if (estr == NULL)
      {
         if ( feof (mf) != 0)
         {  
            break;
         }
         else
         {
            break;
         }
      }
      strncpy (I2P[count_I2P], str, strlen(str));
      int size_I2P_str = strlen(str);
      I2P[count_I2P][size_I2P_str] = '\0';
      I2P[count_I2P][size_I2P_str-1] = '\0';
      count_I2P++;   
   }
   if ( fclose (mf) == EOF)
   {
    printf ("ошибка\n");
    exit(1);
    }
   else 
   {
   }
    pthread_mutex_unlock(&mutex1);
    char last_id_c[BUF_SIZE];
    memset(last_id_c, 0, BUF_SIZE);
    get_how_much_id (PATH_Date_Base,last_id_c);
    int current_last_id = atoi((const char*)last_id_c);
    int new_last_id = current_last_id + 1;
    int length = snprintf( NULL, 0, "%d", new_last_id );
    char last_id_str [length + 1];
    snprintf(last_id_str, length + 1, "%d", new_last_id );
    last_id_str[length] ='\0';
    const char* last_id_str_const = (const char*)last_id_str;
    int length1 = snprintf( NULL, 0, "%d", current_last_id );
    char last_id_str1 [length1 + 1];
    snprintf(last_id_str1, length + 1, "%d", current_last_id );
    last_id_str1[length1] ='\0';
    const char* last_id_str_const1 = (const char*)last_id_str1;
    char last_id_container_hash[HEX_LENGTH + 1];
    memset(last_id_container_hash, 0, HEX_LENGTH + 1);
    get_hash_id_container (PATH_Date_Base, last_id_str_const1, last_id_container_hash);
    last_id_str[length] =';';
    last_id_container_hash[HEX_LENGTH] = '\0';
    int len = strlen(encrypt_message);
    char enc_mes[len];
    memset(enc_mes,0,len);
    int p = 0;
    for (p;p<len;p++)
        {
            if (encrypt_message[p] != '\0')
                {
                    enc_mes[p] = encrypt_message[p];
                }
            else break; 
        }
    
    enc_mes[p] = '\0';
   int j = 0;
   for (j; j < count_I2P; j++)
    {
        int strlen_1 = strlen(enc_mes) + 1; // без +1
        char encrypt_message_part[strlen_1];
        memset(encrypt_message_part,0,strlen_1);
        int pp = 0;
        for (pp;pp<strlen_1;pp++)
            {
                if (enc_mes[pp] != '\0')
                    {
                        encrypt_message_part[pp] = enc_mes[pp];
                    }
                else break;            
            }
        encrypt_message_part[pp] = '\0';
        int length = snprintf( NULL, 0, "%d", new_last_id );
        char str4 [length + 1];
        memset(str4,0,length + 1);
        snprintf( str4, length + 1, "%d", new_last_id );
        str4[length] = '\0';   
        int id_hash_length = strlen(str4) + strlen(last_id_container_hash) +2 ; // +2- это для ';'-того, что между %s и %s '\0'-символа конца строки
        char id_hash [id_hash_length];
        snprintf(id_hash, sizeof id_hash, "%s;%s", str4, last_id_container_hash);
        int request_size_full_2 = strlen(id_hash) + strlen(encrypt_message_part) + 2; // +2 - это для '\0'-символа конца строки
        char get_hash_programm_name_full_2 [request_size_full_2];
        memset(get_hash_programm_name_full_2,0,request_size_full_2);
        snprintf(get_hash_programm_name_full_2, sizeof get_hash_programm_name_full_2, "%s;%s", id_hash, encrypt_message_part);
        std::string pd;
        {
            // Считываем два адреса в соответствии с именем вводимым
            std::string path = "Keys/" + MY_NAME + "/PUBLIC_send_message.txt";
            const char* desinatin_key_file = path.c_str();
            char buffer[MAX_DESTINATION_LENGTH];
            memset(buffer, 0, MAX_DESTINATION_LENGTH);
            int size = read_from_file_to_buffer_1(desinatin_key_file, buffer, MAX_DESTINATION_LENGTH);
            buffer[size-1] = '\0';
            std::string mydest_buf(buffer);
            my_public_send_message = mydest_buf;
            pd = mydest_buf;
        }
         if (strcmp(I2P[j],pd.c_str()) == 0)
            {
                strncpy (ANSWER[j], get_hash_programm_name_full_2, request_size_full_2);
                ANSWER[j][request_size_full_2]='\0';
                continue;               
            }
        char res_buf [ANSWER_LENGTH];
        int c = 0;
        for (c; c < ANSWER_LENGTH; c++)
            res_buf[c] = '\0';
        get_hash_or_blockchain_i2pd(I2P[j], get_hash_programm_name_full_2, res_buf);
        if (strcmp(res_buf, "No_response_message") == 0)
            {
                // Копируем своей запрос (принимаем его как правильный)
                strncpy (ANSWER[j], get_hash_programm_name_full_2, request_size_full_2);
                ANSWER[j][request_size_full_2]='\0';
                memset(ANSWER[j],0,ANSWER_LENGTH);
                ANSWER[j][0] = 'G';
                ANSWER[j][1] = 'A';
                ANSWER[j][2] = 'G';
                continue;
            }
        int real_length = strlen(res_buf);
        strncpy (ANSWER[j], res_buf,real_length);
        ANSWER[j][real_length]='\0';    
    }

    // А тут получается мы выбираем ANSWER по принципу "каких ответов больше, тот ответ и правильный"
   int ij = 0;
   int ji = 0;
   int frequence [count_I2P];
   int y = 0;
   for (y; y < count_I2P; y++)
    frequence[y] = 0;

   // Считаем частоту встречания того или иного хэша
   for (ij; ij < count_I2P; ij++)
    {
        for (ji; ji < count_I2P; ji++)
            {
                if(strcmp(ANSWER[ij],ANSWER[ji]) == 0)
                    {
                        frequence[ij] = frequence[ij] + 1;
                    }   
            }
        ji = 0; 
    }
   
    int jjj = 0;
    for (jjj; jjj < count_I2P; jjj++)
        {
        //printf("ANSWER[%d]=|%s|\n",jjj,ANSWER[jjj]);
        //printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);      
        }


    // Находим наибольший элемент среди frequence
    int ii = 0;
    int maximum = 0; 
    int maximum_ID = 0; // По умолчанию - 0-ый элемент массива самый наиболее встречающийся
    for (ii; ii < count_I2P - 1; ii++)
        {
            if (frequence[ii] > maximum)//frequence[ii+1])
                {
                    maximum = frequence[ii];
                    maximum_ID = ii;
                }
        }

    if (frequence[count_I2P -1] > maximum)
    {
        maximum = frequence[count_I2P -1];
        maximum_ID = count_I2P -1;
    }   

   //printf("%s%s|\n","Наиболее часто встречающийся ANSWER = ", ANSWER[maximum_ID]);
    if (strcmp(ANSWER[maximum_ID],"GAG") == 0)
    {
        // Ищем новый максимум
        int max_flag = 0;   
        int new_max = maximum;
        int iii = 0;
        while (new_max != 0)
        {
            for (iii=0; iii<count_I2P; iii++)
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
    for (jjj; jjj < count_I2P; jjj++)
        {
        //printf("ANSWER[%d]=|%s|\n",jjj,ANSWER[jjj]);
        //printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);      
        }

     //printf("%s%s|\n","Наиболее часто встречающийся ANSWER ТЕПЕРЬ = ", ANSWER[maximum_ID]);

    if (strcmp(ANSWER[maximum_ID],"NO") == 0)
        {
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
        sleep(4);
        }
    else 
        {
        }
   return 0;
} 

int get_hash_or_blockchain_i2pd(const char* hostname, const char* hash_or_blockchain, char* buffer) // В buffer будет записан результат, где buffer[BUF_SIZE]
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_hash_or_blockchain_i2pd\"]\n");
    //printf("Отправляю запрос [%s] к I2P: ||%s||\n", hash_or_blockchain, hostname);   

    char hash[BUF_SIZE] = "get_last_id_hesh\n";
    char blockchain[BUF_SIZE] = "get_actual_blockchain\n";
    char buf[BUF_SIZE];

    int buf_size;
    std::string target_before(hostname);
    const std::string& target = target_before;
    //const std::string target("kjsdbfkjdsfkjsdhfjksadkfjasdlkjfhasdfasldkjfhalskjdfhlkjasdhfasdgfuiasdhfghafkasdgfiuwe");    
    printf("create session...\n");
    //SAM::StreamSession session("session_client", "127.0.0.1", 7656);
    std::shared_ptr<SAM::StreamSession> session;
    if (strcmp(hash_or_blockchain,"hash") == 0)
        {
            session = session_ptr_hash_blockchain;
            memcpy(buf, hash, sizeof hash);
            //printf("%s\n","Запрос на получение last_id_hash");
        }
    else if (strcmp(hash_or_blockchain,"blockchain") == 0)
        {
            session = session_ptr_hash_blockchain; 
            memcpy(buf, blockchain, sizeof blockchain);
            //printf("%s\n","Запрос на получение actual_blockchain");
        }
    else 
        {
            session = session_ptr_send_message;
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
    
    printf("connect...\n");
    auto connect_result = session->connect(target, false);
    if (!connect_result.isOk) 
    {
        int count_connect = 0;
        int MAX_COUNT_CONNECT = 1; //
        while (!connect_result.isOk)
        {
         if (count_connect >= MAX_COUNT_CONNECT)
            {
        
                if (strcmp(hash_or_blockchain,"hash") == 0)
                    {
                        printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / hash, I2P-client: ", hostname);
                        fflush(stdout);
                        const char* buf_no = "No_response_hash";
                        int recieve_bytes = strlen (buf_no);
                        strncpy(buffer,buf_no,recieve_bytes);
                        return 0;
                    }
                else if (strcmp(hash_or_blockchain,"blockchain") == 0)
                    {
                        printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / blockchain, I2P-client: ", hostname);
                        fflush(stdout);
                        const char* buf_no = "No_response_blockchain";
                        int recieve_bytes = strlen (buf_no);
                        strncpy(buffer,buf_no,recieve_bytes);
                        return 0;
                    }
                else 
                    {
                        printf("%s%s\n","Не смог подключиться к другому клиенту в get_hash_or_blockchain / message, I2P-client: ", hostname);
                        fflush(stdout);
                        const char* buf_no = "No_response_message";
                        int recieve_bytes = strlen (buf_no);
                        strncpy(buffer,buf_no,recieve_bytes);
                        return 0;
                }

            }
            printf("%s%s ...\n","Trying connecting to another_client: ", hostname);
            fflush(stdout);
            count_connect++;
            connect_result = session->connect(target, false);
        }
    }

    FILE* fd;
    const char* data_filename = "data_filename.txt";
    if (( fd = fopen(data_filename, "wb")) == NULL)
    {
        perror("open failed on file file.dat");
        exit(1);
    }
    else
    {
        fwrite(buf, 1, strlen(buf), fd);
        fclose(fd);
    }
    SAM::I2pSocket* socket = connect_result.value.get(); // auto
    unsigned char uc_int[INT_SIZE];
    memset(uc_int,0,INT_SIZE);
    //printf("strlen(buf) = |%d|\n", strlen(buf));
    int len_buf = strlen(buf);
    //printf("len_buf = |%d|\n", len_buf);
    int_to_unsigned_char_array_4(len_buf, uc_int);
    int len_buf_2 = unsigned_char_array_4_to_int(uc_int);
    //printf("len_buf_2 = |%d|\n", len_buf_2);
    //printf("uc_int = |%s|\n", uc_int);
    socket->write(uc_int, INT_SIZE);
    int  numread, count;
    if (( fd = fopen(data_filename, "rb")) == NULL)
       {
            perror("open failed on file file.dat");
            exit(1);
       }
    int size_data = strlen(buf); 
    char buffer_data[512];
    memset(buffer_data, 0, 512);
    int total_sent = 0;
    int file_size_yet = size_data;
    while(total_sent != size_data)
    {
    if (file_size_yet <  512)
        numread = fread(buffer_data, 1, file_size_yet, fd); // offset сохраняется автоматически
    else
        numread = fread(buffer_data, 1, 512, fd);
    //printf("numread = %d\n", numread);
        int sentBytes = 0;
    int i;
    for (i = 0; i < numread; i = i + sentBytes)
    {
        sentBytes = socket->write(reinterpret_cast<unsigned char*>(&buffer_data[i]), numread-i);
        sleep(1);
    }
    total_sent = total_sent + numread;
    //printf("total_sent = %d\n", total_sent);
    file_size_yet = file_size_yet - numread;    
    }
    
    fclose(fd);
    memset(buf, 0, BUF_SIZE);
    unsigned char buf_int [INT_SIZE];
    int res = 0;
    struct timeval tv;
    tv.tv_sec = 200; //200; //120; //60; //200 //timeout_in_seconds; 150 - точно работает
    tv.tv_usec = 0;
    int ret_val = setsockopt(socket->socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int e = errno;
    res = socket->read(INT_SIZE, buf_int);
    fflush(stdout);
    if (res != INT_SIZE)
    {
        if (strcmp(hash_or_blockchain,"hash") == 0)
                            {
                                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / hash, I2P-client: ", hostname);
                                fflush(stdout);
                                const char* buf_no = "No_response_hash";
                                int recieve_bytes = strlen (buf_no);
                                strncpy(buffer,buf_no,recieve_bytes);
                                int i =0;
                                socket->close();
                                return 0;
                            }
                        else if (strcmp(hash_or_blockchain,"blockchain") == 0)
                            {
                                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / blockchain, I2P-client: ", hostname);
                                fflush(stdout);
                                const char* buf_no = "No_response_blockchain";
                                int recieve_bytes = strlen (buf_no);
                                strncpy(buffer,buf_no,recieve_bytes);
                                socket->close();
                                return 0;
                            }
                        else 
                            {
                                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / message, I2P-client: ", hostname);
                                fflush(stdout);
                                const char* buf_no = "No_response_message";
                                int recieve_bytes = strlen (buf_no);
                                strncpy(buffer,buf_no,recieve_bytes);
                                socket->close();
                                return 0;
                            }           
    }
    fflush(stdout);
    const int reply_size = unsigned_char_array_4_to_int(buf_int);
    int data_size = reply_size;
    unsigned char indata [data_size];
        int recieved_total = 0;
        int inverse_size_have = data_size;
        std::string filename_data_str = "from_client_get_hash_blockchain_.txt";
        const char* filename_data = filename_data_str.c_str();
    pthread_mutex_lock(&mutex);
        FILE *mf;
        mf=fopen (filename_data,"w");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
        }
        while (recieved_total != data_size)
        {
            int recv_bytes = socket->read(512, &indata[recieved_total]);
            // Сохраняем принятые данные в файл
            fwrite(reinterpret_cast<unsigned char*>(&indata[recieved_total]), 1, recv_bytes, mf);
            //write_to_file_name_c(reinterpret_cast<unsigned char*>(&indata[recieved_total]), recv_bytes, filename_data);
            recieved_total = recieved_total + recv_bytes;
            inverse_size_have = inverse_size_have - recv_bytes;
            printf("recv_bytes = %d\n", recv_bytes);
            printf("recieved_total = %d\n", recieved_total);
            if ((inverse_size_have < 512) && (inverse_size_have > 0))
            {
                int recv_bytes_last = socket->read(inverse_size_have, &indata[recieved_total]);
                if (recv_bytes_last != inverse_size_have)
                {
                    printf("Error_receive_data !\n");
                    exit(1);
                }
                //write_to_file_name_c(reinterpret_cast<unsigned char*>(&indata[recieved_total]), recv_bytes_last, filename_data);
                fwrite(reinterpret_cast<unsigned char*>(&indata[recieved_total]), 1, recv_bytes_last, mf);
                inverse_size_have = inverse_size_have - recv_bytes_last;
                recieved_total = recieved_total + recv_bytes_last;
                printf("recv_bytes_last = %d\n", recv_bytes_last);
                printf("recieved_total = %d\n", recieved_total);
                break;
            }
        }
        fclose (mf);
        char buffer_data_real [data_size];
        mf=fopen (filename_data,"r");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
        }
        int result = fread (buffer_data_real,1,data_size,mf);
        if (result != data_size) 
        {
            fputs ("Reading error",stderr); exit (3);
        }
        fclose (mf);
        pthread_mutex_unlock(&mutex);
    if (result != data_size)
    {
        if (strcmp(hash_or_blockchain,"hash") == 0)
            {
                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / hash, I2P-client: ", hostname);
                fflush(stdout);
                const char* buf_no = "No_response_hash";
                int recieve_bytes = strlen (buf_no);
                strncpy(buffer,buf_no,recieve_bytes);
                socket->close();
                return 0;
            }
        else if (strcmp(hash_or_blockchain,"blockchain") == 0)
            {
                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / blockchain, I2P-client: ", hostname);
                fflush(stdout);
                const char* buf_no = "No_response_blockchain";
                int recieve_bytes = strlen (buf_no);
                strncpy(buffer,buf_no,recieve_bytes);
                socket->close();
                return 0;
            }
        else 
            {
                printf("%s%s\n","Не дождался ответа от сервера в get_hash_or_blockchain / message, I2P-client: ", hostname);
                fflush(stdout);
                const char* buf_no = "No_response_message";
                int recieve_bytes = strlen (buf_no);
                strncpy(buffer,buf_no,recieve_bytes);
                socket->close();
                return 0;
            }         
    }
    socket->close();
    strncpy(buffer, buffer_data_real, data_size);
    return 0;
}


int get_actual_blockchain_copy_i2pd (void)
{  
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_actual_blockchain_copy_i2pd\"]\n");
    char I2P[MAX][MAX_DESTINATION_LENGTH];
    char HASH[MAX][HEX_LENGTH];
   int i = 0;
   for (i;i<MAX;i++)
    {
        memset(I2P[i], 0, MAX_DESTINATION_LENGTH);
        memset(HASH[i], 0, HEX_LENGTH);
    }
   FILE *mf;
   char str[1000];
   memset(str,0,1000);
   char *estr;
   pthread_mutex_lock(&mutex1); 
   mf = fopen ("Keys/user_list_client_i2pd.txt","r");
   if (mf == NULL) { 
   return -1;
   }
   else 
   {
   }
   int count_I2P = 0;
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
      strncpy (I2P[count_I2P], str, strlen(str)); // -1
      int size_I2P_str = strlen(str);
      I2P[count_I2P][size_I2P_str] = '\0';
      I2P[count_I2P][size_I2P_str-1] = '\0';
      //printf ("     IP  = %s|",IP[count_IP]);
      
      count_I2P++;
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
    //printf("count_IP = %d\n", count_IP);
    std::string pd;

   {
    // Считываем два адреса в соответствии с именем вводимым
    std::string path = "Keys/" + MY_NAME + "/PUBLIC_hash_and_blockchain.txt";
    const char* desinatin_key_file = path.c_str();
    char buffer[MAX_DESTINATION_LENGTH];
    memset(buffer, 0, MAX_DESTINATION_LENGTH);
    int size = read_from_file_to_buffer_1(desinatin_key_file, buffer, MAX_DESTINATION_LENGTH);
    buffer[size-1] = '\0';
    std::string mydest_buf(buffer);
    pd = mydest_buf;
    my_public_dest_hash_and_blockchain = mydest_buf;
      
    }

    //std::cout << "my_public_dest_hash_and_blockchain = ||" << my_public_dest_hash_and_blockchain << "||" << std::flush;
    //std::cout << "pd = ||" << pd << "||" << std::flush;
    //std::cout << "" << std::endl; 
    //sleep(3);

   int j = 0;
   for (j; j < count_I2P; j++)
    {
        //printf("Сравнение: I2P[%d] = ||%s||, my_public_dest_hash_and_blockchain = ||%s||.\n",j,I2P[j],my_public_dest_hash_and_blockchain.c_str());
        //sleep(5);
        if (strcmp(I2P[j],pd.c_str()) == 0)
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
            
        char res_buf [HEX_LENGTH+1];
        int c = 0;
        for (c; c < HEX_LENGTH; c++)
            res_buf[c] = '\0';
        //call_Linux_programm(get_hash_programm_name_full, res_buf);
        //printf("IP[j]_2 = %s\n", IP[j]);
        get_hash_or_blockchain_i2pd(I2P[j], "hash", res_buf);
        //printf("МЫ ВЫШЛИ ИЗ get_hash_or_blockchain_i2pd\n");
        //printf("res_buf = |%s|\n", res_buf);
        fflush(stdout);
        //sleep(5);
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

                    printf("No_response_hash from I2P = %s\n", I2P[j]);
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
   int frequence [count_I2P];
   int y = 0;
   for (y; y < count_I2P; y++)
    frequence[y] = 0;
   //printf("count_I2P = %d\n", count_I2P);
   // Считаем частоту встречания того или иного хэша
   for (ij; ij < count_I2P; ij++)
   {
    for (ji; ji < count_I2P; ji++)
        {
            if(strcmp(HASH[ij],HASH[ji]) == 0)
                {
                    frequence[ij] = frequence[ij] + 1;
                }   
        }
        ji = 0;
   }

    int jjj = 0;
    for (jjj; jjj < count_I2P; jjj++)
        {
        //printf("HASH[%d]=|%s|\n",jjj,HASH[jjj]);
        //printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);      
        }

    // Находим наибольший элемент среди frequence
    int maximum = 0; 
    int maximum_ID = 0; // По умолчанию - 0-ый элемент массива самый наиболее встречающийся
    int ii = 0;
    for (ii; ii < count_I2P - 1; ii++)
        {
            if (frequence[ii] > maximum)//frequence[ii+1]))
                {
                    maximum = frequence[ii];
                    maximum_ID = ii;
                }
        }

    if (frequence[count_I2P -1] > maximum)
    {
        maximum = frequence[count_I2P -1];
        maximum_ID = count_I2P -1;
    }   
    if (strcmp(HASH[maximum_ID],"GAG") == 0)
    {
        // Ищем новый максимум
        int max_flag = 0;   
        int new_max = maximum;
        int iii = 0;
        while (new_max != 0)
        {
            for (iii=0; iii<count_I2P; iii++)
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
 for (jjj; jjj < count_I2P; jjj++)
    {
    //printf("HASH[%d]=|%s|\n",jjj,HASH[jjj]);
    //printf("frequence[%d]=|%d|\n",jjj,frequence[jjj]);      
    }
 //printf("%s%s|\n","Наиболее часто встречающийся ХЭШ ТЕПЕРЬ = ", HASH[maximum_ID]);

    char res_buf [HEX_LENGTH];
    int c = 0;
    for (c; c < HEX_LENGTH; c++)
        res_buf[c] = '\0';
        
    //call_Linux_programm(get_hash_programm_name_full, res_buf);
    sleep(3);
    //printf("IP[maximum_ID] = %s\n", IP[maximum_ID]);
    get_hash_or_blockchain_i2pd(I2P[maximum_ID], "blockchain", res_buf);
    
    if (strcmp(res_buf, "No_response_blockchain") == 0)
        {
            int io = 0;
            int max_trying_times = 3; //10;
            while ((strcmp(res_buf, "No_response_blockchain") == 0) && (io < max_trying_times))
                {
                    c = 0;
                    for (c; c < HEX_LENGTH; c++)
                        res_buf[c] = '\0';
                    //printf("IP[count_IP -io] = %s\n", IP[maximum_ID]);
                    printf("No_response_blockchain from I2P = %s\n", I2P[maximum_ID]);
                    sleep(2);
                    get_hash_or_blockchain_i2pd(I2P[maximum_ID], "blockchain", res_buf);                      
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



void * send_message_server_part_function_i2pd(void * arg)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"send_message_server_part_function_i2pd\"]\n");
    struct i2p_session_str structure;
    structure = *(i2p_session_str*) arg;
    std::shared_ptr<SAM::StreamSession> session = structure.session_ptr;
    int number = structure.number;
    //printf("send_message_tread[%d] have been started!\n", number);
    while (true) 
    {
        SAM::RequestResult<std::unique_ptr<SAM::I2pSocket> > session_accept;
        SAM::I2pSocket* socket;
        session_accept = session->accept(false);
            if (!session_accept.isOk)
        {
                printf("session_accept[%d] failed! Trying more time...\n", number);
            session_accept = session->accept(false);
            if (!session_accept.isOk)
            {
                printf("session_accept[%d] failed! \n", number);
                continue;
            }
                
            }
        socket = session_accept.value.get();
        socket_send_message[number] = socket;
        //printf("send_message_read[%d]...\n", number);
                       
        unsigned char another_dest_ch [PUBLIC_KEY_LENGTH];

 		int recieve_len = 0;
        
        while (true)
        	{
        		unsigned char ch[1];
        		memset(ch,0,1);
        		int one = socket->read(1, &ch[0]);
        		if (ch[0] == '\n')
        		{
        			another_dest_ch[recieve_len] = ch[0];
        			recieve_len++;
        			break;
        		}
        		another_dest_ch[recieve_len] = ch[0];
        		recieve_len++;	

        	}
        	recieve_len++;
			another_dest_ch[recieve_len] = '\0';        	
            
        
        printf("recieve_len = %d\n",recieve_len);
        std::string another_dest_ (const_cast<const char*>(reinterpret_cast<char*>(another_dest_ch)));
        const std::string& another_dest = another_dest_;
		if(another_dest == "")
		{
			// Выйти
			printf("%s\n", "Безопасный выход...");
			sleep(5);
			void * gag = nullptr;
			return gag;
		}        
        unsigned char buf_int [INT_SIZE];
            int res = socket->read(INT_SIZE, buf_int);
            int data_size = unsigned_char_array_4_to_int(buf_int);
            printf("send_message_data_size[%d] %d ...\n", number, data_size);
            unsigned char indata [data_size];
        int recieved_total = 0;
        int inverse_size_have = data_size;
        std::string filename_data_str = "from_client_send_message_" + std::to_string(number) + ".txt";
        const char* filename_data = filename_data_str.c_str();
        pthread_mutex_lock(&mutex);
        FILE *mf;
        mf=fopen (filename_data,"w");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
        }
        while (recieved_total != data_size)
        {
            printf("recieved_total = %d\n", recieved_total);
            printf("data_size = %d\n", data_size);
            int recv_bytes = socket->read(512, &indata[recieved_total]);
            // Сохраняем принятые данные в файл
            fwrite(reinterpret_cast<unsigned char*>(&indata[recieved_total]), 1, recv_bytes, mf);
            //write_to_file_name_c(reinterpret_cast<unsigned char*>(&indata[recieved_total]), recv_bytes, filename_data);

            recieved_total = recieved_total + recv_bytes;
            inverse_size_have = inverse_size_have - recv_bytes;
            printf("recv_bytes = %d\n", recv_bytes);
            printf("recieved_total = %d\n", recieved_total);
            if (inverse_size_have == 0)
                break;
            if ((inverse_size_have < 512) && (inverse_size_have > 0))
            {
                int recv_bytes_last = socket->read(inverse_size_have, &indata[recieved_total]);
                if (recv_bytes_last != inverse_size_have)
                {
                    printf("Error_receive_data[%d] !\n", number);
                    exit(1);
                }
                //write_to_file_name_c(reinterpret_cast<unsigned char*>(&indata[recieved_total]), recv_bytes_last, filename_data);
                fwrite(reinterpret_cast<unsigned char*>(&indata[recieved_total]), 1, recv_bytes_last, mf);
                inverse_size_have = inverse_size_have - recv_bytes_last;
                recieved_total = recieved_total + recv_bytes_last;
                printf("recv_bytes_last = %d\n", recv_bytes_last);
                printf("recieved_total = %d\n", recieved_total);
                break;
            }
        }
        //printf("read result = before-2\n");
        fclose (mf);


        //printf("read result = before-1\n");
        fflush(stdout);
        char buffer_data_real [data_size+1];
        memset(buffer_data_real, 0, data_size+1);
        //printf("read result = before0\n");
        fflush(stdout);
        mf=fopen (filename_data,"r");
        if (mf == NULL)
        { 
            printf ("ошибка\n");
            exit(1);
        }
        else 
        {
            // printf ("выполнено\n");
        }
        //printf("read result = before1\n");
        fflush(stdout);
        int result = fread (buffer_data_real,1,data_size,mf);
        //printf("read result = %d\n",result);
        fflush(stdout);
        if (result != data_size) 
        {
            fputs ("Reading error",stderr); exit (3);
        }
        fclose (mf);
        pthread_mutex_unlock(&mutex);
        //printf("lala_send_message_");
        fflush(stdout);
 
     int i = 0;
     for (i; i < BUF_SIZE; i++)
        {
            if (buffer_data_real[i] == ';')
                {
                    break;
                }
        }
 
     
     // Копируем считанное как ID и HASH
     
     
     int size_1 = i;
     char ID_str[size_1+1];
     memset(ID_str,0,size_1+1);
     strncpy (ID_str, buffer_data_real, size_1);
     ID_str[size_1] = '\0';
  //   printf ("     ID_str  = |%s|\n",ID_str);
     
     int ID = atoi((const char*)ID_str);
     
     i++;
     for (i; i < BUF_SIZE; i++)
        {
            if (buffer_data_real[i] == ';')
                {
                    break;
                }
        }
     
          
     int len_of_hash = i - 1 - size_1;     
     char HASH_of_before_ID[len_of_hash+1]; 
     strncpy (HASH_of_before_ID, &buffer_data_real[size_1 + 1], len_of_hash);
     HASH_of_before_ID[len_of_hash] = '\0';
 //    printf ("     HASH_of_before_ID  = |%s|\n", HASH_of_before_ID);
     
     i++;
     

     int len_of_enc_mess = recieved_total - size_1 - len_of_hash - 2;   
     char encrypt_message_part[len_of_enc_mess + 1];
     memset(encrypt_message_part,0,len_of_enc_mess + 1);
     strncpy (encrypt_message_part, &buffer_data_real[size_1 + len_of_hash + 2], len_of_enc_mess);
     encrypt_message_part[len_of_enc_mess] = '\0';
   //  printf ("     ENC_MESS  = |%s|\n",encrypt_message_part);


     // Запросить сколько всего id  в БД
     
     char last_id_c[BUF_SIZE];
     memset(last_id_c, 0, BUF_SIZE);
     get_how_much_id (PATH_Date_Base,last_id_c);  
     int last_id = atoi((const char*)last_id_c);
  //   printf("last_id = %d|\n", last_id);
     
     // Если в БД last_id <= 10, то size_buf = last_id, иначе size_buf = 10; 
     int size_buf;
     
     size_buf = last_id;
     
     //if (last_id <= 10)
        
     //else 
    //  size_buf = 10;  

     // Запрос из БД последних size_buf id
     int my_DB_last_ID_arr[size_buf];
     int ii = 0;
     for(ii; ii < size_buf; ii++)
    {
        my_DB_last_ID_arr[ii] = size_buf - ii;
    }
     
     
     // Запрос из БД hash последнеих 10-ти id-контейнеров
     
     char my_DB_last_ID_container_hash_arr [size_buf][HEX_LENGTH];
     
     ii = 0;
     for(ii;ii<size_buf;ii++)
        {
            
            memset(my_DB_last_ID_container_hash_arr[ii], 0, HEX_LENGTH);
            int length = snprintf( NULL, 0, "%d", my_DB_last_ID_arr[ii]);
            char last_id_str [length + 1];
            snprintf(last_id_str, length + 1, "%d", my_DB_last_ID_arr[ii]); // по идее в последнем символе и так уже ноль должен быть
            last_id_str[length] = '\0';
            
        get_hash_id_container (PATH_Date_Base, last_id_str, my_DB_last_ID_container_hash_arr[ii]);

    //  printf("my_DB_last_ID_container_hash_arr[%d] = %s|\n", ii, my_DB_last_ID_container_hash_arr[ii]);
        }


      int my_DB_last_ID = my_DB_last_ID_arr[0];
      const char* my_DB_last_ID_container_hash = my_DB_last_ID_container_hash_arr[0];

     //  Проверяем совпадение hash-ей (какой посчету с конца совпадет с присланным)
     
     int count_hash = 0;
  //   printf("%s\n","checking ...");
     
     int flag = 0;





     for (count_hash; count_hash < size_buf; count_hash ++) // count_hash < size_buf поменяли на 3 (так надо для правильной работы алгоритмов)
        {
        
            if (count_hash > 0)
                break;
        
                      
            if (((ID - 1) == my_DB_last_ID) && (strcmp(HASH_of_before_ID, my_DB_last_ID_container_hash) == 0))
                {
                
                   // Пересылаем этот же буфер обратно как ответ 
                    if (count_hash == 0)
                        {
                        
                            // Добавление BEGIN -----------------------
                            int len_buf = strlen(buffer_data_real);
                            unsigned char uc_int [4];
                            memset(uc_int,0,4);
                            int_to_unsigned_char_array_4 (len_buf, uc_int);

                            socket->write(uc_int, INT_SIZE);
                            socket->write(reinterpret_cast<unsigned char*>(buffer_data_real), len_buf);                            
                            
                        }
                        
                    else
                    
                        //Это никогда не запуститься
                        {
                            
                            //Запросить все целиком контейнеры из БД (от "count_hash -1" до "0")    
                            
                            
                            // Сформировать из них большой буфер char[]
                            char big_buffer[MAX_TRANSACTION_LENGTH];
                            memset(big_buffer, 0, MAX_TRANSACTION_LENGTH);
                            
                            //printf("%s","checking...\n");
                            
                            const char* path_to_buffer_file = "buf_file.txt";
                            int from_id =  my_DB_last_ID + 1;                           
                            int size_of_buffer = blockchain_from_DB_to_file_c_correct (PATH_Date_Base, from_id, path_to_buffer_file, big_buffer);
                            
                             if(remove(path_to_buffer_file)) 
                                {
                                    printf("Error removing path_to_buffer_file");
                                    exit(1);
                                }
                            
                            //printf("Размер буфера = %d\n", size_of_buffer);
                            
                            // Добавить к этому всему тот контейнер, который прислали (изменив ID на новый, encrypt - не меняем, т.к. он в дальнейшем будет зашифрован)
                            
                            // Узнаем последний существующий ID и прибавляем 1
                            
                            
                            char last_id_c[BUF_SIZE];
                            memset(last_id_c, 0, BUF_SIZE);
                            get_how_much_id (PATH_Date_Base,last_id_c);  
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
                            get_hash_id_container (PATH_Date_Base, last_id_c, hash_last_id_container);
                    
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

                            socket->write(uc_int, INT_SIZE);
                            socket->write(reinterpret_cast<unsigned char*>(big_buffer), size_of_buffer + len_of_enc_mess + 1); 
                            
                        }
                        
                    // Добавить присланную транзакцию в свою копию блокчейн
                    
                    
                        // Запросить сколько всего id  в БД
     
                        char last_id_c1[BUF_SIZE];
                        memset(last_id_c1, 0, BUF_SIZE);
                        get_how_much_id (PATH_Date_Base,last_id_c1);  
                        int last_id1 = atoi((const char*)last_id_c1);
                        //printf("last_id = %d|\n", last_id1);
                        
                        // Запросить hash предыдущего
                    
                    char last_id_container_hash_c1[HEX_LENGTH];
                    memset(last_id_container_hash_c1, 0, HEX_LENGTH);
                    get_hash_id_container (PATH_Date_Base, (const char*) last_id_c1, last_id_container_hash_c1);

                    const char* hesh_before = last_id_container_hash_c1;

                    int length0 = snprintf( NULL, 0, "%d", len_of_enc_mess);

                    char hesh_size[length0+1];
                    memset(hesh_size,0,length0+1);
                    snprintf(hesh_size, length0 + 1, "%d", len_of_enc_mess); // по идее в последнем символе и так уже ноль должен быть
                        hesh_size[length0] = '\0';
                    
                    int last_id2 = last_id1 + 1;
                    
                    int length1 = snprintf( NULL, 0, "%d", last_id2);
                        char last_id_str1 [length1 + 1];
                        snprintf(last_id_str1, length1 + 1, "%d", last_id2); // по идее в последнем символе и так уже ноль должен быть
                        last_id_str1[length1] = '\0';
                    
                    const char* hash_str = "hash_";
                    const char* of = "_of_";
                    
                    // Соединяем несколько строк вместе ...
                
                    int union_size = strlen(hash_str) + strlen(hesh_size) + strlen(of) + strlen(last_id_str1) + 1; // +1 - это для '\0'-символа конца строки
                    char union_str [union_size];
                    snprintf(union_str, sizeof union_str, "%s%s%s%s", hash_str, hesh_size, of, last_id_str1);
                    union_str [union_size] = '\0';
                    
                    // Добавляем полученную транзакцию в БД в BLOCKCHAIN_TABLE
                    
                    // ТО ЧТО СВЕРХУ - ПОКА ОСТАВИМ (ВДРУГ ПРИГОДИТЬСЯ, чтобы ничего не сломалось)
                    
                    // А тут вычислим НОРМАЛЬНЫЙ HASH
                    
                    // ------ Вызов функции взятия ХЭШа от const char* ------ BEGIN -----------------
                    
                    int union_size_1 = strlen(encrypt_message_part) + strlen(hesh_before) + 1; // +1 - это для '\0'-символа конца строки
                    char string_for_hashing_0 [union_size_1];
                    snprintf(string_for_hashing_0, sizeof string_for_hashing_0, "%s%s", encrypt_message_part, hesh_before);
                    string_for_hashing_0 [union_size_1] = '\0';
                    
                    const char* string_for_hashing =  (const char*) string_for_hashing_0; //argv[1];
    
                    char hash_from_string[HASH_LENGTH];
                    memset(hash_from_string,0,HASH_LENGTH);
    
                    get_hash_from_string (string_for_hashing, hash_from_string);
    
                    const char* hash_from_string_result = (const char*)hash_from_string;
    
                    //printf("hash_from_string_result = %s|\n",hash_from_string_result);
                        
                    // ------ Вызов функции взятия ХЭШа от const char* ------ END -----------------
                    
                    // Проверка на совпадение IP-адресов клиента и сервера (чтобы избежать двойного добавления)



                      
                    if (strcmp(another_dest.c_str(), my_public_send_message.c_str()) == 0)
                        {

                            // ПОДУМАТЬ КАК ТУТ ДЕЛАТЬ ВООБЩЕ !!! (возможно сравнивать со свои публичным адресом !!!)

                            //printf("%s\n", "IP-адресс клиента == моему IP-адресу. Добавление транзакции произведено не будет.");
                        }
                    else
                        {
                            int res_add = add_record_to_local_blockchain(PATH_Date_Base, last_id2, (const char*) encrypt_message_part, hesh_before, hash_from_string_result);
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
                                analyse_blockchain_for_input_messages(PATH_Date_Base);
                                check_clear_blockchain();
                        }   
                        
                //printf("%s\n","Мое мнение совпало с мнением запрашивающего."); 
                flag = 1;
                break;                  
                }
            else 
                {
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



    // Проверяем нашлось ли совпадение
    if (flag == 0)
        {
            // Отправляем сообщение о том, что совпадений в последних 10-ти ячейках найдено не было
            
            // Добавление BEGIN -----------------------
                int len_buf = 2;
                unsigned char uc_int [4];
                memset(uc_int,0,4);
                int_to_unsigned_char_array_4 (len_buf, uc_int);

                socket->write(uc_int, INT_SIZE);
                std::string NO_str = "NO";
                socket->write(reinterpret_cast<unsigned char*>(const_cast<char*>(NO_str.c_str())), NO_str.length());
            
            // Не добавляем в свою копию локального блокчейн это сообщение
            //timeout(1); // Это чтобы другие члены сети успели обновить свои blockchain
            // Запрос на обновление своей копии blockchain
            
            //printf("%s\n","Совпадений id и hash не нашлось.");
        }
    else
        {
            // Добавляем в свою копию локального блокчейн это сообщение (запрос в БД делаем)
                // Дописываем в файл "blockchain.txt" то, что прислали - (ВМЕСТО ЭТОГО _ ЗАПРОС К БД)
                // Прогоняем через алгоритм проверки блокчейн на наличие входящих МНЕ сообщений
            //printf("%s%d\n","Совпадение ID и HASH было найдено на шаге: ", count_hash);
        }   

            
        // Закрываем соединение 
        socket->close();
        sleep(5);
        //printf("%s\n","Виток участия во всеобщем алгоритме консенсуса завершен.");

    if (flag == 0)
        {
       

        }











    }

}






// For i2p -------------------- END (06.09.21) ------------------------









void user_pause (void)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"user_pause\"]\n");
    // Пауза для анализа произошедшего
    std::cout << "Enter any symbol for continue: ";
    char a;
    std::cin >> a;
    std::cout << "" << std::endl;
    return;
}


long int filesize_share( FILE *fp )
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"filesize_share\"]\n");
    long int save_pos, size_of_file;
    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
}


std::string count_hash ( const char* filename_for_hash)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"count_hash\"]\n");
    using namespace CryptoPP;
    std::string digits;
    CryptoPP::SHA256 hash;
    FileSource f(filename_for_hash, true, new HashFilter(hash, new HexEncoder(new StringSink(digits))));
    return digits;
}


int encrypt (const char* filename_for_encrypte)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"encrypt\"]\n");
    using namespace std;
    using namespace CryptoPP;
    const char* filename_c = filename_for_encrypte;
    byte key[AES::MAX_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    vector<byte> plain;
    vector<byte> cipher, recover;
    
    HexEncoder encoder(new FileSink(cout));

    memset(key, 0x00, sizeof(key));
    memset(iv, 0x00, sizeof(iv));

    //  string str("Attack at dawn!");
    //  std::copy(str.begin(), str.end(), std::back_inserter(plain));

    // ------------------------------------

    std::string filename (filename_c);

    // Создали указатель на файл
    FILE* f;

    //Открыли файл (с проверкой)
    if ((f=fopen(filename.c_str(), "rb"))==NULL)
    {
        std::cout << "Невозможно открыть файл для чтения оригинального файла!" << std::endl;
	return 1;
        //exit(1);
    }

    // //Узнали его размер
    int fsize = filesize_share(f);
    // std::cout << "File_size = " << fsize << std::endl;

    byte* plain1 = new byte[fsize];
    int real_read_bytes = fread(plain1, 1, fsize, f);
    // std::cout << "Real_read_bytes = " << real_read_bytes << std::endl;

    fclose(f);

    for (int i =0; i < fsize; i ++)
    {
        plain.push_back(plain1[i]);
    }

    delete[] plain1;

    // ------------------------------------

    //std::cout << "Plain_size = " << plain.size() << std::endl;
    //cout << "Plain text: ";
    encoder.Put(plain.data(), plain.size());
    encoder.MessageEnd();
    //cout << endl;

    /////////////////////////////////////////////////////////////

    CFB_Mode<AES>::Encryption enc;
    enc.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));

    // Make room for padding
    cipher.resize(plain.size()+AES::BLOCKSIZE);
    ArraySink cs(&cipher[0], cipher.size());

    ArraySource(plain.data(), plain.size(), true,
                new StreamTransformationFilter(enc, new Redirector(cs)));

    // Set cipher text length now that its known
    cipher.resize(cs.TotalPutLength());

    //std::cout << "Cipher_size = " << cipher.size() << std::endl;
    //cout << "Cipher text: ";
    encoder.Put(cipher.data(), cipher.size());
    encoder.MessageEnd();
    //cout << endl;

    // Сохранение "key" в файл
    std::string filename_aes_key = "AES.key";
    // Открыли файл (с проверкой)
    if ((f = fopen(filename_aes_key.c_str(), "wb")) == NULL)
    {
        std::cout << "Невозможно открыть файл для записи KEY!" << std::endl;
        exit(1);
    }

    // Записываем ключевые байты в файл
    int real_write_bytes_key = fwrite(key, 1, AES::MAX_KEYLENGTH, f);
    if (real_write_bytes_key != AES::MAX_KEYLENGTH) {
        std::cout << "ERROR = real_write_bytes_key != AES::MAX_KEYLENGTH " << std::endl;
        exit(1);
    }
    fclose(f);

    //std::cout << "AES.key с ключевыми байтами записан" << std::endl;

    // Сохранение "iv" в файл
    std::string filename_aes_iv = "AES.iv";

    // Открыли файл (с проверкой)
    if ((f = fopen(filename_aes_iv.c_str(), "wb")) == NULL)
    {
        std::cout << "Невозможно открыть файл для записи IV!" << std::endl;
        exit(1);
    }

    // Записываем ключевые байты в файл
    int real_write_bytes_iv = fwrite(iv, 1, AES::BLOCKSIZE, f);
    if (real_write_bytes_iv != AES::BLOCKSIZE) {
        std::cout << "ERROR = real_write_bytes_iv != AES::BLOCKSIZE " << std::endl;
        exit(1);
    }

    fclose(f);
    //std::cout << "AES.iv с ключевыми байтами записан" << std::endl;

    // ------------------------------------
    std::string filename_encrypt =  filename + "_ENC";
    // Открыли файл (с проверкой)
    if ((f = fopen(filename_encrypt .c_str(), "wb")) == NULL)
    {
        std::cout << "Невозможно открыть файл для записи зашифрованного файла!" << std::endl;
        exit(1);
    }
// Записываем зашифрованные байты в файл
    int real_write_bytes = fwrite(cipher.data(), 1, cipher.size(), f);
    if (real_write_bytes != cipher.size()) {
        std::cout << "ERROR = real_write_bytes != cipher.size() " << std::endl;
        exit(1);
    }
    fclose(f);
// =============================================================================

    //std::cout << "File have been encrypted (" << filename_encrypt << ")" << std::endl;
    return 0;
}




int decrypt (const char* filename_for_decrypte)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"decrypt\"]\n");
    using namespace std;
    using namespace CryptoPP;
    const char* filename_c = filename_for_decrypte;
    std::string filename_encrypt (filename_c);
    byte key[AES::MAX_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    vector<byte> plain;
    vector<byte> cipher, recover;
    HexEncoder encoder(new FileSink(cout));

    memset(key, 0x00, sizeof(key));
    memset(iv, 0x00, sizeof(iv));

    // Считывание файла "AES.key"
    // ------------------------------------

    std::string filename_aes_key = "AES.key";
    // Создали указатель на файл
    FILE* f;
    //Открыли файл (с проверкой)
    if ((f=fopen(filename_aes_key.c_str(), "rb"))==NULL)
    {
        std::cout << "Невозможно открыть файл для чтения AES.key!" << std::endl;
        exit(1);
    }

    // //Узнали его размер
    int fsize_key = filesize_share(f);
    //std::cout << "File_size_key = " << fsize_key << std::endl;
    int real_read_bytes = fread(key, 1, fsize_key, f);
    //std::cout << "Real_read_bytes_key = " << real_read_bytes << std::endl;
    fclose(f);

    // ------------------------------------

    // Считывание файла "AES.iv"
    // ------------------------------------
    std::string filename_aes_iv = "AES.iv";
    //Открыли файл (с проверкой)
    if ((f=fopen(filename_aes_iv.c_str(), "rb"))==NULL)
    {
        std::cout << "Невозможно открыть файл для чтения AES.iv!" << std::endl;
        exit(1);
    }

    // //Узнали его размер
    int fsize_iv = filesize_share(f);
    //std::cout << "File_size_iv = " << fsize_iv << std::endl;
    int real_read_bytes_iv = fread(iv, 1, fsize_iv, f);
    //std::cout << "Real_read_bytes_iv = " << real_read_bytes_iv << std::endl;
    fclose(f);
    // ------------------------------------

// =============================================================================

    //Открыли файл (с проверкой)
    if ((f=fopen(filename_encrypt.c_str(), "rb"))==NULL)
    {
        std::cout << "Невозможно открыть файл для чтения зашифрованного файла!" << std::endl;
        exit(1);
    }

    // //Узнали его размер
    int fsize1 = filesize_share(f);
    //std::cout << "File_size_chiper = " << fsize1 << std::endl;

    byte* chiper1 = new byte[fsize1];

    int real_read_bytes1 = fread(chiper1, 1, fsize1, f);
    //std::cout << "Real_read_bytes_chiper = " << real_read_bytes1 << std::endl;


    fclose(f);

    for (int i =0; i < fsize1; i ++)
    {
        cipher.push_back(chiper1[i]);
    }

    delete[] chiper1;

    // --------------------------------------

    /////////////////////////////////////////////////////////////

    CFB_Mode<AES>::Decryption dec;
    dec.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));

    // Recovered text will be less than cipher text
    recover.resize(cipher.size());
    ArraySink rs(&recover[0], recover.size());

    ArraySource(cipher.data(), cipher.size(), true,
                new StreamTransformationFilter(dec, new Redirector(rs)));

    // Set recovered text length now that its known
    recover.resize(rs.TotalPutLength());

    // Удаляем перед сохранением принятый файл (зашифрованный с таким же именем)
    remove(filename_encrypt.c_str());

    std::string filename_decrypt =  filename_encrypt;

    //Открыли файл (с проверкой)
    if ((f=fopen(filename_decrypt.c_str(), "wb"))==NULL)
    {
        std::cout << "Невозможно открыть файл для записи расшифрованного файла: |" << filename_decrypt << "|" << std::endl;
        exit(1);
    }

    // ------------------------------------

// Записываем расшифрованные байты в файл
    int real_write_bytes1 = fwrite(recover.data(), 1, recover.size(), f);
    if (real_write_bytes1 != recover.size()) {
        std::cout << "ERROR = real_write_bytes1 != recover.size() " << std::endl;
        exit(1);
    }

    fclose(f);

    // ------------------------------------

    //std::cout << "Recovered_size = " << recover.size() << std::endl;
    //cout << "Recovered text: ";
    encoder.Put(recover.data(), recover.size());
    encoder.MessageEnd();
    //cout << endl;


    //std::cout << "File have been decrypted (" << filename_decrypt << ")" << std::endl;


    return 0;
}



class client
{
public:
    
    client(boost::asio::io_context& io_context,
           boost::asio::ssl::context& context,
           boost::asio::ip::tcp::resolver::results_type endpoints)
            : socket_(io_context, context)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::client()\"]\n");
        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
        socket_.set_verify_callback(
                boost::bind(&client::verify_certificate, this, _1, _2));

        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
                                   boost::bind(&client::handle_connect, this,
                                               boost::asio::placeholders::error));
    }

    
    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context& ctx)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::verify_certificate()\"]\n");
        //std::cout << "Try Verifying " << std::endl;
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        //std::cout << "Verifying " << subject_name << "\n";
        //sleep(3);
        return preverified;
    }

    
    void handle_connect(const boost::system::error_code& error)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::handle_connect()\"]\n");
        if (!error)
        {
            //std::cout << "try conneecting..." << std::endl;
            socket_.async_handshake(boost::asio::ssl::stream_base::client,
                                    boost::bind(&client::handle_handshake, this,
                                                boost::asio::placeholders::error));
        }
        else
        {
            //std::cout << "Connect failed: " << error.message() << "\n";
        }
    }
    
    void handle_handshake(const boost::system::error_code& error)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::handle_handshake()\"]\n");
        if (this->stop_send_flag !=2)
        {
            if (!error) {

                // Вывод передающихся данных
                //   for (int i = 0; i < this->current_data_length; i++) {
                //       std::cout << "buf[" << i << "] = " << this->buf[i] << std::endl;
                //   }

                //std::cout << "Отправка: ";
                //std::cout << this->current_data_length;
                //std::cout << " байт данных на сервер ..." << std::endl;


                boost::asio::async_write(socket_,
                                         boost::asio::buffer(this->buf, this->current_data_length),
                                         boost::bind(&client::handle_write, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));

                if (this->fsize - this->total == 4)
                    this->buf_size=2;

            } else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        }
        else
        {
            if (!error) {
                this->stop_send_flag = 0;
                if (this->filename != "ZERO FILE")
                {
                    delete[] this->buf;
                    //std::cout << "END of FILE" << std::endl;
                    //std::cout << "File_hh " << this->filename << " sent to server.\n" << std::endl;

                    this->stop_flag = 1;
                }

                this->total = 0;

                // Обновляем информацию о файле
                info_about_file();

            }
            else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        }


    }

    
    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::handle_write()\"]\n");
        if (!error)
        {
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(reply_, max_length),
                                    boost::bind(&client::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::handle_read()\"]\n");
        if (!error) {

            //std::cout << "reply_bytes_transferred = " << bytes_transferred <<std::endl;
            //std::cout << "Reply: ";
            // std::cout<<reply_<<std::endl;
            //std::cout.write(reinterpret_cast<const char*>(reply_), bytes_transferred);
           //std::cout << "\n";

            // Дальнейшая отправка части файла
            if (this->stop_send_flag == 0) {
                //Считали из него "buf_size" (или менее) байт данных
                this->buf = new char[1024];
                int real_read_bytes = fread(this->buf, 1, this->buf_size, this->f);
                this->current_data_length = real_read_bytes;

                this->total = this->total + this->current_data_length;

                if (this->current_data_length == 0)
                {

                    // Закрытие файла
                    fclose(this->f);

                    //std::cout << "END of FILE" << std::endl;

                    this->stop_send_flag = 0;

                    if (this->filename != "ZERO FILE")
                    {
                        delete[] this->buf;
                        //std::cout << "File_hr " << this->filename << " sent to server.\n" << std::endl;
                        //return;
                        this->stop_flag = 1;
                    }

                    this->filename = "ZERO FILE";
                    this->current_data_length = -1;
                    this->info_about_file();
                    this->total = 0;

                    return;
                }

                //std::cout << "Real_read_bytes = " << this->current_data_length << std::endl;
                //std::cout<< "Total = " << this->total << "/" << this->fsize << std::endl;

                //Проверка на окончание файла
                if ((real_read_bytes != this->buf_size) or (feof(this->f)))
                    if (feof(this->f)) {
                        //std::cout << "END of FILE" << std::endl;

                        // Означает ПОСЛЕДНЮЮ отправку порции данных для ТЕКУЩЕГО файла
                        this->stop_send_flag = 1;

                        // Закрытие файла
                        fclose(this->f);
                    }

                // Создаем переменную ошибки для совместимости с handle_handshake
                const boost::system::error_code error_my;
                // Вот он повторный вызов-то !!!
                this->handle_handshake(error_my);

            }
            else
            {
                this->stop_send_flag = 2;

                // Создаем переменную ошибки для совместимости с handle_handshake
                const boost::system::error_code error_my;
                // Вот он повторный вызов-то !!!
                // Но тут "this->buf, real_read_bytes" ужны лишь для совместимости с "handle_handshake"
                this->handle_handshake(error_my);
            }

        } else {
            std::cout << "Read failed: " << error.message() << "\n";
        }

    }

    
    void info_about_file (void)

    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client::info_about_file()\"]\n");
        if (this->stop_flag == 1)
        {

            std::string gen_return_local(reply_);

            gen_return = std::atoi(gen_return_local.c_str());

            return ;
        }

        // Создаем массив char для хранения имени файла
        char filename_char[150];
        for (int i = 0; i < 150; i++)
            filename_char[i] = '\0';

        // Преобразуем массив char в string
        std::string filename1(ARGV_FN);

        this->filename = filename1;

        //Пока что сделаем так (чтобы не вводить длинное название много раз)
        //filename = "Files_for_sending/from_client_to_server.txt";

        //Открыли файл (с проверкой)
        if ((this->f=fopen(this->filename.c_str(), "rb"))==NULL)
        {
            std::cout << "Невозможно открыть файл !" << std::endl;
            exit(1);
        }

        //Узнали его размер
        this->fsize = filesize_share(this->f);
        //std::cout << "File_size = " << this->fsize << std::endl;

        this->buf = reinterpret_cast<char*>(&this->fsize);

        this->current_data_length = sizeof(fsize);

        // Создаем переменную ошибки для совместимости с handle_handshake
        const boost::system::error_code error_my;
        // Вот он повторный вызов-то !!!
        this->handle_handshake(error_my);

    }

    // Указатель на файл
    FILE* f;

    int stop_flag = 0;

    //Размер файла
    int fsize = 0;

    //Определили счетчик считанных байт данных из файла
    int total = 0;

    // Определили, сколько за раз будет считываться даных из файла
    int buf_size = 512;

    // Этот флаг будет срабатывать когда будет достигнут конец файла
    int stop_send_flag = 2;

    //Буфер памяти под считываемые данные из файла
    char* buf;

    // Имя обрабатываемого в данный момент файла
    std::string filename = "ZERO FILE";

    // Текущее значения данных, которое было считано
    int current_data_length = -1;

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    //char request_[max_length];
    char reply_[max_length];
};



// from server_to_client
class client_sc
{
public:
    int bt = 0;

    
    client_sc(boost::asio::io_context& io_context,
              boost::asio::ssl::context& context,
              boost::asio::ip::tcp::resolver::results_type endpoints)
            : socket_(io_context, context)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::client_sc()\"]\n");
        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
        socket_.set_verify_callback(
                boost::bind(&client_sc::verify_certificate, this, _1, _2));

        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
                                   boost::bind(&client_sc::handle_connect, this,
                                               boost::asio::placeholders::error));
    }

    
    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context& ctx)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::verify_certificate()\"]\n");
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        //std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }

    
    void handle_connect(const boost::system::error_code& error)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::handle_connect()\"]\n");
        if (!error)
        {
            socket_.async_handshake(boost::asio::ssl::stream_base::client,
                                    boost::bind(&client_sc::info_about_file, this,
                                                boost::asio::placeholders::error));
        }
        else
        {
            //std::cout << "Connect failed: " << error.message() << "\n";
            this->close();
        }
    }

    
    void handle_handshake(const boost::system::error_code& error)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::handle_handshake()\"]\n");
        if (!error) {


            //std::cout << "handle_handshake. this->filename = " << this->filename << std::endl;
            std::string request = "GET_" + this->filename;

            //std::cout << "Отправка запроса: |";
            //std::cout << request;
            //std::cout << "| на сервер ..." << std::endl;

             //std::cout << "request.length() = " << request.length() << std::endl;

            int length_name = request.length();
            //std::cout << "length_name = " << length_name << std::endl;


            char buf_ [max_length];
            memset(buf_,0,max_length);
            strncpy(buf_,request.c_str(),length_name);


            boost::asio::async_write(socket_,
                                     boost::asio::buffer(buf_, max_length), // max_length
                                     boost::bind(&client_sc::handle_write, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred));


        } else {
            std::cout << "Handshake failed: " << error.message() << "\n";
        }

    }

    
    void handle_write(const boost::system::error_code& error,
                      size_t bytes_transferred)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::handle_write()\"]\n");
        if (!error)
        {
            if (this->flag_end == 1)
            {return;}
            else {
                socket_.async_read_some(boost::asio::buffer(reply_, max_length),
                                        boost::bind(&client_sc::handle_read, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));


                timer_my->expires_from_now(boost::posix_time::microseconds(5000000)); //500000 НОРМАЛЬНЫЙ_ТАЙМАУТ = 1000000
                timer_my->async_wait(boost::bind(&client_sc::close, this));
            }
            //timer_my->async_wait([&](auto err_c) { std::cout << "After 5 seconds" << std::endl; bad_recieve = 1; timer_my = nullptr; return; } );


            //std::cout << "lala" << std::endl;


            // Он handle не возвращает, пока не придет именно то количество байт, которое указано в "length"

            /*
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(reply_, length), // max_length
                                    boost::bind(&client::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
*/
            //std::cout << "Hi_0" <<std::endl;

        }
        else
        {
            std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    void close()
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::close()\"]\n");
        //std::cout << "Не удалось долждатся ответа сервера в течении 5 секунд." << std::endl;
        //socket_.shutdown();
        if (bt == 0) {
            //std::cout << "After 1 seconds no response from server!!!" << std::endl;
            //std::cout << "This connection will be" << std::endl;
            bad_recieve = 1;
            timer_my = nullptr;
            io_my->stop();
            return;
        }

    }

    
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::handle_read()\"]\n");
        // std::cout << "Hi_1" <<std::endl;

        int flag = 0;

        if (!error) {

            //std::cout << "reply_bytes_transferred = " << bytes_transferred <<std::endl;
            bt = bytes_transferred;
            // Обработка длины полученных данных
            // Если принят размер файла
            //std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
            if ((bytes_transferred == 4) and (this->size_flag == 0)) {
                std::string filename = path_to_temp_dir + "$"+ this->filename;
                if ((this->f = fopen(filename.c_str(), "wb")) == NULL) {
                    std::cout << "Невозможно создать/открыть файл !" << std::endl;
                    exit(1);
                }


                //  ....
                this->filesize = *(reinterpret_cast<int *>(reply_));

                //std::cout << "Размер нового принимаемого файла равен = " << this->filesize << std::endl;

                for (int i = 0; i < 1024; i++)
                    this->responde[i] = '\0';

                this->responde[0] = 'I';
                this->responde[1] = ' ';
                this->responde[2] = 'R';
                this->responde[3] = 'F';
                this->responde[4] = 'S';

                int respone_size = strlen(this->responde) + 1;


                //std::cout << "Отправляю ответ !" << std::endl;
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(this->responde, max_length), // max_length
                                         boost::bind(&client_sc::handle_write, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));

                this->size_flag = 1;
                return;
            }




            if (this->size_flag == 1) {

                // Увеличиваем счетчик на количество принятых байт
                this->total = this->total + bytes_transferred;

                //std::cout<< "Total = "<<this->total<<"/"<<this->filesize<<std::endl;

                // Записывае принятые байты в файл
                int real_write_bytes = fwrite(reply_, 1, bytes_transferred, this->f);
                if (real_write_bytes != bytes_transferred) {
                    std::cout << "ERROR = real_write_bytes != bytes_transferred " << std::endl;
                    exit(1);
                }



                // Файл принят полностью
                if (this->total == this->filesize) {
                    fclose(this->f);
                    this->filesize = 0;
                    this->total = 0;
                    this->current_file_name = this->current_file_name + 1;

                    //std::cout << "Файл " << this->filename << " принят полностью" << std::endl;

                    for (int i = 0; i < 1024; i++)
                        this->responde[i] = '\0';


                    this->responde[0] = 'I';
                    this->responde[1] = ' ';
                    this->responde[2] = 'F';
                    this->responde[3] = 'U';
                    this->responde[4] = 'L';
                    this->responde[5] = 'L';
                    this->responde[6] = 'R';
                    this->responde[7] = 'F';

                    this->flag_size = 0;

                    delete[] this->reply_;


                    this->flag_end = 1;

                    int respone_size = strlen(this->responde) + 1;

                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(this->responde, max_length), // max_length
                                             boost::bind(&client_sc::handle_write, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

                    //   return;

                    flag = 1;
                    this->size_flag = 0;


                }

                // Просмотр принимаемых данных

                //std::cout << "Recieved: ";

                //for (int i = 0; i < bytes_transferred; i++) {
                //   std::cout << data_[i];
                //}

                if (flag == 0) {
                    //std::cout << "\n" << std::endl;


                    for (int i = 0; i < 1024; i++)
                        this->responde[i] = '\0';

                    this->responde[0] = 'O';
                    this->responde[1] = 'K';

                    int respone_size = strlen(this->responde) + 1;

                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(this->responde, max_length), // max_length
                                             boost::bind(&client_sc::handle_write, this,
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred));

                }
            }


        } else {
            std::cout << "Read failed: " << error.message() << "\n";
        }

    }

    
    void info_about_file (const boost::system::error_code& error)

    {
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"client_sc::info_about_file()\"]\n");
        this->reply_ = new char[max_length];
        // Создаем массив char для хранения имени файла
        char filename_char[150];
        for (int i = 0; i < 150; i++)
            filename_char[i] = '\0';

        // Преобразуем массив char в string
        std::string filename(ARV_FN);

        //filename = filename + ".dat";

        //Пока что сделаем так (чтобы не вводить длинное название много раз)
        // filename = "43434.mp4";

        this->filename = filename;



        // Создаем переменную ошибки для совместимости с handle_handshake
        const boost::system::error_code error_my;
        // Вот он повторный вызов-то !!!
        this->handle_handshake(error_my);


    }

    // public variabels

    // ----------------------------

    int flag_size = 0;

    int flag_end = 0;

    int filesize;
    int total = 0;

    FILE* f;

    int size_flag = 0;
    int current_file_name = 0;

    std::string filename = "NO FILE";

    char responde[1024] = {'n','o',' ','r','e','s','p','o','n','d'};

    // ------------------------------

private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    char* reply_;
};



int send_file_to_server (const char* hostname, int port, const char* filename_for_sending_server)
{

	gen_return = -1;
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"send_file_to_server\"]\n");
    try
    {
        std::string port_str = std::to_string(port);
        boost::asio::io_context io_context;

        // const char* a1 = "localhost";
        // const char* a2 = "2041";

        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints =
                // resolver.resolve(a1, a2);
                resolver.resolve(hostname, port_str.c_str());

        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.load_verify_file("rootca.crt");

        client c(io_context, ctx, endpoints);

        ARGV_FN = filename_for_sending_server;

        io_context.run();

        //Закрыли файл
        //fclose(c.f);

        //std::cout << "RETURNING = " << gen_return <<std::endl;
        io_context.stop();

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    //  Возвращаем номер файла на сервере (то как он будет называться: "Store/<return>.dat")
    return gen_return;

}


int recieve_from_server (const char* hostname, int port, const char* filename_for_request_from_server)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"recieve_from_server\"]\n");
    try
    {

        boost::asio::io_context io_context;

        std::string port_str = std::to_string(port);

        // const char* a1 = "localhost";
        // const char* a2 = "2041";

        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints =
                // resolver.resolve(a1, a2);
                resolver.resolve(hostname, port_str.c_str());

        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.load_verify_file("rootca.crt");

        client_sc c(io_context, ctx, endpoints);

        ARV_FN = filename_for_request_from_server;

        boost::asio::deadline_timer timer(io_context);
        timer_my = &timer;

        io_my = &io_context;
        io_context.run();

        //Закрыли файл (на всякий случай тут тоже дублируем)
        //fclose(c.f);

        if (bad_recieve == 0)
        { }
        else
        {

            bad_recieve = 0;
            return 1;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}





void write_object_to_file(Reference_File* rf, const char* path_to_reference_file_c_str)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"write_object_to_file\"]\n");
    std::ofstream fout;
    std::string path_to_reference_file (path_to_reference_file_c_str);
    fout.open(path_to_reference_file, std::ofstream::out);
    if (!fout.is_open())
    {
        std::cout << "Ошибка открытия файла: " << path_to_reference_file << std::endl;
        exit(1);
    }
    fout.write((char*)rf, sizeof(Reference_File));
    fout.close();
    //std::cout << "Файл-ссылка " << path_to_reference_file_c_str << " успешно записан." << std::endl;
    return;
};


int read_object_from_file(Reference_File* rf, const char* path_to_reference_file_c_str)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"read_object_from_file\"]\n");
    std::ifstream fin;
    std::string path_to_reference_file (path_to_reference_file_c_str);
    fin.open(path_to_reference_file, std::ifstream::binary);
    if (!fin.is_open())
    {
        std::cout << "Ошибка открытия файла: " << path_to_reference_file << std::endl;
	return 1;        
	//exit(1);
    }

    fin.read((char*)rf, sizeof(Reference_File));
    fin.close();
    //std::cout << "Файл-ссылка " << path_to_reference_file_c_str << " успешно считан." << std::endl;
    return 0;

};


int share_file(const char* filename_for_share_c_str, const char* filename_shared_reference_file_c_str)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"share_file\"]\n");
    //================================== РАЗЛОЖИЛИ ФАЙЛ ==== BEGIN =========================================================
    //====================================================================================================================


    
    //const int N_serv_addr = 10;
    std::vector< std::string > servers_address;
    //std::string servers_address[N_serv_addr];

    // Заполняем доступные адреса серверов, которые готовы хранить файлы
    int i_ = 0;

    	// Считать из файла адреса IPv6 (15_09_21)
    	std::fstream file("Yggdrasil_addresses.txt");
    	std::string temp;
    	while( !file.eof() ) {
        std::getline( file, temp );
        servers_address.push_back( temp );
        //std::cout << "Yggdrasil_addresses["<<i_<<"] = " << temp << std::endl;
        i_++;
    	}

        //servers_address[i] = "127.0.0.1"; // Пока что сделаем все одинаковые, а потом нужно будет либо в конфиг файле это указывать, либо хз)))
    

    const int N_serv_addr = servers_address.size() - 1;
    //std::cout << "N_serv_addr = " << N_serv_addr << std::endl;
    fflush(stdout);
    //sleep(5);


    // filename_for_share_c_str - это путь к файлу ОТНОСИТЕЛЬНО директории "Big_files_dir_output"
    std::string filename_for_share (filename_for_share_c_str);

    // filename_shared_reference_file_c_str - это путь к файлу ОТНОСИТЕЛЬНО директории "Big_files_dir_input"
    std::string filename_shared_reference_file (filename_shared_reference_file_c_str);
    std::string filename = Big_files_dir_output + filename_for_share;
    std::string filename_old = filename;
    std::string filename_in = Big_files_dir_input + filename_for_share_c_str;
    //std::string execute_path (argv[0]);
    //std::cout << "get_path( ) = |" << get_path() << "|" <<std::endl;
    std::string path_buf = get_path();
    int stop_symbol = 0;
    for (int i = path_buf.length()-1; ;i--)
    {
        if (path_buf[i] == '/')
        {
            stop_symbol = i;
            break;        
        }
            
        
    }

    std::string path = path_buf.substr (0,stop_symbol + 1); // +1 - чтобы захватить '/'
    //std::cout << "path = |" << path << "|" <<std::endl;
    //sleep(10); 
    std::string filename_in_for_check =  path + filename;
    int check_res = checkFile(filename_in_for_check.c_str());
    if (check_res == -1)
    {
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(31); 
        std::cout << "Файл '" << filename_in_for_check << "' не найден." << std::endl;
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(32);
        std::cout << "Проверьте правильность введенных данных." << std::endl;
        fflush(stdout);
        sleep(3);
        return 1;                
    }

    // Зашифровать файл
    int enc_res_0 = encrypt (filename.c_str());

    if (enc_res_0 == 1)
	{
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(31); 
		std::cout << "При отправке файла ('Этап шифрования') произошла ошибка." << std::endl;
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(32);
		std::cout << "Проверьте правильность введенных данных." << std::endl;
		sleep(3);
		fflush(stdout);
		return 1;
		
	}
    set_cursor_position (start_line + 1, 4);
    start_line++;
    set_color(32);


    //std::cout << "Name of encrypted_file: " << filename.c_str() << std::endl;
    //user_pause();


    //printf("test: 1\n");
    //user_pause();

    // Создаем объект Refernce_File
    Reference_File rf;
    //memset(&rf,0,sizeof(Reference_File));

    // Считываем файл AES.key и AES.iv в Reference File
    FILE* ff;

    // Открыли AES.key файл (с проверкой)
    if ((ff = fopen("AES.key", "rb")) == NULL) {
        std::cout << "Невозможно открыть файл !" << std::endl;
        exit(1);
    }
    // Узнаем размер файла (зашифрованного)
    int file_size_0 = filesize_share(ff);

    //std::cout << "Размер файла AES.key = " << file_size_0 << std::endl;

    unsigned char buf_key [file_size_0];
    memset(buf_key,0,file_size_0);

    //printf("test: 2\n");
    //user_pause();

    // Загружаем файл (зашифрованный) в оперативную память
    int real_read_bytes_0 = fread(buf_key, 1, file_size_0, ff);
    fclose(ff);
    // Проверка на правильность считывания
    if (real_read_bytes_0 != file_size_0) {
        std::cout << "Считывание файла AES.key! (real_read_bytes_0 != file_size_0) " << std::endl;
        exit(1);
    }

    memset(rf.buf_key,0,CryptoPP::AES::MAX_KEYLENGTH);
    memset(rf.buf_iv,0,CryptoPP::AES::BLOCKSIZE);
    // Записываем этот ключ в определенное поле в Reference File
    for (int i =0; i < real_read_bytes_0; i++)
    {
        rf.buf_key[i] = buf_key[i];
    }

    // Открыли AES.iv файл (с проверкой)
    if ((ff = fopen("AES.iv", "rb")) == NULL) {
        std::cout << "Невозможно открыть файл !" << std::endl;
        exit(1);
    }
    // Узнаем размер файла (зашифрованного)
    int file_size_1 = filesize_share(ff);
    //std::cout << "Размер файла AES.iv = " << file_size_1 << std::endl;

    unsigned char buf_iv [file_size_1];
    memset(buf_iv,0,file_size_1);

    //printf("test: 3\n");
    //user_pause();

    // Загружаем файл (зашифрованный) в оперативную память
    int real_read_bytes_1 = fread(buf_iv, 1, file_size_1, ff);
    fclose(ff);
    // Проверка на правильность считывания
    if (real_read_bytes_1 != file_size_1) {
        std::cout << "Считывание файла AES.iv! (real_read_bytes_1 != file_size_1) " << std::endl;
        exit(1);
    }
    // Записываем этот ключ в определенное поле в Reference File
    for (int i =0; i < real_read_bytes_1; i++)
    {
        rf.buf_iv[i] = buf_iv[i];
    }

    rf.key_size = real_read_bytes_0;
    rf.iv_size = real_read_bytes_1;

    // Удаляем файлы AES.key и AES.iv
    remove("AES.key");
    remove("AES.iv");

    filename = filename + "_ENC";
    // Вычислить общий хэш зашифрованного файла
    std::string hash_of_all_file = count_hash (filename.c_str());
    const char* hash_of_all_file_c = hash_of_all_file.c_str();

    //printf("test: 4\n");
    //user_pause();

    int m1;
    for (int i = 0; i < hash_of_all_file.length()+1; i++) //rf.max_len_hash
    {
        if (hash_of_all_file_c[i] == '\0')
        {
            m1 = i;
            break;
        }
        rf.hash_of_all_file[i] = hash_of_all_file_c[i];
    }

    rf.hash_of_all_file[m1] = '\0';

    //printf("test: 4.1\n");
    //user_pause();

    FILE *f;

    // Открыли зашифрованный файл (с проверкой)
    if ((f = fopen(filename.c_str(), "rb")) == NULL) {
        std::cout << "Невозможно открыть файл !" << std::endl;
        exit(1);
    }

    //printf("test: 4.2\n");
    //user_pause();

    // Узнаем размер файла (зашифрованного)
    int file_size = filesize_share(f);
    //std::cout << "file_size of encrypted file = " << file_size << std::endl;


    //printf("test: 5\n");
    //user_pause();
    // Выделяем динамический массив (char== byte)
    unsigned char *buf = new unsigned char[file_size];

    // Загружаем файл (зашифрованный) в оперативную память
    int real_read_bytes = fread(buf, 1, file_size, f);

    fclose(f);
    // Проверка на правильность считывания
    if (real_read_bytes != file_size) {
        std::cout << "Считывание файла для разбиения не выполнено! (real_read_bytes != file_size) " << std::endl;
        exit(1);
    }

    /* initialize random seed: */
    srand(time(NULL));

    // Случайным образом устанавливаем число N (количество реальных файлов для разбиения)


    //printf("test: 6\n");
    //user_pause();
    int N_files = (rand() % file_size + 1) % 50 + 1; // +5 (в любом случае файлов не будет более 50)
    //printf("N_files = %d|\n",N_files);
    fflush(stdout);
    /* generate secret number between 3 and 6: */
    // int N_files = rand() % 3 +3;

    // Случайным образом устанавливаем размер каждого файла (но не более какого-то, чтобы не превысить на всех N_files размер файла file_size)
    int max_size = file_size / N_files;

    // -------- Подсказка ---------------

    // v1 = rand() % 100;         // v1 in the range 0 to 99
    // v2 = rand() % 100 + 1;     // v2 in the range 1 to 100
    // v3 = rand() % 30 + 1985;   // v3 in the range 1985-2014

    //-----------------------------------

    // Создаем массив размеров для будущих файлов
    int N_files_size[N_files];

    int total_memory = 0;
    // (-1) очень важен !!!
    for (int i = 0; i < N_files - 1; i++) {
        // random == from 1 to max_size
        N_files_size[i] = rand() % max_size + 1; 
        total_memory = total_memory + N_files_size[i];

    }

    //Все оставшееся присваиваем последнему размеру файла
    N_files_size[N_files - 1] = file_size - total_memory;

    // Создаем массив файлов из одного общего
    int total = 0;
    // Разбиваем наш общий файл на мелкие файлы
    for (int i = 0; i < N_files; i++) {

        // Создали имя для временного файла
        std::string filename_part = path_to_temp_dir + std::to_string(i) + "_.dat";

        //Открыли файл (с проверкой)
        if ((f = fopen(filename_part.c_str(), "wb")) == NULL) {
            std::cout << "Невозможно открыть файл " << filename_part << " для записи в него части основного файла!"
                      << std::endl;
            exit(1);
        }

        // Записывае часть общего файла в temp файл
        int real_write_bytes = fwrite(&buf[total], 1, N_files_size[i], f);
        if (real_write_bytes != N_files_size[i]) {
            std::cout << "ERROR = real_write_bytes != N_files_size[i], where i = " << i << std::endl;
            exit(1);
        }

        fclose(f);
        total = total + N_files_size[i];

    }

    // Пауза для анализа произошедшего
     //user_pause();

    // Добавляем фейковые файлы K_files (от 1-го файла до 25 % от общего числа рельных маленьких файлов (N_files) [Случайно генерирум массивы байтов случайного рамера и записываем их в файлы])
    int K_files = rand() % (int) (N_files * 0.75) + 1;
    //std::cout << "K_files = " << K_files << std::endl;
    unsigned char K_files_size[K_files];

    for (int i = 0; i < K_files; i++) {
        K_files_size[i] = rand() % max_size + 1;
    }

    for (int i = 0; i < K_files; i++) {
        // Создали имя для временного файла
        std::string filename_part = path_to_temp_dir + std::to_string(N_files + i) + "_.dat";

        //Открыли файл (с проверкой)
        if ((f = fopen(filename_part.c_str(), "wb")) == NULL) {
            std::cout << "Невозможно открыть файл " << filename_part << " для записи в него части основного файла!"
                      << std::endl;
            exit(1);
        }

        // Генерим случайный файл размером K_files_size[i]
        unsigned char buf1[K_files_size[i]];

        for (int ii = 0; ii < K_files_size[i]; ii++)
        {
            buf1[ii] = 1 + rand() %  254;
        }

        // Записывае часть общего файла в temp файл
        int real_write_bytes1 = fwrite(buf1, 1, K_files_size[i], f);
        if (real_write_bytes1 != K_files_size[i]) {
            std::cout << "ERROR = real_write_bytes != K_files_size[i], where i = " << i << std::endl;
            exit(1);
        }

        fclose(f);
    }

    // Пауза для анализа произошедшего
     //user_pause();

    // Перемешиваем названия файлов и сохраняем список соответствия в отдельный файл (или в оперативную память)
    // Заполняем множество элементов - цифрами файлов
    std::set<int> st;
    for (int i = 0; i < N_files + K_files; i++) {
        st.insert(i);
    }

    std::set<int>::iterator it;

    int new_file_names_int[N_files + K_files];

    for (int i = 0; i < N_files + K_files; i++) {
        int count_it = rand() % st.size() + 0;
        it = st.begin();
        for (int j = 0; j < count_it; j++) {
            it++;
        }
        new_file_names_int[i] = *it;
        st.erase(*it);
        //std::cout << "new_file_names_int[" << i << "] = " << new_file_names_int[i] << std::endl;
    }



    //printf("test: 7\n");
    //user_pause();
    // Переименовываем файлы
    for (int i = 0; i < N_files + K_files; i++) {
        std::string file_name_i_old = path_to_temp_dir + std::to_string(i) + "_.dat";
        std::string file_name_i_new = path_to_temp_dir + std::to_string(new_file_names_int[i]) + ".dat";

        rf.switch_table_first[i] = i;
        rf.switch_table_second[i] = new_file_names_int[i];

        int res_rename = rename(file_name_i_old.c_str(), file_name_i_new.c_str());
        if (res_rename != 0) {
            std::cout << "При переименовании файла " << file_name_i_old << " на файл " << file_name_i_new
                      << " ПРОИЗОШЛА ОШИБКА." << std::endl;
            exit(1);
        }

    }


    //printf("test: 8\n");
    //user_pause();

    // Необходимо определиться, какую часть файла у себя оставить (только часть реального файла, а не фейковая)
    int flag = 0; // Когда найдется нужный случайный файл, то он станет 1
    int count_trys = 0;
    int part_here;
    while ((flag == 0) && count_trys != 1000)
    {
        count_trys++;
        part_here = rand() % (N_files + K_files) + 0;
        for (int i = 0; i < N_files + K_files; i++)
        {
            if (rf.switch_table_second[i] == part_here)
            {
                if (rf.switch_table_first[i] < N_files)
                {
                    std::string filename_part_here =path_to_temp_dir + std::to_string(part_here) + ".dat";
                    if ((f = fopen(filename_part_here.c_str(), "rb")) == NULL) 
                    {
                        std::cout << "Невозможно открыть файл " << filename_part_here << " для чтения из него части основного файла!" << std::endl;
                        exit(1);
                    }
                    int filesize_part_here = filesize_share(f);
                    fclose(f);
                    if (!(filesize_part_here > rf.max_data_size_of_part_here - 1)) 
                    {
                        flag = 1;
                        //std::cout << "Для оставления выбран файл №" << rf.switch_table[i].first << " что меньше чем " << N_files << ". и по размеру меньше, чем 1000 (max_data_size_of_part_here)" << std::endl;
                    }
                }
            }
        }

    }
    if ((count_trys == 1000) && (flag == 0))
    {
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(31); 
        std::cout << "Произошло несовместимое с алгоритмами работы системы разбиение файла на части." << std::endl;
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(32);
        std::cout << "Файл не будет отправлен в размещенное хранилище. Пожалуйста, попытайтесь еще раз." << std::endl;
        fflush(stdout);
        sleep(10);
        return 1;
    }

    //printf("test: 9\n");
    //user_pause();
    //Открыли файл (с проверкой)
    std::string filename_part_here =path_to_temp_dir + std::to_string(part_here) + ".dat";
    if ((f = fopen(filename_part_here.c_str(), "rb")) == NULL) {
        std::cout << "Невозможно открыть файл " << filename_part_here << " для чтения из него части основного файла!"
                  << std::endl;
        exit(1);
    }

    int filesize_part_here = filesize_share(f);

    int real_read_bytes2 = fread(rf.part_here_data, 1, filesize_part_here, f);
    if (real_read_bytes2 != filesize_part_here)
    {
        std::cout << "ERROR. real_read_bytes2 != filesize_part_here." << std::endl;
        exit(1);
    }
    fclose(f);

    // Удаляем тот мини-файл, который оставим у себя
    remove(filename_part_here.c_str());



    //printf("test: 10\n");
    //user_pause();
    // Пауза для анализа произошедшего
    // user_pause();

    // Сохраняем необходимые данные в объект Refernce_File
    rf.real_number_files = N_files + K_files;
    rf.real_files = N_files;
    rf.fake_files = K_files;
    rf.part_here_file = part_here;
    rf.size_part_here_file = filesize_part_here;


    //printf("test: 11\n");
    //user_pause();

    // Устанавливаем хэши для всех файлов
    for (int i = 0; i < N_files + K_files; i++) {
        // Вычисляем хэш каждого маленького зашифрованного файла
        if (i == part_here)
        {
            continue;
        }
        std::string file_i_name_str = path_to_temp_dir + std::to_string(i) + ".dat";
        std::string hash_of_file_i = count_hash (file_i_name_str.c_str());
        const char* hash_of_file_i_c = hash_of_file_i.c_str();

        int m2;
        for (int j = 0; j < hash_of_file_i.length()+1; j++) // rf.max_len_hash
        {
            if (hash_of_file_i_c[j] == '\0')
            {
                m2 = j;
                break;
            }
            rf.hash_each_file[i][j] = hash_of_file_i_c[j];
        }

        rf.hash_each_file[i][m2] = '\0';
    }


    //printf("test: 12\n");
    //user_pause();

    // Устанавливаем имя файла для сборки
    const char* filename_in_c = filename_in.c_str();

    int m4;
    for (int i = 0; i < filename_in.length() + 1; i++) //rf.max_len_filename
    {
        if (filename_in_c[i] == '\0')
        {
            m4 = i;
            break;
        }
        rf.filename[i] = filename_in_c[i];
    }

    rf.filename[m4] = '\0';

    // Выгружаем из оперативной памяти файл (зашифрованный) // delete buf[];
    delete[] buf;



    //printf("test: 13\n");
    //user_pause();

    // Определяем addr1, addr2, addr3 для каждого из файлов
    for (int i = 0; i < N_files + K_files; i++) // -1 -> т.к. мы должны учесть filename_part_here
    {
        if (i == part_here)
        {
            continue;
        }

        // Дублирования
        int random_i_1 = 0 + rand() % N_serv_addr; // могут и одинаковые попасться (с одной стороны это плохо, с другой - не очень)
        int random_i_2 = 0 + rand() % N_serv_addr; // могут и одинаковые попасться (с одной стороны это плохо, с другой - не очень)
        while (random_i_1 == random_i_2)
        {
        	random_i_2 = 0 + rand() % N_serv_addr;
        }
        int random_i_3 = 0 + rand() % N_serv_addr; // могут и одинаковые попасться (с одной стороны это плохо, с другой - не очень)
        while ((random_i_3 == random_i_2) || (random_i_3 == random_i_1))
        {
        	random_i_3 = 0 + rand() % N_serv_addr;
        }
        const char* servers_address_i_c_1 = servers_address[random_i_1].c_str();
        const char* servers_address_i_c_2 = servers_address[random_i_2].c_str();
        const char* servers_address_i_c_3 = servers_address[random_i_3].c_str();

        int m3;
        for (int j = 0; j < rf.max_len_inet_addr; j++)
        {
            if (servers_address_i_c_1[j] == '\0')
            {
                m3 = j;
                break;
            }
            rf.addr_1[i][j] = servers_address_i_c_1[j];
        }
        rf.addr_1[i][m3] = '\0';
        m3 = 0;

        for (int j = 0; j < rf.max_len_inet_addr; j++)
        {
            if (servers_address_i_c_2[j] == '\0')
            {
                m3 = j;
                break; 
            }
            rf.addr_2[i][j] = servers_address_i_c_2[j];
        }
        rf.addr_2[i][m3] = '\0';
        m3 = 0;

        for (int j = 0; j < rf.max_len_inet_addr; j++)
        {
            if (servers_address_i_c_3[j] == '\0')
            {
                m3 = j;
                break;
            }
            rf.addr_3[i][j] = servers_address_i_c_3[j];
        }
        rf.addr_3[i][m3] = '\0';

 // И сразу же тут отправляем наш файл
        std::string filename_i = path_to_temp_dir + std::to_string(i) + ".dat";
        // Отправляем по различным серверам наши файлы (кроме "filename_part_here", который мы оставили у себя - сделать на это обработку)
        int filename_i_on_server_int_1 = send_file_to_server(const_cast<const char*>(rf.addr_1[i]), port_cs, filename_i.c_str());
        int filename_i_on_server_int_2 = send_file_to_server(const_cast<const char*>(rf.addr_2[i]), port_cs, filename_i.c_str());
        int filename_i_on_server_int_3 = send_file_to_server(const_cast<const char*>(rf.addr_3[i]), port_cs, filename_i.c_str());
        // Заполняем массив пар имен switch_table_servers_filename

        //std::cout << "1" << std::endl;
        //fflush(stdout);
        //user_pause();

        
        std::string buf_str = std::to_string(i) + ".dat";
        memset(&rf.switch_table_servers_filename_first[i][0],0,256);
        memcpy(&rf.switch_table_servers_filename_first[i][0], const_cast <char*>(buf_str.c_str()), buf_str.length() + 1);
        

        buf_str = std::to_string(filename_i_on_server_int_1) + ".dat";
        memset(&rf.switch_table_servers_filename_second[i][0][0],0,256);
        memcpy(&rf.switch_table_servers_filename_second[i][0][0], const_cast <char*>(buf_str.c_str()), buf_str.length() + 1);



        buf_str = std::to_string(filename_i_on_server_int_2) + ".dat";
        memset(&rf.switch_table_servers_filename_second[i][1][0],0,256);
        memcpy(&rf.switch_table_servers_filename_second[i][1][0], const_cast <char*>(buf_str.c_str()), buf_str.length() + 1);


        buf_str = std::to_string(filename_i_on_server_int_3) + ".dat";
        memset(&rf.switch_table_servers_filename_second[i][2][0],0,256);
        memcpy(&rf.switch_table_servers_filename_second[i][2][0], const_cast <char*>(buf_str.c_str()), buf_str.length() + 1);

        //user_pause();

        //rf.switch_table_servers_filename[i].first = std::to_string(i) + ".dat"; // имя у нас
        //rf.switch_table_servers_filename[i].second = std::to_string(filename_i_on_server_int) + ".dat"; // имя на сервере

        // Удаляем очередной мини-файл
        remove(filename_i.c_str());
        //std::cout << i << "-ый файл передан" << std::endl;
        //sleep(2);

    }


    //printf("test: 14\n");
    //user_pause();

    // Записываем объект Reference_File в файл

    //std::cout << "rf[write] = " << std::endl;
    //rf.print();
    //user_pause();

    write_object_to_file(&rf, filename_shared_reference_file_c_str);

    // Выводим на экран объект Reference_File
    //rf.print();
    //user_pause();

    // Удалим зашифрованный оригинал и файл-оригинал
    remove(filename.c_str());
    remove(filename_old.c_str());

    //std::cout << "Алгоритм распрделения файла по сети отработал без ошибок.\n\n\n" << std::endl;

    //================================== РАЗЛОЖИЛИ ФАЙЛ ==== END =========================================================
    //====================================================================================================================

    return 0;
}


int union_file(const char* filename_reference_file_c_str)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"union_file\"]\n");
    //================================== СОБРАЛИ ФАЙЛ ==== BEGIN =========================================================
    //====================================================================================================================


    std::string path_buf = get_path();
    int stop_symbol = 0;
    for (int i = path_buf.length()-1; ;i--)
    {
        if (path_buf[i] == '/')
        {
            stop_symbol = i;
            break;        
        }
            
        
    }

    std::string path = path_buf.substr (0,stop_symbol + 1); // +1 - чтобы захватить '/'
    //std::cout << "path = |" << path << "|" <<std::endl;
    //sleep(10); 
    std::string filename_in_for_check =  path + filename_reference_file_c_str;

    int check_res = checkFile(filename_in_for_check.c_str());
    if (check_res == -1)
    {
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(31); 
        std::cout << "Файл '" << filename_in_for_check << "' не найден." << std::endl;
        set_cursor_position (start_line + 1, 4);
        start_line++;
        set_color(32);
        std::cout << "Проверьте правильность введенных данных." << std::endl;
        fflush(stdout);
        sleep(3);
        return 1;                
    }


    //filename_reference_file_c_str - это путь к файлу-ссылке ОТНОСИТЕЛЬНО директории "какая-нибудь там..."
    std::string filename_reference_file (filename_reference_file_c_str);

    // filename_union_file_c_str - это путь к файлу ОТНОСИТЕЛЬНО директории "Big_files_dir_input"
    // std::string filename_union_file (filename_union_file_c_str);

    //std::string filename_in = Big_files_dir_input + filename_union_file;

    // Создаем объект Refernce_File
    Reference_File rf;
    //memset(&rf,0,sizeof(Reference_File));
    // Считываем из файла объект Reference_File
    int res_read_obj = read_object_from_file(&rf, filename_reference_file_c_str);
    if (res_read_obj == 1)
	{
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(31); 
		std::cout << "При запросе файла из распределенного хранилища произошла ошибка." << std::endl;
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(32);
		std::cout << "Проверьте правильность введенных данных." << std::endl;
		sleep(2);
		fflush(stdout);
		return 1;
	}


	//std::cout << "rf[read] = " << std::endl;
	//rf.print();
	//user_pause();
    // Выводим на экран объект Reference_File
    //rf.print();
    //user_pause();

    // Считываем файл AES.key и AES.iv из Reference File
    FILE* ff0;

    // Открыли AES.key файл (с проверкой)
    if ((ff0 = fopen("AES.key", "wb")) == NULL) {
        std::cout << "Невозможно открыть файл !" << std::endl;
        exit(1);
    }

    // Загружаем файл (зашифрованный) в оперативную память
    int real_write_bytes_0 = fwrite(rf.buf_key, 1, rf.key_size, ff0);
    fclose(ff0);
    // Проверка на правильность считывания
    if (real_write_bytes_0 != rf.key_size) {
        std::cout << "Запись файла AES.key! (real_write_bytes_0 != rf.key_size) " << std::endl;
        exit(1);
    }

    // Открыли AES.iv файл (с проверкой)
    if ((ff0 = fopen("AES.iv", "wb")) == NULL) {
        std::cout << "Невозможно открыть файл !" << std::endl;
        exit(1);
    }

    // Загружаем файл (зашифрованный) в оперативную память
    int real_write_bytes_1 = fwrite(rf.buf_iv, 1, rf.iv_size, ff0);
    fclose(ff0);
    // Проверка на правильность считывания
    if (real_write_bytes_1 != rf.iv_size) {
        std::cout << "Запись файла AES.iv! (real_write_bytes_1 != rf.iv_size) " << std::endl;
        exit(1);
    }

    // Достуим скачали все файлы (кроме "rf.part_here_file", который мы оставили у себя - сделать на это обработку)
    for (int i = 0; i < rf.real_number_files; i++)
    {
        if (i == rf.part_here_file)
        {
            continue;
        }
        // Запрашиваем у серверов необходимые файлы
        int answered_server_number = 0;
        //std::cout << "const_cast<const char*>(&rf.switch_table_servers_filename_second[i][0][0])= " << const_cast<const char*>(&rf.switch_table_servers_filename_second[i][0][0]) << std::endl;
        int res_recieve = recieve_from_server (const_cast<const char*>(rf.addr_1[i]), port_sc, const_cast<const char*>(&rf.switch_table_servers_filename_second[i][0][0]));
        //std::cout << "Запрос файла: '" << reinterpret_cast<char*>(&rf.switch_table_servers_filename_second[i][0][0]) << "' с сервера [" << const_cast<const char*>(rf.addr_1[i]) << "] ...";
        if (res_recieve != 0)
        {
        	answered_server_number = 1;
            //std::cout << "1-ый сервер не ответил." << std::endl;
            //user_pause();
            //std::cout << "const_cast<const char*>(&rf.switch_table_servers_filename_second[i][1][0])= " << const_cast<const char*>(&rf.switch_table_servers_filename_second[i][1][0]) << std::endl;
            int res_recieve_b1 = recieve_from_server (const_cast<const char*>(rf.addr_2[i]), port_sc, const_cast<const char*>(&rf.switch_table_servers_filename_second[i][1][0]));
            //std::cout << "Запрос файла: '" << reinterpret_cast<char*>(&rf.switch_table_servers_filename_second[i][1][0]) << "' с сервера [" << const_cast<const char*>(rf.addr_2[i]) << "] ...";
            if (res_recieve_b1 != 0)
            {
            	answered_server_number = 2;
                //std::cout << "2-ый сервер не ответил." << std::endl;
                //user_pause();
                //std::cout << "const_cast<const char*>(&rf.switch_table_servers_filename_second[i][2][0])= " << const_cast<const char*>(&rf.switch_table_servers_filename_second[i][2][0]) << std::endl;
                int res_recieve_b2 = recieve_from_server (const_cast<const char*>(rf.addr_3[i]), port_sc, const_cast<const char*>(&rf.switch_table_servers_filename_second[i][2][0]));
                //std::cout << "Запрос файла: '" << reinterpret_cast<char*>(&rf.switch_table_servers_filename_second[i][2][0]) << "' с сервера [" << const_cast<const char*>(rf.addr_3[i]) << "] ...";
                if (res_recieve_b2 != 0)
                {
                    //std::cout << "3-ый сервер не ответил." << std::endl;
                    set_cursor_position (start_line + 1, 4);
                    start_line++;
                    set_color(32);
                    std::cout << "При запросе файла: " <<  reinterpret_cast<char*>(&rf.switch_table_servers_filename_second[i][2][0]) << " произошла ошибка.";
                    fflush(stdout);
                    set_cursor_position (start_line + 1, 4);
                    start_line++;
                    //exit(1);
                    //continue;
                    set_color(34);
                    std::cout << "Ни один из серверов не прислал необходимый файл: '" << reinterpret_cast<char*>(&rf.switch_table_servers_filename_first[i][0]) <<"'.";
                    fflush(stdout);
                    set_cursor_position (start_line + 1, 4);
                    start_line++;
                    set_color(31);
                    std::cout << "Запрос файла из распределенного хранилища завершился с ошибкой.";
					sleep(10);
					fflush(stdout);
					return 1;
                }
            }
        }

        // Переименовываем правильно файлы
        std::string old_name = path_to_temp_dir + "$" + std::string(const_cast<const char*>(reinterpret_cast<char*>(&rf.switch_table_servers_filename_second[i][answered_server_number][0])));
        std::string new_name = path_to_temp_dir + std::string(const_cast<const char*>(reinterpret_cast<char*>(&rf.switch_table_servers_filename_first[i][0])));
        rename(old_name.c_str(), new_name.c_str());
    }

    // Проверяем все хэши
    // Устанавливаем хэши для всех файлов
    for (int i = 0; i < rf.real_number_files; i++) {
        // Вычисляем хэш каждого маленького зашифрованного файла
        if (i == rf.part_here_file)
        {
            continue;
        }
        std::string file_i_name_str = path_to_temp_dir + std::to_string(i) + ".dat";
        std::string hash_of_file_i = count_hash (file_i_name_str.c_str());
        if (rf.hash_each_file[i] != hash_of_file_i)
        {
            std::cout << "Хэш файла: " << i << ".dat" << " не совпадает с сохраненным в файле-ссылке." << std::endl;
            exit(1);
        }
    }

    FILE* f;

    // Записываем тот файл, который мы оставляли у себя под тем именем, под которым необходимо
    // Открыли файл (с проверкой)
    std::string filename_part_here0 = path_to_temp_dir + std::to_string(rf.part_here_file) + ".dat";
    if ((f = fopen(filename_part_here0.c_str(), "wb")) == NULL) {
        std::cout << "Невозможно открыть файл " << filename_part_here0 << " для записи в него части основного файла!"
                  << std::endl;
        exit(1);
    }

    // Записывае часть общего файла в temp файл
    int real_write_bytes0 = fwrite(rf.part_here_data, 1, rf.size_part_here_file, f);
    if (real_write_bytes0 != rf.size_part_here_file) {
        std::cout << "ERROR = real_write_bytes0 != rf.size_part_here_file" << std::endl;
        exit(1);
    }

    fclose(f);
    //std::cout << "std::to_string(rf.part_here_file) = " << std::to_string(rf.part_here_file) << std::endl;
    //std::fflush(stdout);

    // Пауза для анализа произошедшего
    //user_pause();

    // Переименование принятых файлов в правильном порядке
    for (int i = 0; i < rf.real_number_files; i++)
    {
        // Переименование в нормальный список
        std::string file_name_i_old = path_to_temp_dir + std::to_string(rf.switch_table_second[i]) + ".dat";
        std::string file_name_i_new = path_to_temp_dir + std::to_string(rf.switch_table_first[i]) + "_.dat";
        int res_rename = rename(file_name_i_old.c_str(), file_name_i_new.c_str());
        if (res_rename != 0)
        {
            std::cout << "При переименовании файла " << file_name_i_old << " на файл " << file_name_i_new
                      << " ПРОИЗОШЛА ОШИБКА." << std::endl;
            exit(1);
        }
    }

    // Пауза для анализа произошедшего
    // user_pause();

    // Сборка всех реальных файлов (без фейковых)
    FILE* fo;

    std::string filename_in_1 = rf.filename;
    if ((fo = fopen(filename_in_1.c_str(), "ab")) == NULL) {
        std::cout << "Невозможно открыть файл " << filename_in_1 << " для записи в него части основного файла!"
                  << std::endl;
        exit(1);
    }

    for (int i = 0; i < rf.real_files; i++)
    {
        std::string filename_i = path_to_temp_dir + std::to_string(i) + "_.dat";
        if ((f = fopen(filename_i.c_str(), "rb")) == NULL) {
            std::cout << "Невозможно открыть файл " << filename_i << " для записи в него части основного файла!"
                      << std::endl;
            exit(1);
        }

        int file_size_i = filesize_share (f);
        unsigned char file_i_buf [file_size_i];

        // Считываем маленький файл в буфер
        int real_read_bytes1 = fread(file_i_buf, 1, file_size_i, f);
        if (real_read_bytes1 != file_size_i) {
            std::cout << "ERROR = real_read_bytes1 != file_size_i" << std::endl;
            exit(1);
        }

        fclose(f);

        // Записывае маленький файл в большой
        int real_write_bytes1 = fwrite(file_i_buf, 1, file_size_i, fo);
        if (real_write_bytes1 != file_size_i) {
            std::cout << "ERROR = real_write_bytes1 != file_size_i" << std::endl;
            exit(1);
        }

    }

    fclose(fo);

    for (int i =0; i < rf.real_number_files; i++)
    {
        // Удаляем очередной принятый мини-файл после его записи в большой зашифрованный файл
        std::string filename_i = path_to_temp_dir + std::to_string(i) + "_.dat";
        remove(filename_i.c_str());
    }

    // Проверяем хэш всего зашифрованного файла
    std::string hash_of_all_file_dec = count_hash (const_cast<const char*>(rf.filename));
    if (rf.hash_of_all_file != hash_of_all_file_dec)
    {
        std::cout << "Общий хэш зашифрованного файла не совпал с общим хэшем зашифрованного файла, который указан в файле-ссылке." << std::endl;
        std::cout << "Файл не принят из распределенного хранилища." << std::endl;
        std::cout << "Возможные причины: \n\tПроблемы с сетью - необходимо повторить запрос. \n\tДержателей частей файлов в данный момент нет в сети - попробуйте повторить запрос позже." << std::endl;
        sleep(10);
        return 1;
        //exit(1);
    }


    std::cout << "Name of encrypted_file_before_decrypting: " << filename_in_1.c_str() << std::endl;
    //user_pause();

    // Расшифровываем этот файл
    decrypt (filename_in_1.c_str());

    // Удаляем файлы AES.key и AES.iv
    remove("AES.key");
    remove("AES.iv");
    remove(filename_reference_file_c_str);

    //std::cout << "Сборка файла из распределенного хралища завершена успешно.\n" << std::endl;

    //================================== СОБРАЛИ ФАЙЛ ==== END =========================================================
    //====================================================================================================================

    return 0;
}


void set_color(int color_number)
{
	//write_to_log_file (LOG_FILE_NAME, "C_part [FN: \"set_color\"]\n");
	/*


         foreground background
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47



*/


	std::cout << "\033[" << 1 << ";" << color_number << "m";

}


void set_cursor_position (int x, int y)
{
	//write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"set_cursor_position\"]\n");
	printf("%s%d%s%d%s","\033[",x,";",y,"H");
	return;
}


// Возвращаем key_seek на начало (рекомендовано пользователям иметь несколько ключей с каждым пользователем!)
void key_seek_zero_for_all_users(void)
{
	// Обнуляем текущий ключ для каждого пользователя
    	user_list_and_pointer struct1;
    	int res0 = return_my_user_list (PATH_Date_Base, &struct1);
    	if (res0 == -1)
        	{
            	std::cout << "При запросе списка добавленных пользователей произошла ошибка." << std::endl; 
            	exit(1);       
        	}
    	else if (res0 == 0)
        	{
   			int j = 0;

			if (struct1.pointer > 0)
				{
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(35);
					std::cout << "За время вашего отстутствия происходило обновление глобального блокчейна." << std::endl;
					set_cursor_position (start_line + 1, 4);
					start_line++;
					start_line++;
					set_color(31);
					std::cout << "Настоятельно рекомендуется обновить ключи для пользователей: ";
					set_cursor_position (start_line + 1, 8);
					start_line++;
					set_color(36);				
				}

   			for (j;j<struct1.pointer;j++)
   				{

					int current_seek = get_key_size_for_user(PATH_Date_Base, struct1.user_list[j].c_str());
   					set_key_seek_for_user(PATH_Date_Base, struct1.user_list[j].c_str(), 0);
					
					if (current_seek != 0)
						{
							
							std::cout << struct1.user_list[j] << "; ";
						}

   				}
					// это типо '\n'
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(32); 
					fflush(stdout);
					sleep(5); 
        	}
}




void check_clear_blockchain(void)
{


	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"check_clear_blockchain\"]\n");
	
	// Запрашиваем количество транзакций
	char last_id_c[BUF_SIZE];
	memset(last_id_c, 0, BUF_SIZE);
    	get_how_much_id (PATH_Date_Base, last_id_c);
	
	int last_id = atoi((const char*) last_id_c);	

	if (last_id >= limit_number_of_transaction)
		{


			std::cout << " Запущен процесс обнуление глобального блокчейна, пожалуйста, подождите ..." << std::endl;
			fflush(stdout);
			// Алгоритм обнуления
			// Да, спим целых десять секунд!!!
			// Mutex - нужен для того, чтобы не обновлялась БД, пока идет процесс обнуления глобального блоечейна
			//pthread_mutex_lock(&mutex);
			//sleep(10);
			//pthread_mutex_unlock(&mutex);
			// Очищение старой копии BLOCKCHAIN_TABLE
			Clear_Blockchain_Table(PATH_Date_Base);
			// Создание первой записи в BLOOCKCHAIN_TABLE БД (она будет у всех пользователей всегда одинаковая)

                	int default_id = 1;
                	std::string encrypt_message_default = "1;1;1;0;encrypt_message;";
                	std::string default_hesh_before = "0123456789";
                
                	// Хэш будем считать вот так
                	//std::string default_this_hash = get_hash (encrypt_message_default);
                	// Но пока не можем поэтому пока что напишем вот так
                	std::string default_this_hash = "9876543210";

                	int default_blockchain_add = add_record_to_local_blockchain(PATH_Date_Base, default_id, encrypt_message_default.c_str(), default_hesh_before.c_str(), default_this_hash.c_str());
                
                	if (default_blockchain_add != 0)
                    		{
                        		std::cout << "При добавлении в BLOCKCHAIN_TABLE записи по умолчанию произошла ошибка." << std::endl; 
                        		exit(1);                       
                   	 	}
               	 else
                   	 	{
                        	//std::cout << "При добавлении в BLOCKCHAIN_TABLE записи по умолчанию ошибок не выявлено." << std::endl;                    
                    		}
                    		
			// Да, спим целых десять секунд!!!
			// Mutex - нужен для того, чтобы не обновлялась БД, пока идет процесс обнуления глобального блоечейна
			//pthread_mutex_lock(&mutex);
			//sleep(10);
			//pthread_mutex_unlock(&mutex);
			int new_last_analysed_id = 1;
			set_last_analysed_id_in_blockchain(PATH_Date_Base, new_last_analysed_id);
			set_last_available_id_in_blockchain(PATH_Date_Base, new_last_analysed_id);
			
			
			
			// Тут функция обновления ключевой информации для всех моих клиентов
			key_seek_zero_for_all_users();
			
			set_cursor_position (start_line + 1, 4);
			start_line++;
			std::cout << "Процесс обнуление глобального блокчейна звершен удачно, благодарим за ожидание." << std::endl;
			fflush(stdout);
			//get_actual_blockchain_copy(port2);
			// Заполнение БД из файла
			//const char* path_to_buffer_file = "actual_blockchain.txt";
			//blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
			return;
		
		}
	
	else
		{
			// Ничего не делаем
			return;
		}	
	
}









void square(int heigth, int width)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"square\"]\n");
	// Выведем квадрат и вернемся в начало строки 

	int i = 0;
	printf("%s","\n");
	
	// Рисуем левую вертикальную линию
	for (i;i<heigth-4;i++)
		{
			printf("%s\n","||");
		}

	i = 0;
	// Рисуем правую вертикальную линию
	for (i;i<heigth-2;i++)
		{
			printf("%s\n","||");
			printf("%s%d%s%d%s","\033[",i+3,";",width,"H");
		}
		
	printf("%s","\033[2;2H");	
	i = 0;
	// Рисуем верхнюю горизонтальную линию
	for (i;i<width-1;i++)
		{
			printf("%s","=");
		}	

	printf("%s%d%s","\033[",heigth,";2H");
	i = 0;
	// Рисуем нижнюю горизонтальную линию
	for (i;i<width-1;i++)
		{
			printf("%s","=");
		}


	printf("%s","\033[4;4H");
	fflush (stdout);
	return;
}


void loading_animation (int start_or_stop, int start_x, int start_y, int height, int width, int delay_us)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"loading_animation\"]\n");
	if (start_or_stop == 0) // start
		{
		
			// Процесс загружки отображается
	
			set_cursor_position (start_x + height/2, start_y + width/3);
			set_color(35);
			printf("%s","Загрузка ...");
			fflush (stdout);
			set_color(34);
		
			int i = 0;		
			for (i; i< width; i++)
				{
					set_cursor_position (start_x, start_y + i);
					printf("%s",">");
					set_cursor_position (start_x, start_y + i + 1);
					printf("%s"," ");
					fflush (stdout);
					usleep(delay_us);
				}
			
			i = 0;	
			for (i; i< height; i++)
				{
					set_cursor_position (start_x + i, start_y + width);
					printf("%s","v");
					set_cursor_position (start_x + i + 1, start_y + width);
					printf("%s"," ");
					fflush (stdout);
					usleep(delay_us);
				}	



			i = width;	
			for (i; i > 1; i--)
				{
					set_cursor_position (start_x + height, start_y + i);
					printf("%s","<");
					set_cursor_position (start_x + height, start_y + i - 1);
					printf("%s"," ");
					fflush (stdout);
					usleep(delay_us);
				}				


			i = height;	
			for (i; i > 0; i--)
				{
					set_cursor_position (start_x + i, start_y);
					printf("%s","^");
					set_cursor_position (start_x + i - 1, start_y);
					printf("%s"," ");
					fflush (stdout);
					usleep(delay_us);
				}
				
		}
								
	else if (start_or_stop == 1)
		{
		
			set_color(32);
			int i = 0;
			int j = 0;
			for (i;i<height+1;i++)
				{	
					for (j;j<width+1;j++)
						{
							set_cursor_position (start_x + i, start_y + j);
							printf("%s"," ");
						}
					j = 0;
				}
			fflush (stdout);	
				
			int new_height = height/3;
			int new_height_1 = height - new_height;
			int new_width = width/3;
			int ii = 0;
			
			int count_width = 0;
			for (ii;ii<new_height;ii++)
				{
					set_cursor_position (start_x + height/2 + ii, start_y + width/5 + count_width);
					count_width++;
					printf("%s","\\");
					printf("%s","\\");
					fflush (stdout);
					usleep(delay_us/2);
				}
				

			int x = start_x + height/2 + ii + 1;
			int y =  start_y + width/5 + count_width + 4;
			
			int jj = 0;
			for (jj;jj < new_height_1;jj++)
				{
					set_cursor_position (start_x + height/2 + ii - jj -1, start_y + width/5 + count_width + 1);
					count_width++;
					printf("%s","//");
					fflush (stdout);
					usleep(delay_us/2);
				}
		
		set_cursor_position (x + 1, y - 20 );
		set_color(33);
		printf("%s","Инициализация завершена успешно!");
		fflush (stdout);
		

			
				
		}
	else
		{
			set_cursor_position (start_x , start_y);
			set_color(31);
			std::cout << "Введены неверные параметры для функции анимации!";
			fflush (stdout);
			exit(1);
		}	
	

	return;
}


int filesize(const char* filename)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"filesize\"]\n");
    std::fstream file(filename);
    int size = 0;
    file.seekg (0, std::ios::end);
    size = file.tellg();
    file.close();
    return size;
}

std::string getString(char x) 
{ 
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"getString\"]\n");
    // string class has a constructor 
    // that allows us to specify size of 
    // string as first parameter and character 
    // to be filled in given size as second 
    // parameter. 
    std::string s(1, x); 
    return s;    
} 

std::string input_password(void)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"input_password\"]\n");
	srand(time(0)); // инициализация генератора случайных чисел

	char key_buf[KEY_SIZE];
	memset(key_buf,0,KEY_SIZE);
	
	// Объявляем множество элементов
	std::set<char> Char_Set;
	
	// Заполняем его элементами от 32 до 122
	int i = 32;
	for (i;i<=122;i++)
		Char_Set.insert(i);

	// Сформируем массив (двумерный). 122 - 32 = 90 => необходим массив 10x9
	int size_1 = 10;
	int size_2 = 9;
 
	char random_arr_2 [size_1][size_2];
	int j = 0;
	for(j;j<size_1;j++)
		memset(random_arr_2[j],0,size_2);
	
	
	// Формируем псевдослучайным образом двумерный массив символов
	int y = 0;
	int z = 0;
	for(y;y<size_1;y++)
		{
			for(z;z<size_2;z++)
				{
					// Узнаем размер множества
					int current_size_of_set = Char_Set.size();
					
					// Выбираем случайное число от 0 до current_size_of_set включительно
					int random_elemet_of_set = 0 + rand() % current_size_of_set; // получаем целое случайное число от 1 до current_size_of_set
										
					// Объявляем итератор
					std::set<char>::iterator it = Char_Set.begin();
					
    					for (int ii = 0; ii < random_elemet_of_set ; ii++, it++) 
    						{
        						// Просто заглушка
    						}
					
					// Получаем значение нужного элемента
					random_arr_2[y][z] = *it;
					
					// Удаляем добавленный элемент из множества
					Char_Set.erase(random_arr_2[y][z]);
				}
			z = 0;
		
		}
	
	// Выведем сформированный массив в красивом виде на экран
	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(32);
	std::cout << "    0   1   2   3   4   5   6   7   8";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(34);
	std::cout << "    _________________________________";
	
	y = 0;
	z = 0;
	for(y;y<size_1;y++)
		{
			set_cursor_position (start_line + 1, 4);
			start_line++;
			set_color(32);
			std::cout << y ;
			set_color(34);
			std::cout << "|  ";
			for(z;z<size_2;z++)
				{	
					set_color(37);
					std::cout << random_arr_2[y][z] << "   ";
				}
			set_cursor_position (start_line + 1, 4);
			start_line++;	
			std::cout << "" << std::endl;	
			z = 0;
		
		}
	
	fflush (stdout);
	// Вводим строку символов
	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(36);
	std::cout << "Введит пароль в формате <";
	set_color(37);
	std::cout << "Строка";
	set_color(35);
	std::cout << "Столбец";
	set_color(37);
	std::cout << "Строка";
	set_color(35);
	std::cout << "Столбец";
	set_color(36);
	std::cout << "...>";
	fflush (stdout);
	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(36);
	std::cout << "Виртуальный пароль: ";
	fflush (stdout);
	std::string password_virt;
	set_color(34);
	std::getline(std::cin,password_virt);
	//fflush (stdout);
	fflush (stdin);
	
	// Проверяем подходит ли пароль по формату (должно быть четное количество элементов)
	int value = password_virt.length();
    	if (value % 2 != 0)
    		{
    			set_cursor_position (start_line + 1, 4);
			start_line++;
			set_color(31);
        		std::cout << "Пароль не подходит по формату." << std::endl;
			set_cursor_position (start_line + 1, 4);
			start_line++;
			set_color(32);
			std::cout << "Проверьте правильность введенных данных. ";
			//set_cursor_position (start_line + 1, 4);
			//start_line++;
			set_color(31);
			std::cout << "Работа программы будет завершена с ошибкой." << std::endl;
			set_color(37);
        		fflush (stdout);
			sleep(1);
        		exit(1);
		}
	
	// Парсим введенный виртуальный пароль в настоящий
	const char* password_virt_c = password_virt.c_str();
	
	char buf [value+1];
	memset(buf,0,value+1);
	
	int jj = 0;
	for (jj;jj<value;jj++)
		{
			if (password_virt_c[jj] != '\0')
				buf[jj] = password_virt_c[jj];
		}
	
	buf[jj] = '\0';	
	
	int p = 0;
	int count = 0;		
	for(p;p<value;p++)
		{
			std::string s1 = getString(buf[p]);
			std::string s2 = getString(buf[p+1]);
			int line = atoi(s1.c_str());
			int column = atoi(s2.c_str());
			p++;
			key_buf[count] = random_arr_2[line][column];
			count++;
			//std::cout << "p = " << p << std::endl;
		}
	
	key_buf[count] = '\0';	
	const char* password_c = const_cast<const char*>(key_buf);
	std::string password (password_c);
	//std::cout << "Вы ввели пароль (отображение из функции): |" << password << "|" << std::endl;
	return password;	
}




int read_file_into_memory(const char* filepath, unsigned char* buffer, int length)
{

  write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"read_file_into_memory\"]\n");
  std::ifstream is;
  is.open (filepath, std::ios::binary );
 
  // get length of file:
  is.seekg (0, std::ios::end);
  length = is.tellg();
  is.seekg (0, std::ios::beg);
 
  // read data as a block:
  is.read (reinterpret_cast<char*>(buffer),length);
  is.close();
  //std::cout.write (buffer,length);
  return 0;
}


int read_file_into_memory(const char* filepath, char* buffer, int length)
{

  write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"read_file_into_memory\"]\n");
  std::ifstream is;
  is.open (filepath, std::ios::binary );
 
  // get length of file:
  is.seekg (0, std::ios::end);
  length = is.tellg();
  is.seekg (0, std::ios::beg);
 
  // read data as a block:
  is.read (buffer,length);
  is.close();
  //std::cout.write (buffer,length);
  return 0;
}




int write_to_file (char* buf, int size, const char* filename)
{

write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"write_to_file\"]\n");
//pthread_mutex_lock(&mutex);
   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
   //printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"a");

   // Проверка открытия файла
   if (mf == NULL) 
   
   {
   	printf ("ошибка открытия файла: %s", filename);
   	exit(1);
   }
   else 
	{
		//printf ("выполнено\n");
 	}
   //Запись данных в файл
   fwrite(buf, 1, size, mf);
   //printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
//pthread_mutex_unlock(&mutex);
   //printf ("Файл закрыт\n");
   return 0;
}




/*
int write_to_file_bin (char* buf, int size, const char* filename)
{

std::string buf_str(buf);



unsigned char buf_0 [size];
memset(buf_0,0,size);

int res = const_char_string_to_bin_arr(buf_0,size,(const char*) buf);


write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"write_to_file_bin\"]\n");
//pthread_mutex_lock(&mutex);
   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
   //printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"a");

   // Проверка открытия файла
   if (mf == NULL) 
   
   {
    printf ("ошибка открытия файла: %s", filename);
    exit(1);
   }
   else 
    {
        //printf ("выполнено\n");
    }
   //Запись данных в файл
   fwrite(buf, 1, size, mf);
   //printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
//pthread_mutex_unlock(&mutex);
   //printf ("Файл закрыт\n");
   return 0;
}
*/






int checkFile(const char* file_name)
{

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"checkFile\"]\n");
   std::ifstream file;
   file.open(file_name);
   if(!file)
    {
        file.close();
        //std::cout << "\nCant find this file : " << file_name;      
        return -1;
    }
   file.close();
   //std::cout << "\nThis file is found: " << file_name; 
   return 1;   
}

void encrypt_file (const char* filename_in, const char* filename_out, const char* password)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"encrypt_file\"]\n");
	std::string password_str (password);
	std::string filename_in_str (filename_in);
	std::string filename_out_str (filename_out);
	std::string programm_name_str = "openssl aes-256-cbc -a -salt -pass pass:" + password_str + " -in " +  filename_in_str +  " -out " + filename_out_str + " > /dev/null";
	char* ptr; // для совместимости
	call_Linux_programm(programm_name_str.c_str(),ptr);	
	//std::cout << "Файл базы данных зашифрован успешно." << std::endl;
	return;
}

void decrypt_file (const char* filename_in, const char* filename_out, const char* password)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"decrypt_file\"]\n");
	std::string password_str (password);
	std::string filename_in_str (filename_in);
	std::string filename_out_str (filename_out);	
	std::string programm_name_str = "openssl aes-256-cbc -d -a -pass pass:" + password_str + " -in " +  filename_in_str +  " -out " + filename_out_str + " > /dev/null";
	char* ptr; // для совместимости
	call_Linux_programm(programm_name_str.c_str(),ptr);	
	//std::cout << "Файл базы данных расшифрован успешно." << std::endl;
	return;
}

void get_hash_from_string (const char* string, char* result_string)
{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_hash_from_string\"]\n");
	std::string str(string);
	unsigned char result[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)str.c_str(), str.size(), result);
	std::ostringstream sout;
	sout<<std::hex<<std::setfill('0');
	for(long long c: result)
		{
    			sout<<std::setw(2)<<(long long)c;
		}	
		
	//std::cout << "hash_from_string = " << sout.str() << "|" << std::endl;
	std::string hash_from_string_str = sout.str();
	const char* hash_from_string = hash_from_string_str.c_str();
	//std::cout << "hash_from_string = " << hash_from_string << "|" << std::endl;
	int len = strlen(hash_from_string);
	int i = 0;
	for(i;i<len;i++)
		{
			if(hash_from_string[i] != '\0')
				result_string[i] = hash_from_string[i];
			else
				break;	
		}
	result_string[i] = '\0';	
		
	return;
}


static int callback_update_key(void *data, int argc, char **argv, char **azColName){
   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_update_key\"]\n");
   int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      //printf("%s\n", "callback_update_key-выполнено");
   }
   
   //printf("\n");
   return 0;
}

// Принимает на вход - расшифрованное сообщение
// Возвращает либо переделанное сообщение (и записывает файл), либо само сообщение, если это не файл
std::string analyse_input_message_for_file_content (const char* input_message)
{

	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"analyse_input_message_for_file_content\"]\n");
	if (input_message[0]=='$')
		{
		
			int len = strlen(input_message) - 6; // 6 - "$file$"
			int pointer = 6;
			
			// Сформировать имя файла из даты и времени
			// Узнаем текущую дату и время
			  
			// current date/time based on current system
   			time_t now = time(0);
   			tm *ltm = localtime(&now);
			std::string filename_date_time = "Date__" + std::to_string(ltm->tm_mday) +  "_" + std::to_string(1 + ltm->tm_mon) + "_" + std::to_string(1900 + ltm->tm_year) + "_Time__" +
										 std::to_string(ltm->tm_hour) + "_" + std::to_string(ltm->tm_min) + "_" + std::to_string( ltm->tm_sec) + "_FILE_INPUT.txt";
										 
			// Проверка на существование директории и ее создание если ее нет
			std::string dir_name = "INPUT_FILES";
			DIR* dir = opendir(dir_name.c_str());
			if (dir)
				{
    					/* Directory exists. */
    					//std::cout << "Директория для файлов существует." << std::endl;
    					closedir(dir);
				}
			else if (ENOENT == errno)
				{
					//std::cout << "Директория для файлов не существует." << std::endl;
    					/* Directory does not exist. */
    					std::string linux_command = "mkdir " + dir_name;
    					char res_buf[100]; // Просто для совместимости 
    					call_Linux_programm(linux_command.c_str(), res_buf);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					printf("%s\n","Ошибка при проверке существования директории");
    					exit(1);
				}
			
			
			std::string where_write = dir_name + "/" + filename_date_time;							 							 
			// Записываем необходимую часть в файл

            // 05_10_21
			//write_to_file_bin (const_cast<char*>(&input_message[pointer]), len, where_write.c_str());
			
            std::string input_message_string(const_cast<const char*>(&input_message[pointer]));
            int res_str_to_bin_file = string_to_bin_file(&input_message_string,where_write.c_str());
            if (res_str_to_bin_file != 1)
            {
                printf("При выполнении функции 'string_to_bin_file' произошла ошибка.\n");
                fflush(stdout);
                sleep(5);
                exit(1);
            } 
            // Формируем форму сообщения
			std::string new_input_message_str = "FILE. Path_to_file: " + where_write;		
			//std::cout << "В данном сообщение был передан файл. Файл записан под названием: " << where_write << std::endl;
			return new_input_message_str;
		}
		
	else
		{
			std::string ret_same_message(input_message);
			return ret_same_message;
		}	
	
}


int update_key_file_for_user (const char* path_to_DB, const char* user_name, const char* path_to_key_file)

{
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"update_key_file_for_user\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    // Для чтения значения файла-ключа
    int fsize;
    char* buffer_key;

	//Проверка существования файла БД
    	//std::cout << "Проверка существования файла БД ..." <<std::endl;    
    	int file_exist = checkFile(path_to_DB);
    	
    	    if (file_exist == 1)
        {
            //std::cout << "\nФайла базы данных существует." << std::endl;

            /* Open database */
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
                //fprintf(stdout, "Opened database successfully\n");
            }
            //std::cout << "Файл БД открыт." << std::endl;


            // Проверяем существование файла ключа для этого нового пользователья
            int check_key_file_result = checkFile(path_to_key_file);

            if (check_key_file_result == -1)
                {
                    set_color(32);
                    std::cout << "Файла ключа для данного пользователя не найдено.";
                    fflush (stdout);
                    //set_cursor_position (start_line + 1, 4);
                    //start_line++;
                   
                    return -1;                
                }
            else if (check_key_file_result == 1)
                {

                    //std::cout << "Файл ключа для данного пользователя найден." << std::endl; 

                     // Узнаем размер файла
                    fsize = filesize(path_to_key_file);
                    //std::cout << "Размер файла-ключа: " << fsize << " байт." << std::endl;

                     // Считываем содержимое файла в переменную "key"
                                  

                      // allocate memory:
                    
                    buffer_key = new char [fsize];
                    int res = read_file_into_memory(path_to_key_file,buffer_key,fsize);
                    
                    if (res != 0)
                        {
                            std::cout << "При чтении файла-ключа произошла ошибка." << std::endl;
                            delete[] buffer_key;
                            
                            return -1;                        
                        }
                    else if (res == 0)
                        {
                            //std::cout << "Файл-ключ успешно считан в память." << std::endl;

                        }
 
                }



              // Добавление записи в ГЛАВНУЮ ТАБЛИЦУ БД о новом пользователе

              /* Create SQL statement */


		std::string user_name_str (user_name);
		std::string buffer_key_string (buffer_key, fsize);
                std::string string_sql_0 = "UPDATE GENERAL_TABLE SET KEY = '" + buffer_key_string;
                std::string string_sql_1 = "', KEY_SIZE = " + std::to_string(fsize) + ", ";
                std::string string_sql_2 = " KEY_SEEK = " + std::to_string(0) + " ";
                std::string string_sql_3 = "WHERE USER_NAME = '" + user_name_str + "';";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1 + \
                                                string_sql_2 + \
                                                string_sql_3;

                sql = const_cast<char*>(string_sql_full.c_str());

   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_update_key, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);

	// (30.07.21)
      delete[] buffer_key;
      sqlite3_close(db);
      return -1;   
	//exit(1); 
	// (30.07.21)
  
	

   } else {
      //fprintf(stdout, "Records created successfully\n");
   }

              delete[] buffer_key;
              sqlite3_close(db);
              //std::cout << "Файл БД закрыт." << std::endl;
              
              // Удаление оригинала файла-ключа

              if(remove(path_to_key_file)) {
               // printf("Error removing file-key: %s", path_to_key_file);
               
                return -1;
                }

              //std::cout << "Файл-ключ скопирован в БД. Оригинал файла-ключа успешно удален." << std::endl;       
                  
              analyse_blockchein_for_user_name(path_to_DB, user_name);
              

              //std::cout << "Новый пользователь успешно добавлен." << std::endl;
              return 0;

        }    
    else if (file_exist == -1)
        {

           std::cout << "Ошибка при попытке открытия файла БД. Файла БД не существует." << std::endl;
           std::cout << "Новый пользователь не добавлен." << std::endl;
           
           return -1;
        }
    	
}


int blockchain_from_file_to_DB (const char* path_to_DB, const char* path_to_buffer_file)
{
	
	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"blockchain_from_file_to_DB\"]\n");
	int buffer_filesize = filesize(path_to_buffer_file);
	//std::cout << "Размер path_to_buffer_file = " << buffer_filesize << std::endl;
	
	char buf [buffer_filesize];
	memset(buf,0,buffer_filesize);
	int res_read =  read_file_into_memory(path_to_buffer_file, buf, buffer_filesize);
	if (res_read != 0)
		{
			std::cout << "При чтении path_to_buffer_file произошла ошибка." << std::endl;
			exit(1);
		}



	//std::cout << "buf = |" << buf << "| buffer_filesize = |" << buffer_filesize << "|." << std::endl;
	fflush(stdout);







	// Запрашиваем последний id
	int last_id;
	std::string last_id_string;
    	int res2 = how_many_transaction_id(path_to_DB, &last_id_string);
    	if (res2 != 0)
    		{
    			std::cout << "При получении last_id произошла ошибка." << std::endl;
    			exit(1);
    		}
    	else
    		{
    			//std::cout << "LAST_ID = " << last_id_string << std::endl;
    			//std::cout << "При получении last_id произошла ошибки не произошло." << std::endl;
    		}	
	last_id = atoi(last_id_string.c_str());
	
	
	int i = 0;
	int count = 0;
	int general_length = 0;
	
	for (i;i<buffer_filesize;i++)
		{
			if (buf[i] == '~') count++; // символ '~' - конец транзакции
			if (count == 1)
				{	
					int this_transaction_size = i -1 - general_length + 1; 
					char buf_one_transaction[this_transaction_size + 1];
					strncpy(buf_one_transaction, &buf[i - 1 -this_transaction_size + 1 ],this_transaction_size);
					buf_one_transaction[this_transaction_size] = '\0';
					general_length = general_length + this_transaction_size + 1;
					count = 0;
					std::string transaction_body = const_cast<const char*>(buf_one_transaction);
					

                    //std::cout << "ПРОВЕРКА: ";
                    //std::cout << "buffer_filesize = |" << buffer_filesize  << "|." << std::endl;
                    //std::cout << "this_transaction_size = |" << this_transaction_size  << "|." << std::endl;
                    //std::cout << "buf_one_transaction = |" << buf_one_transaction  << "|." << std::endl;
                    //fflush(stdout);
                    //sleep(2);


					// Запросить hash предыдущего
					char last_id_container_hash_c[HEX_LENGTH];
					memset(last_id_container_hash_c, 0, HEX_LENGTH);
					std::string last_id_std_str = std::to_string(last_id);
					const char* last_id_str = last_id_std_str.c_str();
					get_hash_id_container (path_to_DB, last_id_str, last_id_container_hash_c);

					//std::cout << "last_id_container_hash_c = " << last_id_container_hash_c << "|"<< std::endl;   
					
					
										
					std::string hesh_before = last_id_container_hash_c;
					std::string last_id_std_str_new = std::to_string(last_id + 1);
					
					// НОРМАЛЬНОЕ получение HASH_THIS ----
					
					// ------ Вызов функции взятия ХЭШа от const char* ------ BEGIN -----------------
					std::string string_for_compute_hash = transaction_body + hesh_before;
					const char* string_for_hashing =  string_for_compute_hash.c_str(); //argv[1];
	
					char hash_from_string[HASH_LENGTH];
					memset(hash_from_string,0,HASH_LENGTH);
	
					get_hash_from_string (string_for_hashing, hash_from_string);
	
					const char* hash_from_string_result = const_cast<const char*>(hash_from_string);
	
					//std::cout << "hash_from_string_result = " << hash_from_string_result << "|" << std::endl;
	
					// ------ Вызов функции взятия ХЭШа от const char* ------ END -----------------
					
					std::string hesh_this (hash_from_string_result);
					
					last_id++;


					if (last_id == 1)
					{
						hesh_before = "0123456789";
						hesh_this = "9876543210";
					}


					//std::cout << "Перед непосредственной записью в блокчейн: ";
					//std::cout << "transaction_body.c_str() = |" << transaction_body.c_str()  << "|." << std::endl;
					//fflush(stdout);
					//sleep(2);

					// Добавляем полученную транзакцию в БД в BLOCKCHAIN_TABLE
					int res_add = add_record_to_local_blockchain(path_to_DB, last_id, transaction_body.c_str(), hesh_before.c_str(), hesh_this.c_str());
					if (res_add != 0)
						{
							std::cout << "При выполнении add_record_to_local_blockchain-функции произошла ошибка." << std::endl;
    							exit(1);
						}
					else
						{
							// Проверяем на заполнение глобальный блокчейн (свою локальную копию глобального блокчейна)
							check_clear_blockchain();
							//std::cout << "При выполнении add_record_to_local_blockchain-функции ошибки не произошло." << std::endl;
						}		
					
					count = 0;
				}
		}
	
	
	if(remove(path_to_buffer_file)) 
		{
                	//printf("Error removing %s\n", path_to_buffer_file);
                	return -1;
                }
	
	//std::cout << "Запись блокчейна в БД из файла успешно завершена." << std::endl;
	
	return 0;
}

static int callback_update_key_seek(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_update_key_seek\"]\n");
   int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      //printf("%s\n", "callback_update_key_seek-выполнено");
   }
   
   // printf("\n");
   return 0;
}

static int callback_select_key_for_user(void *data, int argc, char **argv, char **azColName){
 
   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_key_for_user\"]\n");
   char* key_buf = (char*) data;
   
   int i = 0;
   while (argv[0][i] != '\0')
   	{
   		key_buf[i] = argv[0][i];
   		i++;
   	}
   
   key_buf[i] = '\0';
	
  // printf("CALL_BACK_key_buf = %s|\n", key_buf);	   

 //  printf("\n");
   return 0;
 
}


static int callback_select_key_seek_for_user(void *data, int argc, char **argv, char **azColName){
   
   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_key_seek_for_user\"]\n");
   int* seek = (int*)data;
   
   *seek = atoi(argv[0]);
   
  // printf("\n");
   return 0;
}

static int callback_select_key_size_for_user(void *data, int argc, char **argv, char **azColName){
   
   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_key_size_for_user\"]\n");
   int* key_size = (int*)data;
   
   *key_size = atoi(argv[0]);
   
  // printf("\n");
   return 0;
}


const char* encrypt_text_message (const char* user_name, const char* text_message, int* res_enc)
{
			write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"encrypt_text_message\"]\n");
			// Считываем текущее смещение ключа для user_name
			int current_seek = get_key_seek_for_user(PATH_Date_Base, user_name);
	

			//user_pause();
			// Узнаем размер текстового сообщения для шифрования
			int size_of_text_message = strlen(text_message);
			//printf("size_of_text_message = %d\n",size_of_text_message);
            //printf("text_message = %|%s|\n",text_message);
			
				// Проверяем на то, не больше ли зашифровываемое сообщение чем остаток от ключа
				// Для этого запрашиваем key_size
				int key_size = get_key_size_for_user(PATH_Date_Base, user_name);
				// Находим разницу между key_size и current_seek.
				int diff = key_size - current_seek;
				
				// Если она меньше, чем  size_of_encrypt_message, то тогда "continue;"
				if (diff < size_of_text_message -1)
					{
						printf("%s\n","ОШИБКА !!! diff < size_of_text_message -1 !!!");
						printf("%s%s\n","Не хватает оставшейся длины ключа для пользователя: ", user_name);
                        fflush(stdout);
                        sleep(5);
                        return "error";
                        //res_enc = 0;
						//exit(1);
					}

		 	char real_key[size_of_text_message];
			memset(real_key,0,size_of_text_message);
			
			char key_buf[BUF_SIZE];
			memset(key_buf,0,BUF_SIZE);
			
			get_key_for_user(PATH_Date_Base, user_name, key_buf);
			

            //user_pause();

			//printf("key_sent = %s| for user = %s|\n",key_buf, user_name);
			
			// Формируем ключ на котором будем зашифровывать сообщение
			
			strncpy(real_key, &key_buf[current_seek], size_of_text_message);
			
			// Процесс зашифрования сообщения
			
			//Сюда будем записывать результат наложения ключа на зашифрованное сообщение
			 char encrypt_message[size_of_text_message+1];
			memset(encrypt_message,0,size_of_text_message+1);
			
			// Непосредственно сам процесс наложения ключа на зашифрованное сообщение
			
			 const char* text_message_uc = reinterpret_cast< const char*>(text_message);
			 const char* real_key_uc = reinterpret_cast< const char*>(real_key);
			
			int j = 0;
			for(j;j<size_of_text_message;j++)
				{
					// Величина одного символа ключа [1;30]
							
					encrypt_message[j] = text_message[j] ^ real_key_uc[j];
					
					
				}
				
				
			// Устанавливаем новое смещение
			int new_current_seek = current_seek + size_of_text_message;
					
			set_key_seek_for_user(PATH_Date_Base, user_name, new_current_seek);	
					
			const char* encrypted_message = reinterpret_cast<const char*>(encrypt_message);
			*res_enc = 1;
			return encrypted_message;
}



int get_key_for_user(const char* path_to_DB, const char* user_name, char* key_buf)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_key_for_user\"]\n");
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
                //fprintf(stdout, "\nOpened database successfully\n");
            }

            //std::cout << "\nФайл базы данных существует." << std::endl;
            //std::cout << "Файл БД открыт." << std::endl;
    




          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string user_name_str (user_name);
    std::string string_sql1 = "SELECT KEY from GENERAL_TABLE WHERE USER_NAME = '";
    std::string string_sql2 = user_name_str + "';";

    std::string string_sql = string_sql1 + string_sql2;

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());

            //std::cout << "Вывод KEY из GENERAL_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_key_for_user, (void*)key_buf, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при запросе KEY из GENERAL_TABLE" << std::endl;
                exit(1);
            } else {
                //fprintf(stdout, "Operation done successfully\n");
                //std::cout << "Вывод KEY из GENERAL_TABLE прошел успешно." << std::endl;
            }

          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
            //std::cout << "Файл БД закрыт." << std::endl;


	//std::cout << "For user: " << user_name << "| KEY = " << key_buf << "|" << std::endl;


    return 0;
}


int get_key_seek_for_user(const char* path_to_DB, const char* user_name)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_key_seek_for_user\"]\n");

    int seek = -1;


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
                //fprintf(stdout, "\nOpened database successfully\n");
            }

            //std::cout << "\nФайл базы данных существует." << std::endl;
            //std::cout << "Файл БД открыт." << std::endl;
    
           // std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string user_name_str (user_name);
    std::string string_sql1 = "SELECT KEY_SEEK from GENERAL_TABLE WHERE USER_NAME = '";
    std::string string_sql2 = user_name_str + "';";

    std::string string_sql = string_sql1 + string_sql2;

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());

            //std::cout << "Вывод KEY_SEEK из GENERAL_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_key_seek_for_user, (void*)&seek, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при запросе KEY_SEEK из GENERAL_TABLE" << std::endl;
                exit(1);
            } else {
                //fprintf(stdout, "Operation done successfully\n");
                //std::cout << "Вывод KEY_SEEK из GENERAL_TABLE прошел успешно." << std::endl;
            }

           // std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
           // std::cout << "Файл БД закрыт." << std::endl;


	//std::cout << "For user: " << user_name << "| key_seek = " << seek << std::endl;


    return seek;
}






int set_key_seek_for_user(const char* path_to_DB, const char* user_name, int current_seek)
{


    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"set_key_seek_for_user\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function SET KEY_SEEK for user called";


    
            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
                //fprintf(stdout, "\nOpened database successfully\n");
            }

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;

       //     std::cout << "\n ----------------------------------------------------- \n" << std::endl;

              /* Create SQL statement */

		std::string user_name_str (user_name);
                std::string string_sql_0 = "UPDATE GENERAL_TABLE SET KEY_SEEK = " + std::to_string(current_seek);
                std::string string_sql_1 = " WHERE USER_NAME = '" + user_name_str + "';";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1;

                sql = const_cast<char*>(string_sql_full.c_str());

           // std::cout << "Вывод результатов UPDATE GENERAL_TABLE SET KET_SEEK..." << std::endl;           

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_update_key_seek, (void*)data, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе результатов UPDATE GENERAL_TABLE SET KEY_SEEK из GENERAL_TABLE" << std::endl;
                exit(1);
            } else {
                //fprintf(stdout, "Operation done successfully\n");
                //std::cout << "Вывод результатов UPDATE GENERAL_TABLE SET KEY_SEEK из GENERAL_TABLE прошел успешно." << std::endl;
            }

            //std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
           // std::cout << "Файл БД закрыт." << std::endl;



    return 0;

}


int get_key_size_for_user(const char* path_to_DB, const char* user_name)
{


    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_key_size_for_user\"]\n");
    int key_size = -1;
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
                //fprintf(stdout, "\nOpened database successfully\n");
            }

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;
   
          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string user_name_str (user_name);
    std::string string_sql1 = "SELECT KEY_SIZE from GENERAL_TABLE WHERE USER_NAME = '";
    std::string string_sql2 = user_name_str + "';";

    std::string string_sql = string_sql1 + string_sql2;

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());

           // std::cout << "Вывод KEY_SIZE из GENERAL_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_key_size_for_user, (void*)&key_size, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при запросе KEY_SIZE из GENERAL_TABLE" << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
              //  std::cout << "Вывод KEY_SIZE из GENERAL_TABLE прошел успешно." << std::endl;
            }

         //   std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
         //   std::cout << "Файл БД закрыт." << std::endl;


	// std::cout << "For user: " << user_name << "| key_size = " << key_size << std::endl;
    return key_size;
}


int blockchain_from_DB_to_file (const char* path_to_DB, int from_id, const char* path_to_buffer_file)
{

	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"blockchain_from_DB_to_file\"]\n");
	// Запрашиваем последний id
	int last_id;
	std::string last_id_string;
    	int res2 = how_many_transaction_id(path_to_DB, &last_id_string);
    	if (res2 != 0)
    		{
    			std::cout << "При получении last_id произошла ошибка." << std::endl;
    			exit(1);
    		}
    	else
    		{
    		//	std::cout << "LAST_ID = " << last_id_string << std::endl;
    		//	std::cout << "При получении last_id произошла ошибки не произошло." << std::endl;
    		}	
	last_id = atoi(last_id_string.c_str());
		
	int i = from_id;
	for (i; i <= last_id; i++)
		{
		
			// Запрашиваем TRANSACTION_BODY с ID == i
			
			std::string transaction_body;
			
			int res3 =  get_transaction_body_where_id(path_to_DB, i, &transaction_body);
			if (res3 != 0)
				{
					std::cout << "При получении get_transaction_body_where_id произошла ошибка.(" << i << ")" << std::endl;
    					exit(1);
				}
			else
				{
				
					int size_of_transaction = transaction_body.length();
					
					const char* buf1 = transaction_body.c_str();
					
					
					char buf_buf [size_of_transaction+1];
					memset(buf_buf,0,size_of_transaction+1);
					
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
					
										
					int res_write = write_to_file (const_cast<char*>(buf), new_size_of_transaction, path_to_buffer_file);
						if (res_write != 0)
							{
								std::cout << "При выполнении write_to_file-функции произошла ошибка.(" << i << ")" << std::endl;
    								exit(1);
							}
						else
							{
							//	std::cout << "При выполнении write_to_file-функции ошибки не произошло.(" << i << ")" << std::endl;
							}	
					// std::cout << "transaction_body(" << i << ") = " << buf << "|" << std::endl;
    					// std::cout << "При получении last_id произошла ошибки не произошло." << std::endl;
				}		
			
		}

	// std::cout << "Запись блокчейна в файл из БД успешно завершена." << std::endl;
	return 0;
}


int try_decrypt(std::string encrypt_mess, std::string* encrypt_mess_DEC)
{

	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"try_decrypt\"]\n");
	char* encrypted_message = const_cast< char*>(encrypt_mess.c_str());
	
	// Считать пользователей в массив

    	user_list_and_pointer struct1;

    	int res0 = return_my_user_list (PATH_Date_Base, &struct1);

	//printf("%s\n","TEST 1");

	int count_of_users = struct1.pointer;;
        const char* users_list[count_of_users];
        memset(users_list,0,count_of_users); //MAX_LEN_USER_NAME
        int seeks_keys[count_of_users];

	//printf("%s\n","TEST 2");

    	if (res0 == -1)
        	{
            	std::cout << "При ЗАПРОСЕ списка добавленных пользователей произошла ошибка." << std::endl; 
            	exit(1);       
        	}
    	else if (res0 == 0)
        	{
        		//printf("%s\n","TEST 3");
			

   			int j = 0;
   			for (j;j<count_of_users;j++)
   				{
   					// Запрашиваем смещение
                    			                //    	printf("%s\n","TEST 4");		
                    			users_list[j] = struct1.user_list[j].c_str();
                    			//printf("%s\n","TEST 5");
                    			int res_seek = get_key_seek_for_user(PATH_Date_Base, users_list[j]);
                    			//printf("%s\n","TEST 6");
                    			if (res_seek == -1)
                    				{
                    				
                    					printf("%s\n", "При запросе key_seek произошла ошибка.");
                    					exit(1);
                    				
                    				}
                    			else 
                    				{
                    					//printf("%s\n","TEST 7");
                    					seeks_keys[j] = res_seek;
                    				//	std::cout << "Для пользователя: " << users_list[j] << " получено смещение ключа = " << seeks_keys[j] << std::endl;
                    				
                    				}	
                    			//printf("%s\n","TEST 8");
                    			
   				}  
        
       // printf("%s\n","TEST 9");
            		//std::cout << "Вывод ЗАПРОШЕННОГО списка добавленных пользователей успешно завершен." << std::endl; 
        	}
	
	//printf("%s\n","TEST 10");
	
	int flag = 1; // 1 - не расшифровано, 0 - расшифровано
	
	int i = 0;
	for (i;i<count_of_users;i++)
		{
		
			// Запросить ключ для пользователя users_list[i];
			
			int size_of_encrypt_message = strlen(reinterpret_cast<const char*>(encrypted_message));
		 	char real_key[size_of_encrypt_message];
			memset(real_key,0,size_of_encrypt_message);
			
			char key_buf[BUF_SIZE];
			memset(key_buf,0,BUF_SIZE);
			get_key_for_user(PATH_Date_Base, users_list[i], key_buf);
			
			//printf("key_sent = %s| for user = %s|\n",key_buf,users_list[i]);
			
			// Формируем ключ на котором будем ПЫТАТЬСЯ расшифровать сообщение
			int current_seek = seeks_keys[i];
			
			
			
			
			// ЭТО ДЛЯ ТЕСТИРОВАНИЯ !!!!!!!!!!!!!!!!!!!!!!!!!
			
			//	current_seek = 0;
			
			// ЭТО ДЛЯ ТЕСТИРОВАНИЯ !!!!!!!!!!!!!!!!!!!!!!!!!
			
			
			// Проверяем на то, не больше ли зашифровываемое сообщение чем остаток от ключа
				// Для этого запрашиваем key_size
				int key_size = get_key_size_for_user(PATH_Date_Base, users_list[i]);
				
				//printf("%s\n", "POP 1");
				
				// Находим разницу между key_size и current_seek.
				int diff = key_size - current_seek;
				
				
				//printf("%s\n", "POP 1");
				
				// Если она меньше, чем  size_of_encrypt_message, то тогда "continue;"
				if (diff < size_of_encrypt_message -1)
					{
						//printf("%s\n","diff < size_of_encrypt_message -1 !!!");
						continue;
					}
				
				
				//printf("%s\n", "POP 3");
			// Но это оставим на потом ...		
			
			strncpy(real_key, &key_buf[current_seek], size_of_encrypt_message);
			
			//printf("%s\n", "POP 4");
			
			// ПРОБА РАСШИФРОВКИ сообщения
			
			//Сюда будем записывать результат наложения ключа на зашифрованное сообщение
			char ckeck_decrypt[size_of_encrypt_message+1];
			memset(ckeck_decrypt,0,size_of_encrypt_message+1);
			
			 char* ckeck_decrypt_uc = reinterpret_cast< char*>(ckeck_decrypt);
			 char* real_key_uc = reinterpret_cast< char*>(real_key);
			
			//printf("%s\n", "POP 5");
			
			// Непосредственно сам процесс наложения ключа на зашифрованное сообщение
			int j = 0;
			for(j;j<size_of_encrypt_message;j++)
				{
						
					ckeck_decrypt_uc[j] = encrypted_message[j] ^ real_key_uc[j];
					
				}
			
			//printf("%s\n", "POP 6");
			
			
			int ch = 0;
			int count_separate = 0;
			int begin = 0;
			int end = 0;
			for(ch;ch<size_of_encrypt_message;ch++)
				{
					if (ckeck_decrypt_uc[ch] == ';')
						{
							count_separate++;
							if (count_separate == 1)
								begin = ch + 1;
								
							if (count_separate == 2)
								{
									end = ch - 1;
									break;
								}		
						
						}	
				}
			
			//printf("%s\n", "POP 7");
			
			
			//std::cout << "=============== ОТЛАДОЧНАЯ ИНФОРМАЦИЯ ================" << std::endl;
			int iii = 0;
			for (iii; iii < size_of_encrypt_message; iii++)
				{
				
					//std::cout << " ckeck_decrypt_uc[ "<<iii<<"] = " << ckeck_decrypt_uc[iii] << "int = "<< (int)ckeck_decrypt_uc[iii] << std::endl;
					//std::cout << " encrypted_message[ "<<iii<<"] = " << encrypted_message[iii] << "int = "<< (int)encrypted_message[iii] << std::endl;
					//std::cout << " real_key_uc[ "<<iii<<"] = " << real_key_uc[iii] << "int = "<< (int)real_key_uc[iii] << std::endl;
				
				}
				
			
			
			//std::cout << "====================== ОТЛАДОЧНАЯ ИНФОРМАЦИЯ ==========================" << std::endl;
			
			
			//std::cout << " begin = " << begin << std::endl;
			//std::cout << " end = " << end << std::endl;
			
			if (end < begin)
				{
					//std::cout << "не получилось расшифровать сообщение" << std::endl;
					continue;
				}
		
			int ssize = end - begin + 1;
			char who_sender[ssize+1];
			memset(who_sender,0,ssize+1);
			strncpy (who_sender,&ckeck_decrypt_uc[begin],ssize);
			
			int ch1 = 0;
			int count_separate1 = 0;
			int begin1 = 0;
			int end1 = 0;
			for(ch1;ch<size_of_encrypt_message;ch1++)
				{
					if (ckeck_decrypt_uc[ch1] == ';')
						{
							count_separate1++;
							if (count_separate1 == 2)
								begin1 = ch1 + 1;
								
							if (count_separate1 == 3)
								{
									end1 = ch1 - 1;
									break;
								}		
						
						}			
				}
			
			
			//std::cout << " begin1 = " << begin1 << std::endl;
			//std::cout << " end1 = " << end1 << std::endl;
			
			if (end1 < begin1)
				{
					//std::cout << "не получилось расшифровать сообщение" << std::endl;
					continue;
				}
			
			int ssize1 = end1 - begin1 + 1 ;
			char who_reciever[ssize1+1];
			memset(who_reciever,0,ssize1+1);
			strncpy (who_reciever,&ckeck_decrypt_uc[begin1],ssize1);
			
			//std::cout << "=============== ЕЩЕ ОТЛАДОЧНАЯ ИНФОРМАЦИЯ ================" << std::endl;
			int jjj = 0;
			for (jjj; jjj < ssize; jjj++)
				{
					//std::cout << " who_sender[ "<<jjj<<"] = " << who_sender[jjj] << "int = "<< (int)who_sender[jjj] << std::endl;
				}
				
			//std::cout << "====================== ЕЩЕ ОТЛАДОЧНАЯ ИНФОРМАЦИЯ ==========================" << std::endl;
			
			who_sender[ssize] = '\0';
			who_reciever[ssize1] = '\0';
			
			// Тут вот так крест на крест должно быть, это я напутал немного
			//std::cout << " who_sender =" << who_reciever << "|" << std::endl;
			//std::cout << " who_reciever =" << who_sender << "|" << std::endl;
			//std::cout << " MY_NAME.c_str() =" << MY_NAME.c_str() << "|" << std::endl;
			//std::cout << " MY_NAME.c_str() =" << users_list[i] << "|" << std::endl;
			
			//printf("%s\n", "POP 8");
			
			if ((strcmp(who_sender,MY_NAME.c_str()) == 0) || (strcmp(who_reciever,MY_NAME.c_str()) == 0))
			 if((strcmp(who_sender,users_list[i]) == 0) || (strcmp(who_reciever,users_list[i]) == 0))
				{
				
					// Смещаем указатель ключа на длину присланного сообщения
					//printf("%s\n", "POP 9");
					int new_current_seek = current_seek + size_of_encrypt_message;
					
					set_key_seek_for_user(PATH_Date_Base, users_list[i], new_current_seek);
					//printf("%s\n", "POP 10");
					flag = 0;
					const char* decrypt_message = reinterpret_cast<const char*>(ckeck_decrypt_uc);
					std::string decrypt_message_str (decrypt_message);
					*encrypt_mess_DEC = decrypt_message_str;
					
					//printf("%s\n", "POP 11");
					
					//printf("%s\n","Сообщение удачно расшифровано, т.к. who_sender == MY_NAME.c_str() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					
					break;
				}
			else
				{
				
					//printf("%s\n","Сообщение не расшифровано, т.к. who_sender != MY_NAME.c_str() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					continue;
				}			
			
		
		}

	return flag; // Если получилось расшифровать
}


int parser(std::string transaction, transaction_struct* struct_transaction)
{

// format_of_transaction: <transaction_id>;<Receiver>;<Sender>;<Time_of_sending>;<Decrypt_message>

	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"parser\"]\n");
	const char* transaction_body = transaction.c_str();
	
	//std::cout << "transaction_body = " << transaction_body << std::endl;
	
	int i = 0;
	for (i; i< MAX_TRANSACTION_LENGTH; i++)
		{
			if (transaction_body[i] == ';')
				{
					break;
				}
			
		}

	int size_transaction_id = i;
	char ID [size_transaction_id+1];
	strncpy(ID,transaction_body,size_transaction_id);
	ID[size_transaction_id] = '\0';

	//std::cout << "ID = " << ID << "|" <<std::endl;

	i++;

	for (i; i< MAX_TRANSACTION_LENGTH; i++)
		{
			if (transaction_body[i] == ';')
				{
					break;
				}
			
		}

	int size_reciever = i - 1 - size_transaction_id;
	char Reciever [size_reciever+1];
	strncpy(Reciever,&transaction_body[size_transaction_id + 1],size_reciever);
	Reciever[size_reciever] = '\0';

	//std::cout << "Reciever = " << Reciever << "|" <<std::endl;

	i++;

	for (i;i<MAX_TRANSACTION_LENGTH;i++)
		{
			if (transaction_body[i] == ';')
				{
					break;
				}
		}

	int size_sender = i - 2 - size_transaction_id - size_reciever;
	char Sender [size_sender+1];
	strncpy(Sender,&transaction_body[size_reciever + size_transaction_id + 2],size_sender);
	Sender[size_sender] = '\0';

	//std::cout << "Sender = " << Sender << "|" <<std::endl;

	i++;

	for (i;i<MAX_TRANSACTION_LENGTH;i++)
		{
			if (transaction_body[i] == ';')
				{
					break;
				}
		}

	int size_time_of_sending = i - 3 - size_reciever - size_sender - size_transaction_id ;
	char Time_of_sending [size_time_of_sending+1];
	strncpy(Time_of_sending,&transaction_body[size_reciever + size_sender + size_transaction_id + 3],size_time_of_sending);
	Time_of_sending[size_time_of_sending] = '\0';

	//std::cout << "Time_of_sending = " << Time_of_sending << "|" <<std::endl;

	i++;

	for (i;i<MAX_TRANSACTION_LENGTH;i++)
		{
			if (transaction_body[i] == ';')
				{
					break;
				}
		}

	int size_decrypt_message = i - 4 - size_reciever - size_sender - size_time_of_sending - size_transaction_id;
	char Decrypt_message [size_decrypt_message+1];
	strncpy(Decrypt_message,&transaction_body[size_reciever + size_sender + size_time_of_sending + size_transaction_id + 4], size_decrypt_message);
	Decrypt_message[size_decrypt_message] = '\0';

	//std::cout << "Decrypt_message = " << Decrypt_message << "|" <<std::endl;

	struct_transaction->ID = atoi(const_cast<const char*>(ID));
	struct_transaction->Reciever = const_cast<const char*>(Reciever);
	struct_transaction->Sender = const_cast<const char*>(Sender);
	struct_transaction->Time_of_sending = const_cast<const char*>(Time_of_sending);
	struct_transaction->Decrypt_message = const_cast<const char*>(Decrypt_message);

	return 0;
}

// Функция, которая будет срабатывать при отработке SQL-запроса к БД
static int callback_create(void *NotUsed, int argc, char **argv, char **azColName) {

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_create\"]\n");
   int i;
   for(i = 0; i<argc; i++) {
     // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
  // printf("\n");
   return 0;
}



static int callback_insert(void *NotUsed, int argc, char **argv, char **azColName) {

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_insert\"]\n");
   int i;
   for(i = 0; i<argc; i++) {
    //  printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
  // printf("\n");
   return 0;
}



static int callback_select(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select\"]\n");
   int i;
   //fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
     // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   //printf("\n");
   return 0;
}


static int callback_select_where(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_where\"]\n");
   int i;
  // fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
     // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   //printf("\n");
   return 0;
}



static int callback_select_user(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_user\"]\n");
   user_list_and_pointer* struct1 = (user_list_and_pointer*)data;
  // std::cout << "callback_select_user = "<< std::endl;
  
   struct1->user_list[(*struct1).pointer] = const_cast<const char*>(argv[0]);
  // std::cout << "USER # " << struct1->pointer << ": " << struct1->user_list[(*struct1).pointer] << ";" << std::endl;
   (*struct1).pointer = (*struct1).pointer + 1;   
   
 //  printf("\n");
   return 0;
}


static int callback_select_user_messages(void *data, int argc, char **argv, char **azColName){


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_user_messages\"]\n");
   int i;
     user_messages_list_and_pointer* struct2 = (user_messages_list_and_pointer*)data;
  // std::cout << "callback_select_user_messages = "<< std::endl;
   
   struct2->list_transaction_id[(*struct2).pointer] = const_cast<const char*>(argv[0]);
   struct2->list_time_of_sending[(*struct2).pointer] = const_cast<const char*>(argv[1]);
   struct2->list_hwo_sender[(*struct2).pointer] = const_cast<const char*>(argv[2]);
   struct2->list_hwo_reciever[(*struct2).pointer] = const_cast<const char*>(argv[3]);
   struct2->list_messages[(*struct2).pointer] = const_cast<const char*>(argv[4]);
   
  // std::cout << "Message # " << struct2->pointer << ":" <<" TRANSACTION_ID_IN_GLOBAL_BLOCKCHAIN = " << struct2->list_transaction_id[struct2->pointer] << ";" <<" time_of_sending = " << struct2->list_time_of_sending[(*struct2).pointer] << ";" << " hwo_sender = " << struct2->list_hwo_sender[(*struct2).pointer] << ";" << " hwo_reciever = " << struct2->list_hwo_reciever[(*struct2).pointer]  << ";" << " message = " << struct2->list_messages[(*struct2).pointer] << ";" << std::endl;
   (*struct2).pointer = (*struct2).pointer + 1; 
   
   
  // printf("\n");
   return 0;
}

static int callback_select_count_blockchain(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_count_blockchain\"]\n");
   int i;
   std::string* last_id =  (std::string*) data;
   *last_id = const_cast<const char*>(argv[0]);

  // std::cout << "COUNT_ID_BLOCKCHAIN_TABLE = " << *last_id << std::endl;
   
   return 0;
}



static int callback_select_string_where_blockchain(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_string_where_blockchain\"]\n");

   transaction_string_struct* struct_string_transaction = (transaction_string_struct*) data;
   
   struct_string_transaction->transaction_id = atoi(argv[1]);
   struct_string_transaction->transaction_body = argv[2];
   struct_string_transaction->transaction_size = atoi(argv[3]);
   struct_string_transaction->before_transaction_hash = argv[4];
   struct_string_transaction->transaction_hash = argv[5];
  
  // printf("\n");
   return 0;
}





static int callback_select_last_analysed_id(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_last_analysed_id\"]\n");
   std::string* last_analysed_id = (std::string*) data;
   *last_analysed_id = const_cast<const char*>(argv[0]);
 //  std::cout << "LAST_ANALYSED_ID_IN_BLOCKCHAIN = " << *last_analysed_id << std::endl;

   return 0;
}



static int callback_select_last_available_id(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_last_available_id\"]\n");
   std::string* last_available_id = (std::string*) data;
   *last_available_id = const_cast<const char*>(argv[0]);
 //  std::cout << "LAST_ANALYSED_ID_IN_BLOCKCHAIN = " << *last_analysed_id << std::endl;

   return 0;
}



static int callback_select_transaction_body_where_blockchain(void *data, int argc, char **argv, char **azColName){


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_transaction_body_where_blockchain\"]\n");
   std::string* transaction_body =  (std::string*) data;
   
   *transaction_body = const_cast<const char*>(argv[0]);

  // std::cout << "GET_TRANSACTION_BODY = " << *transaction_body << std::endl;
   
   return 0;
}


static int callback_select_count_new_message(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_select_count_new_message\"]\n");
   int* count_new_messages_now =  (int*) data;
   
   *count_new_messages_now = atoi(argv[0]);

 //  std::cout << "count_new_messages_now = " << *count_new_messages_now << std::endl;
   
   return 0;
}

static int callback_update_last_analysed_id(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_update_last_analysed_id\"]\n");
   int i;
 //  fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
  //    printf("UPDATE_LAST_ANALYSED_ID = %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
  // printf("\n");
   return 0;
}



static int callback_update_last_available_id(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_update_last_available_id\"]\n");
   int i;
 //  fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
  //    printf("UPDATE_LAST_ANALYSED_ID = %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
  // printf("\n");
   return 0;
}


static int callback_update_new_message(void *data, int argc, char **argv, char **azColName){
   
   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_update_new_message\"]\n");
   int i;
 //  fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
  //    printf("%s\n", "callback_update_new_message-выполнено");
   }
   
 //  printf("\n");
   return 0;
}


static int callback_delete_blockchain(void *data, int argc, char **argv, char **azColName){

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"callback_delete_blockchain\"]\n");
   int i;
  // fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
  //    printf("%s\n", "callback_delete_blockchain-выполнено");
   }
   
 //  printf("\n");
   return 0;
}



int get_last_analysed_id_in_blockchain(const char* path_to_DB, std::string* last_analysed_id)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_last_analysed_id_in_blockchain\"]\n");
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
              //  fprintf(stdout, "\nOpened database successfully\n");
            }

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;
  
         //   std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string string_sql = "SELECT LAST_ANALYSED_ID from CONFIG_TABLE";

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());

           // std::cout << "Вывод LAST_ANALYSED_ID из CONFIG_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_last_analysed_id, (void*)last_analysed_id, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе  LAST_ANALYSED_ID из CONFIG_TABLE" << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
              //  std::cout << "Вывод LAST_ANALYSED_ID из CONFIG_TABLE прошел успешно." << std::endl;
            }

        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
           // std::cout << "Файл БД закрыт." << std::endl;


	//std::cout << "Last_analysed_id = " << *last_analysed_id << std::endl;

    return 0;
}




int get_last_available_id_in_blockchain(const char* path_to_DB, std::string* last_available_id)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_last_available_id_in_blockchain\"]\n");
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
              //  fprintf(stdout, "\nOpened database successfully\n");
            }

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;
  
         //   std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string string_sql = "SELECT LAST_AVAILABLE_ID from CONFIG_TABLE";

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());

           // std::cout << "Вывод LAST_ANALYSED_ID из CONFIG_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_last_available_id, (void*)last_available_id, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе LAST_AVAILABLE_ID из CONFIG_TABLE" << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
              //  std::cout << "Вывод LAST_ANALYSED_ID из CONFIG_TABLE прошел успешно." << std::endl;
            }

        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
           // std::cout << "Файл БД закрыт." << std::endl;


	//std::cout << "Last_analysed_id = " << *last_analysed_id << std::endl;

    return 0;
}





int set_last_available_id_in_blockchain(const char* path_to_DB, int id)
{
 
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"set_last_available_id_in_blockchain\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function SET_LAST_AVAILABLE_ID_IN_BLOCKCHAIN called";


    
            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
              //  fprintf(stdout, "\nOpened database successfully\n");
            }

         //   std::cout << "\nФайл базы данных существует." << std::endl;
         //   std::cout << "Файл БД открыт." << std::endl;
    
        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

              /* Create SQL statement */

                std::string string_sql_0 = "UPDATE CONFIG_TABLE SET LAST_AVAILABLE_ID = " + std::to_string(id);
                std::string string_sql_1 = " WHERE ID = 1";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1;

                sql = const_cast<char*>(string_sql_full.c_str());

         //   std::cout << "Вывод результатов UPDATE CONFIG_TABLE SET LAST_ANALYSED_ID..." << std::endl;           

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_update_last_available_id, (void*)data, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе результатов UPDATE CONFIG_TABLE SET LAST_ANALYSED_ID из CONFIG_TABLE" << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
               // std::cout << "Вывод результатов UPDATE CONFIG_TABLE SET LAST_ANALYSED_ID из CONFIG_TABLE прошел успешно." << std::endl;
            }

          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;
            sqlite3_close(db);
         //   std::cout << "Файл БД закрыт." << std::endl;

    return 0;
}




int set_last_analysed_id_in_blockchain(const char* path_to_DB, int id)
{
 
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"set_last_analysed_id_in_blockchain\"]\n");
    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function SET_LAST_ANALYSED_ID_IN_BLOCKCHAIN called";


    
            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
              //  fprintf(stdout, "\nOpened database successfully\n");
            }

         //   std::cout << "\nФайл базы данных существует." << std::endl;
         //   std::cout << "Файл БД открыт." << std::endl;
    
        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

              /* Create SQL statement */

                std::string string_sql_0 = "UPDATE CONFIG_TABLE SET LAST_ANALYSED_ID = " + std::to_string(id);
                std::string string_sql_1 = " WHERE ID = 1";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1;

                sql = const_cast<char*>(string_sql_full.c_str());

         //   std::cout << "Вывод результатов UPDATE CONFIG_TABLE SET LAST_AVAILABLE_ID..." << std::endl;           

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_update_last_analysed_id, (void*)data, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе результатов UPDATE CONFIG_TABLE SET SET_LAST_ANALYSED_ID из CONFIG_TABLE" << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
               // std::cout << "Вывод результатов UPDATE CONFIG_TABLE SET LAST_AVAILABLE_ID из CONFIG_TABLE прошел успешно." << std::endl;
            }

          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;
            sqlite3_close(db);
         //   std::cout << "Файл БД закрыт." << std::endl;

    return 0;
}


int analyse_blockchain_for_input_messages(const char* path_to_DB)
{
    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"analyse_blockchain_for_input_messages\"]\n");
    // Пройдемся по всем ЕЩЕ не анализируеммым записям в BLOCKCHAIN_TABLE
    // Для этого в начале необходимо узнать последний ID, который анализировался на входящие сообщения

    std::string last_analysed_id_string;	
    int res1 = get_last_analysed_id_in_blockchain(path_to_DB, &last_analysed_id_string);

    if (res1 != 0)
        {
            std::cout << "При получении последнего проанализированного id произошла ошибка" << std::endl;
        }
    else
        {
           // std::cout << "LAST_ANALYSED_ID = " << last_analysed_id_string << std::endl;
           // std::cout << "При получении последнего проанализированного id ошибок не произошло." << std::endl;        
        }


    // Тут получили last_analysed_id в int
    int last_analysed_id = atoi(last_analysed_id_string.c_str());


    // А теперь необходимо запросить просто последний id

    std::string last_id_string;


    int res2 = how_many_transaction_id(path_to_DB, &last_id_string);
    if (res2 != 0)
        {
            std::cout << "При получении последнего id произошла ошибка" << std::endl;
        }
    else
        {
          //  std::cout << "LAST_ID = " << last_id_string << std::endl;
          //  std::cout << "При получении последнего id ошибок не произошло" << std::endl;        
        }

    // Тут получили last_id в int

    int last_id = atoi(last_id_string.c_str());

    // А теперь необходимо запрашивать только те TRANSACTION_BODY из BLOCKCHAIN_TABLE, которые имеют ID = (last_analysed_id, last_id];

    int ii = last_analysed_id+1; // last_analysed_id+1; - чтобы не анализировать уже проанализированный блок
    for (ii;ii<=last_id;ii++)  // ii<=last_id (меньше либо РАВНО last_id !!!)
        {
            // Делать запрос на encrypt_message

	    std::string transaction_body;	

            int res3 = get_transaction_body_where_id(path_to_DB, ii, &transaction_body);
            if (res3 != 0)
                {
                    std::cout << "При получении TRANSACTION_BODY where id = " << ii << " произошла ошибка." << std::endl;
                }
            else
                {
                	
                    // Тут мы полчаем наше зашифрованное сообщение ...	
                  //  std::cout << "TRANSACTION_BODY where id = " << ii << ": " << transaction_body << std::endl;
                 //   std::cout << "При получении TRANSACTION_BODY where id = " << ii << " ошибки не произошло." << std::endl;        
                }


            
            // std::string encrypt_mess = "Reciever:user1;Sender:user2;Time=12-02-2020;EM=lalala;"


                std::string transaction_body_dec;
                int res_dec = try_decrypt(transaction_body, &transaction_body_dec);
                if (res_dec != 0)
                    {
                    
                    
                    	 // После каждого пробега выставлять в поле LAST_ANALYSED_ID в CONFIG_TABLE
                	int res4 = set_last_analysed_id_in_blockchain(path_to_DB, ii);
                	if (res4 != 0)
                    		{
                       	 	std::cout << "При обновлении LAST_ANALYSED_TRANSACTION_ID произошла ошибка.(" << ii << ")"<< std::endl;
                    		}
                	else
                    		{
                       	 	//std::cout << "При обновлении LAST_ANALYSED_TRANSACTION_ID ошибки не произошло.(" << ii << ")" << std::endl;        
                    		}  
                    set_last_available_id_in_blockchain(path_to_DB, ii);
                    
                        //std::cout << "Не получилось расшифровать транзакцию (" << ii << ")" << std::endl;
                        continue;
                    }
                else
                    {
                    
                   	//std::cout << "transaction_body_dec = " << transaction_body_dec << std::endl;
                       //std::cout << "Получилось расшифровать транзакцию (" << ii << ")" << std::endl;
                       
                                                     
                    }

                transaction_struct struct_transaction;

                // Если сообщение расшифровалось, то можно отдать парсеру на обработку
                int res_pars = parser(transaction_body_dec, &struct_transaction); 

		if (res_pars != 0)
                    {
                        std::cout << "При парсинге транзанции произошла ошибка.(" << ii << ")"<< std::endl;
                    }
                else
                    {
                    
                    	// Выводим на экран строки распарсенной транзакции
                    //	std::cout << "struct_transaction.ID = " << struct_transaction.ID << std::endl;
                    //	std::cout << "struct_transaction.Sender = " << struct_transaction.Sender << std::endl;
                   // 	std::cout << "struct_transaction.Reciever = " << struct_transaction.Reciever << std::endl;
                   // 	std::cout << "struct_transaction.Time_of_sending = " << struct_transaction.Time_of_sending << std::endl;
                   // 	std::cout << "struct_transaction.Decrypt_message = " << struct_transaction.Decrypt_message << std::endl;
                   
                   //     std::cout << "При парсинге транзанции ошибки не произошло.(" << ii << ")" << std::endl;        
                    }  


		if (struct_transaction.Reciever != MY_NAME)
                  {
                  	// std::cout << "Ошибка! struct_transaction.Reciever != MY_NAME" << std::endl;
                  	// exit(1);
                  }


		if ((strcmp(struct_transaction.Reciever.c_str(),MY_NAME.c_str()) == 0) || (strcmp(struct_transaction.Sender.c_str(),MY_NAME.c_str()) == 0))
			{
				int res3_1;
				if (strcmp(struct_transaction.Sender.c_str(),MY_NAME.c_str()) == 0)
					{
					
					std::string message_for_add_str = analyse_input_message_for_file_content (struct_transaction.Decrypt_message.c_str());
					
                			res3_1 = add_record_to_user_table(path_to_DB, struct_transaction.Reciever, struct_transaction.ID, struct_transaction.Sender, struct_transaction.Reciever, struct_transaction.Time_of_sending, message_for_add_str);
                			}
                		else
                			{
                			std::string message_for_add_str = analyse_input_message_for_file_content (struct_transaction.Decrypt_message.c_str());
                			
                			res3_1 = add_record_to_user_table(path_to_DB, struct_transaction.Sender, struct_transaction.ID, struct_transaction.Sender, struct_transaction.Reciever, struct_transaction.Time_of_sending, message_for_add_str);
                			}	

                		if (res3_1 != 0)
                    			{
                        			std::cout << "При добавлении записи в пользовательскую таблицу произошла ошибка.(" << ii << ")"<< std::endl;
                    			}
                		else
                    			{
                    				// Добавляем +1 к NEW_MESSAGE
                    				if (strcmp(struct_transaction.Reciever.c_str(),MY_NAME.c_str()) == 0)
                    					update_new_message_count_to_user(path_to_DB, struct_transaction.Sender, 1);
                        			// std::cout << "При добавлении записи в пользовательскую таблицу ошибки не произошло.(" << ii << ")" << std::endl;        
                    			}  
                    			
                    			 
			}

                // После каждого пробега выставлять в поле LAST_ANALYSED_ID в CONFIG_TABLE
                int res4 = set_last_analysed_id_in_blockchain(path_to_DB, ii);
                if (res4 != 0)
                    {
                        std::cout << "При обновлении LAST_ANALYSED_TRANSACTION_ID произошла ошибка.(" << ii << ")"<< std::endl;
                    }
                else
                    {
                       // std::cout << "При обновлении LAST_ANALYSED_TRANSACTION_ID ошибки не произошло.(" << ii << ")" << std::endl;        
                    }  
                    
                set_last_available_id_in_blockchain(path_to_DB, ii);                  
        

        }


    //std::cout << "Весь блокчейн на предмет новых сообщений проанализирован." << std::endl;
    
    return 0;
}


int get_transaction_body_where_id(const char* path_to_DB, int id, std::string* transaction_body)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_transaction_body_where_id\"]\n");
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

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;
    
           // std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string string_sql = "SELECT TRANSACTION_BODY from BLOCKCHAIN_TABLE WHERE TRANSACTION_ID = " + std::to_string(id);
   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());


           // std::cout << "Вывод TRANSACTION_BODY из BLOCKCHAIN_TABLE, где ID = " << id << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_transaction_body_where_blockchain, (void*)transaction_body, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при вывод TRANSACTION_BODY из BLOCKCHAIN_TABLE, где ID = " << id << std::endl;
                exit(1);
            } else {
                //fprintf(stdout, "Operation done successfully\n");
                //std::cout << "Вывод TRANSACTION_BODY из BLOCKCHAIN_TABLE, где ID = " << id << " прошел успешно." << std::endl;
            }

           // std::cout << "\n ----------------------------------------------------- \n" << std::endl;
            sqlite3_close(db);
           // std::cout << "Файл БД закрыт." << std::endl;


    return 0;
}


int get_id_transaction(const char* path_to_DB, int id, transaction_string_struct* struct_string_transaction)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_id_transaction\"]\n");
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

          //  std::cout << "\nФайл базы данных существует." << std::endl;
          //  std::cout << "Файл БД открыт." << std::endl;
    


           // std::cout << "\n ----------------------------------------------------- \n" << std::endl;

    std::string string_sql = "SELECT * from BLOCKCHAIN_TABLE WHERE TRANSACTION_ID = " + std::to_string(id);

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql.c_str());


         //   std::cout << "Вывод строки из BLOCKCHAIN_TABLE, где ID = " << id << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_string_where_blockchain, (void*)struct_string_transaction, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при вывод строки из BLOCKCHAIN_TABLE, где ID = " << id << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
              //  std::cout << "Вывод строки из BLOCKCHAIN_TABLE, где ID = " << id << " прошел успешно." << std::endl;
            }

          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
          //  std::cout << "Файл БД закрыт." << std::endl;

    return 0;
}


int how_many_transaction_id (const char* path_to_DB, std::string* last_id)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"how_many_transaction_id\"]\n");
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

           // std::cout << "\nФайл базы данных существует." << std::endl;
           // std::cout << "Файл БД открыт." << std::endl;
    

          //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            // Create SQL statement 
            sql = "SELECT COUNT(*) from BLOCKCHAIN_TABLE";

          //  std::cout << "Вывод последнего ID из BLOCKCHAIN_TABLE..." << std::endl;            

            // Execute SQL statement 
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_count_blockchain, (void*)last_id, &zErrMsg);
   	    pthread_mutex_unlock(&mutex);
   	    
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе последнего ID из блокчейн таблицы БД." << std::endl;
                exit(1);
            } else {
              //  fprintf(stdout, "Operation done successfully\n");
              //  std::cout << "Вывод последнего ID из BLOCKCHAIN_TABLE прошел успешно завершен." << std::endl;
            }

         //   std::cout << "\n ----------------------------------------------------- \n" << std::endl;

            sqlite3_close(db);
         //   std::cout << "Файл БД закрыт." << std::endl;

    return 0;
}


// Функция проверки входящих сообщений для МЕНЯ от конкретного пользователя (при добавлении этого пользователя)

int analyse_blockchein_for_user_name(const char* path_to_DB, const char* user_name)
{


    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"analyse_blockchein_for_user_name\"]\n");
    // Тут получили last_analysed_id в int
    int last_analysed_id = 0;


    // А теперь необходимо запросить просто последний id

    std::string last_id_string;


    int res2 = how_many_transaction_id(path_to_DB, &last_id_string);
    if (res2 != 0)
        {
            std::cout << "При получении последнего id произошла ошибка" << std::endl;
        }
    else
        {
          //  std::cout << "LAST_ID = " << last_id_string << std::endl;
          //  std::cout << "При получении последнего id ошибок не произошло" << std::endl;        
        }

    // Тут получили last_id в int

    int last_id = atoi(last_id_string.c_str());

    // А теперь необходимо запрашивать только те TRANSACTION_BODY из BLOCKCHAIN_TABLE, которые имеют ID = (last_analysed_id, last_id];

    int ii = last_analysed_id+1; // last_analysed_id+1; - чтобы не анализировать уже проанализированный блок
    for (ii;ii<=last_id;ii++)  // ii<=last_id (меньше либо РАВНО last_id !!!)
        {
            // Делать запрос на encrypt_message

	    std::string transaction_body;	

            int res3 = get_transaction_body_where_id(path_to_DB, ii, &transaction_body);
            if (res3 != 0)
                {
                    std::cout << "При получении TRANSACTION_BODY where id = " << ii << " произошла ошибка." << std::endl;
                }
            else
                {
                	
                    // Тут мы полчаем наше зашифрованное сообщение ...	
                  //  std::cout << "TRANSACTION_BODY where id = " << ii << ": " << transaction_body << std::endl;
                  //  std::cout << "При получении TRANSACTION_BODY where id = " << ii << " ошибки не произошло." << std::endl;        
                }


            
            // std::string encrypt_mess = "Reciever:user1;Sender:user2;Time=12-02-2020;EM=lalala;"


                std::string transaction_body_dec;
                int res_dec = try_decrypt(transaction_body, &transaction_body_dec);
                if (res_dec != 0)
                    {
                      //  std::cout << "Не получилось расшифровать транзакцию (" << ii << ")" << std::endl;
                        continue;
                    }
                else
                    {
                    
                   //	std::cout << "transaction_body_dec = " << transaction_body_dec << std::endl;
                    //   std::cout << "Получилось расшифровать транзакцию (" << ii << ")" << std::endl;        
                    }

                transaction_struct struct_transaction;

                // Если сообщение расшифровалось, то можно отдать парсеру на обработку
                int res_pars = parser(transaction_body_dec, &struct_transaction); 

		if (res_pars != 0)
                    {
                        std::cout << "При парсинге транзанции произошла ошибка.(" << ii << ")"<< std::endl;
                    }
                else
                    {
                    
                    	// Выводим на экран строки распарсенной транзакции
                    //	std::cout << "struct_transaction.ID = " << struct_transaction.ID << std::endl;
                   //	std::cout << "struct_transaction.Sender = " << struct_transaction.Sender << std::endl;
                   // 	std::cout << "struct_transaction.Reciever = " << struct_transaction.Reciever << std::endl;
                   // 	std::cout << "struct_transaction.Time_of_sending = " << struct_transaction.Time_of_sending << std::endl;
                    //	std::cout << "struct_transaction.Decrypt_message = " << struct_transaction.Decrypt_message << std::endl;
                   
                    //    std::cout << "При парсинге транзанции ошибки не произошло.(" << ii << ")" << std::endl;        
                    }  


		if (struct_transaction.Reciever != MY_NAME)
                  {
                  	// std::cout << "Ошибка! struct_transaction.Reciever != MY_NAME" << std::endl;
                  	//exit(1);
                  }


		if ((strcmp(struct_transaction.Reciever.c_str(),MY_NAME.c_str()) == 0) && (strcmp(struct_transaction.Sender.c_str(),user_name) == 0))
			{
				int res3_1;
				std::string message_for_add_str = analyse_input_message_for_file_content (struct_transaction.Decrypt_message.c_str());
                		
				
				res3_1 = add_record_to_user_table(path_to_DB, struct_transaction.Sender, struct_transaction.ID, struct_transaction.Sender, struct_transaction.Reciever, struct_transaction.Time_of_sending, message_for_add_str);	

                		if (res3_1 != 0)
                    			{
                        			std::cout << "При добавлении записи в пользовательскую таблицу USER NAME ONLY произошла ошибка.(" << ii << ")"<< std::endl;
                    			}
                		else
                    			{
                    				// Добавляем +1 к NEW_MESSAGE
                    				if (strcmp(struct_transaction.Reciever.c_str(),MY_NAME.c_str()) == 0)
                    					update_new_message_count_to_user(path_to_DB, struct_transaction.Sender, 1);
                        			// std::cout << "При добавлении записи в пользовательскую таблицу USER NAME ONLY ошибки не произошло.(" << ii << ")" << std::endl;        
                    			}  
                    			
                    			 
			}
			
			// Вернуть seek на свое место (но пока что оставить)

        }

    // std::cout << "Весь блокчейн на предмет новых сообщений проанализирован для пользователя " << user_name << std::endl;
    
    return 0;
}




// Функция создания таблицы для нового пользователя

int create_table_for_new_user (const char* path_to_DB, std::string new_user_name, std::string path_to_key_file)
{

    write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"create_table_for_new_user\"]\n");
    //Проверка существования файла БД
    // std::cout << "Проверка существования файла БД ..." <<std::endl;    
    int file_exist = checkFile(path_to_DB);
    


    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;


    // Для чтения значения файла-ключа
    
    int fsize;
    char* buffer_key;


    if (file_exist == 1)
        {
          //  std::cout << "\nФайла базы данных существует." << std::endl;

            /* Open database */
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
             //   fprintf(stdout, "Opened database successfully\n");
            }
         //   std::cout << "Файл БД открыт." << std::endl;
//user_pause();

            // Создание ТАБЛИЦЫ БД для нового пользователя



            int check_key_file_result = checkFile(path_to_key_file.c_str());
            if (check_key_file_result == -1)
            {
                set_color(32);
                std::cout << "Файла ключа для данного пользователя не найдено.";
                fflush (stdout);
                //set_cursor_position (start_line + 1, 4);
                //start_line++;
                return -1;                
            }

            std::string string_sql_0 = "CREATE TABLE " + new_user_name;
            std::string string_sql_1 = " (ID INTEGER PRIMARY KEY  AUTOINCREMENT   NOT NULL,";
            std::string string_sql_2 = "ID_TRANSACTION_IN_GLOBAL_BLOCKCHAIN INT      NOT NULL,";
            std::string string_sql_3 = "HWO_SENDER TEXT      NOT NULL,";
            std::string string_sql_4 = "HWO_RECEIVER TEXT      NOT NULL,";
            std::string string_sql_5 = "TIME_OF_SENDING TEXT      NOT NULL,";
            std::string string_sql_6 = "MESSAGE TEXT      NOT NULL);";

            /* Create SQL statement */

            std::string string_sql_full = string_sql_0 + \
                                          string_sql_1 + \
                                          string_sql_2 + \
                                          string_sql_3 + \
                                          string_sql_4 + \
                                          string_sql_5 + \
                                          string_sql_6;

             sql = const_cast<char*>(string_sql_full.c_str());

            /* Execute SQL statement */
            	pthread_mutex_lock(&mutex);
              rc = sqlite3_exec(db, sql, callback_create, 0, &zErrMsg);
              pthread_mutex_unlock(&mutex);
   
              if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при создании таблицы БД для нового пользователя." << std::endl;
                fflush(stdout);
                sleep(5);
		        return -1;                
		//exit(1);
              } else {
              //  fprintf(stdout, "Table created successfully\n");
              //  std::cout << "Таблица БД для нового пользователя успешно создана." << std::endl;
              }


            // Проверяем существование файла ключа для этого нового пользователья
            check_key_file_result = checkFile(path_to_key_file.c_str());

            //user_pause();

            if (check_key_file_result == -1)
                {
                    set_color(32);
                    std::cout << "Файла ключа для данного пользователя не найдено.";
                    fflush (stdout);
                    //set_cursor_position (start_line + 1, 4);
                    //start_line++;
                    return -1;                
                }
            else if (check_key_file_result == 1)
                {

                   // std::cout << "Файл ключа для данного пользователя найден." << std::endl; 

                     // Узнаем размер файла
                    fsize = filesize(path_to_key_file.c_str());
                    //std::cout << "Размер файла-ключа: " << fsize << " байт." << std::endl;

                     // Считываем содержимое файла в переменную "key"
                                  

                      // allocate memory:
                    
                    buffer_key = new char [fsize];
                    int res = read_file_into_memory(path_to_key_file.c_str(),buffer_key,fsize);
                    
                    if (res != 0)
                        {
                            std::cout << "При чтении файла-ключа произошла ошибка." << std::endl;
                            delete[] buffer_key;
                            fflush(stdout);
                            sleep(5);
                            return -1;                        
                        }
                    else if (res == 0)
                        {
                          //  std::cout << "Файл-ключ успешно считан в память." << std::endl;

                        }
 
                }

//user_pause();

              // Добавление записи в ГЛАВНУЮ ТАБЛИЦУ БД о новом пользователе

              /* Create SQL statement */

            std::string string_sql_7 = "INSERT INTO GENERAL_TABLE(USER_NAME,KEY,KEY_SIZE,KEY_SEEK,NEW_MESSAGE) ";
            std::string string_sql_8 = "VALUES ('" + new_user_name;
            std::string buffer_key_string (buffer_key, fsize);
            std::string string_sql_9 = "','" + buffer_key_string;
            std::string string_sql_10 = "'," + std::to_string(fsize);
            std::string string_sql_11 = "," + std::to_string(0);
            std::string string_sql_12 = "," + std::to_string(0);
            std::string string_sql_13 = " );";

            std::string string_sql_full_2 = string_sql_7 + \
                                          string_sql_8 + \
                                          string_sql_9 + \
                                          string_sql_10 + \
                                          string_sql_11 + \
                                          string_sql_12 + \
                                          string_sql_13;

            
   sql = const_cast<char*>(string_sql_full_2.c_str());

   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_insert, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      exit(1);
   } else {
      // fprintf(stdout, "Records created successfully\n");
   }

              delete[] buffer_key;
              sqlite3_close(db);
            //  std::cout << "Файл БД закрыт." << std::endl;
              
              // Удаление оригинала файла-ключа

              if(remove(path_to_key_file.c_str())) {
                //printf("Error removing file-key");
               
                return -1;
                }

             // std::cout << "Файл-ключ скопирован в БД. Оригинал файла-ключа успешно удален." << std::endl;
              
              //user_pause();
              analyse_blockchein_for_user_name(path_to_DB, new_user_name.c_str());
              

             // std::cout << "Новый пользователь успешно добавлен." << std::endl;
         
              return 0;

        }    
    else if (file_exist == -1)
        {

          // std::cout << "Ошибка при попытке открытия файла БД. Файла БД не существует." << std::endl;
         //  std::cout << "Новый пользователь не добавлен." << std::endl;
           
           return -1;
        }
}



// Вернуть список моих пользователей

int return_my_user_list (const char* path_to_db, user_list_and_pointer* struct1)
{


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"return_my_user_list\"]\n");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;   
   
   //const char* data = "Callback function called SHOW MY USER_LIST";

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
     // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
     // std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
     // fprintf(stderr, "Opened database successfully\n");
      //std::cout << "Файл БД успешно открыт." << std::endl;
   }

        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

                           /* Create SQL statement */
            sql = "SELECT USER_NAME from GENERAL_TABLE";

       //     std::cout << "Вывод имен пользователей из таблицы GENERAL_TABLE..." << std::endl;            

            /* Execute SQL statement */
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_user, (void*)struct1, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе имен пользователей из таблицы GENERAL_TABLE." << std::endl;
                exit(1);
            } else {
         
         
             //   fprintf(stdout, "Operation done successfully\n");
             //   std::cout << "Вывод имен пользователей из таблицы GENERAL_TABLE успешно завершен." << std::endl;
            }

         //  std::cout << "\n ----------------------------------------------------- \n" << std::endl;


   sqlite3_close(db);
  // std::cout << "Файл базы данных успешно закрыт." << std::endl;
   
   
   return 0; 

}


// Вернуть список сообщений с пользователем

int return_my_messages_with_user (const char* path_to_db, std::string user_name, user_messages_list_and_pointer* struct2)
{

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"return_my_messages_with_user\"]\n");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called SHOW MY MESSAGES_LIST with USER";

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
    //  fprintf(stderr, "Opened database successfully\n");
    //  std::cout << "Файл БД успешно открыт." << std::endl;
   }


      //      std::cout << "\n ----------------------------------------------------- \n" << std::endl;

                           /* Create SQL statement */

            std::string string_sql_0 = "SELECT ID_TRANSACTION_IN_GLOBAL_BLOCKCHAIN, TIME_OF_SENDING, HWO_SENDER, HWO_RECEIVER, MESSAGE ";
            std::string string_sql_1 = "FROM " + user_name;
            std::string string_sql_2 = ";";

            std::string string_sql_full = string_sql_0 + string_sql_1 + string_sql_2;

            sql = const_cast<char*>(string_sql_full.c_str());

       //     std::cout << "Вывод имен пользователей из таблицы" << user_name << "..." << std::endl;            

            /* Execute SQL statement */
            pthread_mutex_lock(&mutex);
            rc = sqlite3_exec(db, sql, callback_select_user_messages, (void*)struct2, &zErrMsg);
            pthread_mutex_unlock(&mutex);
   
            if( rc != SQLITE_OK ) {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при выводе сообщений с пользователем из таблицы " << user_name << "." << std::endl;


		// (30.07.21)
		sqlite3_close(db);
		return -1;
		// (30.07.21)

                exit(1);
            } else {
          //      fprintf(stdout, "Operation done successfully\n");
         //       std::cout << "Вывод сообщений с пользователем из таблицы " << user_name << " успешно завершен." << std::endl;
            }

        //    std::cout << "\n ----------------------------------------------------- \n" << std::endl;

   sqlite3_close(db);
  // std::cout << "Файл базы данных успешно закрыт." << std::endl;
   return 0; 

}


// Добавление записи в пользовательскую таблицу

int add_record_to_user_table(const char* path_to_db, std::string user_name, int id, std::string hwo_sender, std::string hwo_reciever, std::string time_of_sending, std::string decrypt_message)
{


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"add_record_to_user_table\"]\n");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
    //  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    //  std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
    //  fprintf(stderr, "Opened database successfully\n");
    //  std::cout << "Файл БД успешно открыт." << std::endl;
   }

    std::string string_sql_00 = "INSERT INTO " + user_name;
    std::string string_sql_01 = " (ID_TRANSACTION_IN_GLOBAL_BLOCKCHAIN,HWO_SENDER,HWO_RECEIVER,TIME_OF_SENDING,MESSAGE) VALUES (";
    std::string string_sql_0 = std::to_string(id) + ", '";
    std::string string_sql_1 = hwo_sender + "', '";
    std::string string_sql_2 = hwo_reciever + "', '";
    std::string string_sql_3 = time_of_sending + "', '";
    std::string string_sql_4 = decrypt_message + "' ";
    std::string string_sql_5 = ");";

    std::string string_sql_full = string_sql_00 + \
                                  string_sql_01 + \
                                  string_sql_0 + \
                                  string_sql_1 + \
                                  string_sql_2 + \
                                  string_sql_3 + \
                                  string_sql_4 + \
                                  string_sql_5;

   /* Create SQL statement */
   sql = const_cast<char*>(string_sql_full.c_str());



   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_insert, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    //  std::cout << "При добавлении нового сообщения в пользовательскую таблицу произошла ошибка." << std::endl;
      return -1;
   } else {
    //  fprintf(stdout, "Records created successfully\n");
    //  std::cout << "Новая сообщение успешно добавлено в пользовательскую таблицу." << std::endl;
   }
   sqlite3_close(db);
  // std::cout << "Файл базы данных успешно закрыт." << std::endl;
   
   return 0; 
   
}


// Добавление записи в BLOCKCHAIN таблицу

int add_record_to_local_blockchain(const char* path_to_db, int id, const char* encrypt_message_c, const char* hesh_before_c, const char* hesh_this_c)
{

	write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"add_record_to_local_blockchain\"]\n");
//Проверим encrypt_message_c на ковычки
/*
char new_string_encrypt_meaasage [BUF_SIZE];
memset(new_string_encrypt_meaasage,0,BUF_SIZE);

int length = strlen(encrypt_message_c);
int i = 0;
for(i;i<length;i++)
	{
	
		if (encrypt_message_c[i] == ''')
			{
				
			}
	
	}

*/

std::string encrypt_message = encrypt_message_c;
std::string hesh_before = hesh_before_c;
std::string hesh_this = hesh_this_c;

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
   //   fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   //   std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
    //  fprintf(stderr, "Opened database successfully\n");
    //  std::cout << "Файл БД успешно открыт." << std::endl;
   }


pthread_mutex_lock(&mutex);
// checking --------------------------

	sqlite3_stmt *stmt = NULL;
	sqlite3_prepare_v2 (db,"INSERT INTO BLOCKCHAIN_TABLE (TRANSACTION_ID,TRANSACTION_BODY,TRANSACTION_SIZE,BEFORE_TRANSACTION_HASH,TRANSACTION_HASH) VALUES (?,?,?,?,?)",-1,&stmt, NULL);
	assert (rc == SQLITE_OK && stmt);
	rc = sqlite3_bind_int (stmt,1,id);
	assert (rc == SQLITE_OK);
	rc = sqlite3_bind_text (stmt,2,encrypt_message.c_str(), -1, SQLITE_STATIC);
	assert (rc == SQLITE_OK);
	rc = sqlite3_bind_int (stmt,3,encrypt_message.size());
	assert (rc == SQLITE_OK);
	rc = sqlite3_bind_text (stmt,4,hesh_before.c_str(), -1, SQLITE_STATIC);
	assert (rc == SQLITE_OK);
	rc = sqlite3_bind_text (stmt,5,hesh_this.c_str(), -1, SQLITE_STATIC);
	assert (rc == SQLITE_OK);
	rc = sqlite3_step (stmt);
	assert (rc == SQLITE_DONE);
	
	sqlite3_finalize(stmt);
	

// checking --------------------------
 pthread_mutex_unlock(&mutex);

/*
    std::string string_sql_00 = "INSERT INTO BLOCKCHAIN_TABLE (TRANSACTION_ID,TRANSACTION_BODY,TRANSACTION_SIZE,BEFORE_TRANSACTION_HASH,TRANSACTION_HASH) VALUES (";
    std::string string_sql_0 = std::to_string(id) + ", '";
    std::string string_sql_1 = encrypt_message + "', ";
    std::string string_sql_2 = std::to_string(encrypt_message.size()) + ", '";
    std::string string_sql_3 = hesh_before + "', '";
    std::string string_sql_4 = hesh_this + "' ";
    std::string string_sql_5 = ");";

    std::string string_sql_full = string_sql_00 + \
                                  string_sql_0 + \
                                  string_sql_1 + \
                                  string_sql_2 + \
                                  string_sql_3 + \
                                  string_sql_4 + \
                                  string_sql_5;
  

  //  Create SQL statement 
   sql = const_cast<char*>(string_sql_full.c_str());
   


	std::cout << "sql = " << sql << std::endl;

   // Execute SQL statement 
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_insert, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      std::cout << "При добавлении транзакции в блокчейн произошла ошибка." << std::endl;
      return -1;
   } else {
      fprintf(stdout, "Records created successfully\n");
      std::cout << "Новая транзакция успешно добавлена в блокчейн." << std::endl;
   }
   
 
   */
   
   sqlite3_close(db);
 //  std::cout << "Файл базы данных успешно закрыт." << std::endl;
   
   return 0; 
   
}





int get_count_new_message(const char* path_to_db, std::string user_name)
{


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"get_count_new_message\"]\n");

	int count_now;
	sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
  //    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  //    std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
   //   fprintf(stderr, "Opened database successfully\n");
  //    std::cout << "Файл БД успешно открыт." << std::endl;
   }



	              /* Create SQL statement */


                std::string string_sql_0 = "SELECT NEW_MESSAGE from GENERAL_TABLE" ;
                std::string string_sql_1 = " WHERE USER_NAME = '";
                std::string string_sql_2 = user_name + "'";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1 + \
                                                string_sql_2;

                sql = const_cast<char*>(string_sql_full.c_str());

   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_select_count_new_message, &count_now, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
   //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   //   std::cout << "При запросе NEW_MESSAGES из GENERAL_TABLE произошла ошибка." << std::endl;
      return -1;
   } else {
  //    fprintf(stdout, "Records created successfully\n");
  //    std::cout << "При запросе NEW_MESSAGES из GENERAL_TABLE ошибки не произошло." << std::endl;
   }
   sqlite3_close(db);
  // std::cout << "Файл базы данных успешно закрыт." << std::endl;

	return count_now;
}


int update_new_message_count_to_user(const char* path_to_db, std::string user_name, int add_or_null)
{


write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"update_new_message_count_to_user\"]\n");
// Проверка на правильность введенного параметра
if ((add_or_null != 0) && (add_or_null != 1))
	{
		std::cout << "Введен недопустимый параметр <add_or_null>: " <<  add_or_null << ". Используйте 1-для того чтобы увеличить значение на 1 и 0 - для того, чтобы обнулить значение NEW_MESSAGE из GENERAL_TABLE для " << user_name << std::endl;
		exit(1);
	}
	

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
    //  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   //   std::cout << "Не удалось открыть файл БД." << std::endl;
      
      return -1;
   } else {
   //  fprintf(stderr, "Opened database successfully\n");
   //   std::cout << "Файл БД успешно открыт." << std::endl;
   }

int new_value;	

if (add_or_null == 1)
	{
		// Получить текущее значение NEW_MESSAGE у user_name (из GENERAL_TABLE)
		new_value = get_count_new_message(path_to_db,user_name);
		new_value++;
	}	
if (add_or_null == 0)
	{
		new_value = 0;
	}	

	              /* Create SQL statement */


                std::string string_sql_0 = "UPDATE GENERAL_TABLE SET NEW_MESSAGE = " + std::to_string(new_value);
                std::string string_sql_1 = " WHERE USER_NAME = '";
                std::string string_sql_2 = user_name + "'";


                std::string string_sql_full = string_sql_0 + \
                                                string_sql_1 + \
                                                string_sql_2;

                sql = const_cast<char*>(string_sql_full.c_str());

   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_update_new_message, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    //  std::cout << "При обновлении NEW_MESSAGE в GENERAL_TABLE произошла ошибка." << std::endl;
      
      return -1;
   } else {
    //  fprintf(stdout, "Records created successfully\n");
   //   std::cout << "При обновлении NEW_MESSAGE в GENERAL_TABLE ошибок не произошло." << std::endl;
   }
   sqlite3_close(db);
 //  std::cout << "Файл базы данных успешно закрыт." << std::endl;
   
   return 0; 
}




int Clear_Blockchain_Table(const char* path_to_db)
{

   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"Clear_Blockchain_Table\"]\n");
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open(path_to_db, &db);
   
   if( rc ) {
   //   fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   //   std::cout << "Не удалось открыть файл БД." << std::endl;
      return -1;
   } else {
   //   fprintf(stderr, "Opened database successfully\n");
   //   std::cout << "Файл БД успешно открыт." << std::endl;
   }


	/* Create SQL statement */

        sql = "DELETE FROM BLOCKCHAIN_TABLE;";

   /* Execute SQL statement */
   pthread_mutex_lock(&mutex);
   rc = sqlite3_exec(db, sql, callback_delete_blockchain, 0, &zErrMsg);
   pthread_mutex_unlock(&mutex);
   
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    //  std::cout << "При очищении BLOCKCHAIN_TABLE произошла ошибка." << std::endl;
      return -1;
   } else {
    //  fprintf(stdout, "Records created successfully\n");
    //  std::cout << "При очищении BLOCKCHAIN_TABLE ошибки не произошло." << std::endl;
   }
   sqlite3_close(db);
  // std::cout << "Файл базы данных успешно закрыт." << std::endl;
   return 0; 

}



int Create_or_Enter_to_DB (const char* path_to_DB)
{


   write_to_log_file (LOG_FILE_NAME, "Cpp_part [FN: \"Create_or_Enter_to_DB\"]\n");
    //Проверка существования файла БД
  //  std::cout << "Проверка существования файла БД ..." <<std::endl;    
    int file_exist = checkFile(path_to_DB);

    // Переменные, необходимы для работы БД
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";


    if (file_exist == -1)
        {
         //   std::cout << "\nФайла базы данных еще не существует." << std::endl;

            /* Open database */
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
          //      fprintf(stdout, "Opened database successfully\n");
            }

        //    std::cout << "А теперь существует." << std::endl;
         //   std::cout << "Файл БД открыт." << std::endl;


            // Создание ОСНОВНОЙ ТАБЛИЦЫ БД

            /* Create SQL statement */
             sql = "CREATE TABLE GENERAL_TABLE("  \
             "ID INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL," \
             "USER_NAME           TEXT    NOT NULL," \
             "KEY            TEXT     NOT NULL," \
             "KEY_SIZE        INT     NOT NULL," \
             "KEY_SEEK        INT     NOT NULL," \
             "NEW_MESSAGE        INT     NOT NULL);";


            /* Execute SQL statement */
            pthread_mutex_lock(&mutex);
              rc = sqlite3_exec(db, sql, callback_create, 0, &zErrMsg);
              pthread_mutex_unlock(&mutex);
   
              if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при создании главной таблицы БД." << std::endl;
                exit(1);
              } else {
              //  fprintf(stdout, "Table created successfully\n");
              //  std::cout << "Главная таблица БД успешно создана." << std::endl;
              }

            // Создание BLOCKCHAIN ТАБЛИЦЫ БД
            
            /* Create SQL statement */
             sql = "CREATE TABLE BLOCKCHAIN_TABLE("  \
             "ID INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL," \
             "TRANSACTION_ID INT     NOT NULL," \
             "TRANSACTION_BODY           TEXT    NOT NULL," \
             "TRANSACTION_SIZE            INT     NOT NULL," \
             "BEFORE_TRANSACTION_HASH        TEXT     NOT NULL," \
             "TRANSACTION_HASH        TEXT     NOT NULL);";

            /* Execute SQL statement */
            pthread_mutex_lock(&mutex);
              rc = sqlite3_exec(db, sql, callback_create, 0, &zErrMsg);
              pthread_mutex_unlock(&mutex);
   
              if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при создании блокчейн таблицы БД." << std::endl;
                exit(1);
              } else {
             //   fprintf(stdout, "Table created successfully\n");
             //   std::cout << "Блокчейн таблица БД успешно создана." << std::endl;
              }

            // Создание CONFIG ТАБЛИЦЫ БД

             /* Create SQL statement */
             sql = "CREATE TABLE CONFIG_TABLE("  \
             "ID INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL," \
             "LAST_ANALYSED_ID INT    NOT NULL," \
             "LAST_AVAILABLE_ID INT    NOT NULL," \
             "CONFIG_BODY        TEXT     NOT NULL);";

            /* Execute SQL statement */
            pthread_mutex_lock(&mutex);
              rc = sqlite3_exec(db, sql, callback_create, 0, &zErrMsg);
              pthread_mutex_unlock(&mutex);
   
              if( rc != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                std::cout << "Ошибка при создании конфигурационной таблицы БД." << std::endl;
                exit(1);
              } else {
            //    fprintf(stdout, "Table created successfully\n");
            //    std::cout << "Конфигурационная таблица БД успешно создана." << std::endl;
              }


            // Заполнение CONFIG ТАБЛИЦЫ БД настройками по умолчанию

              /* Create SQL statement */

                std::string string_sql_13 = "INSERT INTO CONFIG_TABLE(ID,LAST_ANALYSED_ID,LAST_AVAILABLE_ID,CONFIG_BODY) VALUES (1,1,1,";
                std::string string_sql_14 = "'1\n0\n0\npassword\n0\n0\n\n'";
                std::string string_sql_15 = " );";

                std::string string_sql_full_3 = string_sql_13 + \
                                                string_sql_14 + \
                                                string_sql_15;

                sql = const_cast<char*>(string_sql_full_3.c_str());

                /* Execute SQL statement */
                pthread_mutex_lock(&mutex);
                rc = sqlite3_exec(db, sql, callback_insert, 0, &zErrMsg);
                pthread_mutex_unlock(&mutex);
   
                if( rc != SQLITE_OK ){
              //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
                		sqlite3_free(zErrMsg);
                		exit(1);
                	}
                else 
                	{
                		//fprintf(stdout, "Records created successfully\n");
                		//std::cout << "Настройки конфигурационного файла по умолчанию установлены." << std::endl;
                		//std::cout << "Для того, чтобы настройки начали действовать, необходимо перезагрузить программу." << std::endl;
                	}

            // Создание первой записи в BLOOCKCHAIN_TABLE БД (она будет у всех пользователей всегда одинаковая)

                int default_id = 1;
                std::string encrypt_message_default = "1;1;1;0;encrypt_message;";
                std::string default_hesh_before = "0123456789";
                
                // Хэш будем считать вот так
                //std::string default_this_hash = get_hash (encrypt_message_default);
                // Но пока не можем поэтому пока что напишем вот так
                std::string default_this_hash = "9876543210";

	

                int default_blockchain_add = add_record_to_local_blockchain(path_to_DB, default_id, encrypt_message_default.c_str(), default_hesh_before.c_str(), default_this_hash.c_str());
                
                if (default_blockchain_add != 0)
                    {
                        std::cout << "При добавлении в BLOCKCHAIN_TABLE записи по умолчанию произошла ошибка." << std::endl; 
                        exit(1);                       
                    }
                else
                    {
                        //std::cout << "При добавлении в BLOCKCHAIN_TABLE записи по умолчанию ошибок не выявлено." << std::endl;                    
                    }


                sqlite3_close(db);
               // std::cout << "Файл БД закрыт." << std::endl;
               
                return 0;

        }    
    else if (file_exist == 1)
        {

            // Open database
            rc = sqlite3_open(path_to_DB, &db);

            // Проверка корректности создания/открытия файла БД
            if( rc ) {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                exit(1);
            } else {
             //   fprintf(stdout, "\nOpened database successfully\n");
            }
            sqlite3_close(db);
       //     std::cout << "Файл БД закрыт." << std::endl;
            
            return 0;
        }
        
        return -1;
 }

int getline_with_block_from_file(std::string& line, std::string filename)
{
	sleep(1);
	// ------ GET_LINE_BEGIN --------------------------------

	// ---------------Blocking_BEGIN ----------------------
	std::fstream cin(filename);
	if (!cin) cin.open(filename, std::fstream::in);
    	int fd = static_cast< __gnu_cxx::stdio_filebuf< char > * const >( cin.rdbuf() )->fd();
	int res_block = flock(fd,LOCK_EX);
    	if(res_block == -1)
    	{
		perror ("Unsuccessful_unblock");
        	return 1;
    	}
	// ---------------Blocking_END ----------------------

	std::getline(cin, line);
	
	// ---------------Unblocking_BEGIN ----------------------
	cin.clear();   // clear eof bit.
    	cin.seekp( 0 );
	
	res_block = flock(fd,LOCK_UN);
        if(res_block == -1)
        {
            perror ("Unsuccessful_unblock");
            return 1;
        }
	cin.close();
	
	// Очистка содержимого файла
	
	cin.open(filename, std::fstream::out | std::fstream::trunc);
	cin.close();
	//std::cout << "file cleared!" << std::endl;
	// ---------------Unblocking_END ----------------------


	// ------ GET_LINE_END --------------------------------

	return 0;
}

