#include <iostream>
#include "Cpp_part.h"

extern "C" {
#include "C_part.h"
}

boost::asio::deadline_timer* timer_my;
boost::asio::io_context* io_my;

const char* ARV_FN;
const char* ARGV_FN;

std::string Big_files_dir_output = "BIG_FILES_OUTPUT/";
std::string Big_files_dir_input = "BIG_FILES_INPUT/";
std::string path_to_temp_dir = "TEMP_DIR/";
std::string path_to_reference_files_dir = "INPUT_FILES/";

const int MAX_count_of_users = 2; // for i2pd-SAM threads

int bad_recieve = 0;
int gen_return = -1;

int port_cs;
int port_sc;
int sending_file_flag = 0;

const char* Hostname_Center_Server; // = "192.168.0.11"; // Для функции: "hello_and_get_user_list" и программы: "./netserver".
int port1; // = 2000; // Для функции: "hello_and_get_user_list" и программы: "./netserver".
int port2; // = 2001; // Для функций: "get_hash_or_blockchain", "hash_and_blockchain_server_part_function", "get_actual_blockchain_copy".
int port3; // = 2002; // Для функций: "func_send_message_server_part_function", "send_message".
int SAMPort;
SAM::I2pSocket* socket_send_message[MAX_count_of_users];
SAM::I2pSocket* socket_hash_and_blockchain[MAX_count_of_users];
const char* my_port_hash_blockain;
const char* my_port_send_message;
const char* use_static_tunnels;

std::string MY_NAME;
const char* PATH_Date_Base;
int start_line;

int limit_number_of_transaction = 5; //15;

const char* LOG_FILE_NAME;

std::string my_public_dest_hash_and_blockchain; 
std::string my_public_send_message;

std::shared_ptr<SAM::StreamSession> session_ptr_hash_blockchain;
std::shared_ptr<SAM::StreamSession> session_ptr_send_message;


int main(int argc, const char* argv[]) 
{


// Увеличение размера стека до 1 Гб

const rlim_t kStackSize = 1024 * 1024 * 1024; // stack size = 1 Gb
struct rlimit rl;
int result;
result = getrlimit(RLIMIT_STACK, &rl);
if (result == 0)
{
	if (rl.rlim_cur < kStackSize)
	{
		rl.rlim_cur = kStackSize;
		result = setrlimit(RLIMIT_STACK, &rl);
		if (result != 0)
		{
			fprintf(stderr, "setrlimit returned result = %d\n", result);
			printf("ERROR. setrlimit returned result = %d\n", result);
			fflush(stdout);
			sleep(15);
			exit(1);
		}
	}
}






// Имя файлу, куда будут записываться Log'и программы
LOG_FILE_NAME = "log_file.txt";
//printf("argc = %d", argc);
if (argc != 13)
	{
		std::cout << "Введены неверные параметры." << std::endl;
		std::cout << "Используйте: <Hostname_Center_Server> <port1> <port2> <port3> <port_cs> <port_sc> <your_user_name> <use_I2P: [TRUE/FALSE]> <SAM_port> <use_static_tunnels: [TRUE/FALSE]> <my_port_hash_blockain> <my_port_send_message>" << std::endl;
		exit(1);
	}	

write_to_log_file (LOG_FILE_NAME, "Начало программы\n");	

// Инициализация параметров
Hostname_Center_Server = argv[1];
	port1 = atoi(argv[2]);
	port2 = atoi(argv[3]);
	port3 = atoi(argv[4]);
	port_cs = atoi(argv[5]);
	port_sc = atoi(argv[6]);
	std::string MY_NAME_this_str_buf (argv[7]);
	MY_NAME = MY_NAME_this_str_buf;
	std::string use_i2p (argv[8]);
	SAMPort = atoi(argv[9]);
	//strcpy(use_static_tunnels, argv[10]);
	//strcpy(my_port_hash_blockain, argv[11]);
	//strcpy(my_port_send_message, argv[12]);
	use_static_tunnels = argv[10];
	my_port_hash_blockain = argv[11];
	my_port_send_message = argv[12];

	if ((use_i2p != "FALSE") && (use_i2p != "TRUE"))
	{
		printf("<use_i2p> have wrong value = %s. It my be '%s' or '%s'.\n", use_i2p, "FALSE", "TRUE");
		exit(1);
	}

	if ((strcmp(use_static_tunnels,"FALSE") != 0) && (strcmp(use_static_tunnels,"TRUE") != 0) )
	{
		printf("<use_static_tunnels> have wrong value = %s. It my be '%s' or '%s'.\n", use_static_tunnels, "FALSE", "TRUE");
		exit(1);
	}


	//printf("%s\n","Hello!");
	//sleep(10);

	{
	// Считываем два адреса в соответствии с именем вводимым
	std::string path = "Keys/" + MY_NAME + "/PUBLIC_hash_and_blockchain.txt";
   	const char* desinatin_key_file = path.c_str();
	char buffer[MAX_DESTINATION_LENGTH];
    memset(buffer, 0, MAX_DESTINATION_LENGTH);
    int size = read_from_file_to_buffer_1(desinatin_key_file, buffer, MAX_DESTINATION_LENGTH);
    buffer[size-1] = '\0';
    std::string mydest_buf(buffer);
    my_public_dest_hash_and_blockchain = mydest_buf;
    	
	}

	//std::cout << "my_public_dest_hash_and_blockchain = ||" << my_public_dest_hash_and_blockchain << "||" << std::flush;
    //std::cout << "" << std::endl;

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
    
	}

	//std::cout << "my_public_send_message = ||" << my_public_send_message << "||" << std::flush;
    //std::cout << "" << std::endl;	

	//sleep(10);


	int height_square = 50;
	int width_square = 150;
	set_color(36);
	square(height_square - 13, width_square - 15);
	
	start_line = 4;
	set_cursor_position (start_line, width_square/4);
	set_color(33);
	printf("BLOCKCHAIN_CRYPTO_MESSANGER");
	start_line++;
	set_cursor_position (start_line, width_square/4 - 3);
	start_line++;
	set_color(31);
	printf("***********************************");
	fflush (stdout);
	
	//printf("BLOCKCHAIN_CRYPTO_MESSANGER");


// 1
// --------------------- Попытка открытия или создания БД --- BEGIN ----------------------
    
    write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 1]\n");
    
    std::string file_name_to_DB;
    set_cursor_position (start_line + 1, 4);
    start_line++;
    set_color(36);
    std::cout << "Введите полный путь к зашифрованной БД (без окончания \".enc\"): ";
    set_color(34);
    std::getline(std::cin,file_name_to_DB);

    fflush (stdout);
    // Проверка существования и открытия файла БД
    
    //Добавление к имени БД окончания ".enc"
    std::string file_name_to_DB_enc = file_name_to_DB + ".enc";



    




    
    //std::cout << "Проверка существования зашифрованного файла БД ..." <<std::endl;    
    int file_exist = checkFile(file_name_to_DB_enc.c_str());

    std::string password;

	// Если зашифрованной БД нет
    if (file_exist == -1)
    {
    	//start_line++;
		//set_color(36);
    	//std::cout << "\tТакой зашифрованной БД еще нет";
    	fflush (stdout);
    	std::string file_name_to_DB_not_enc = file_name_to_DB;
    	int file_exist_2 = checkFile(file_name_to_DB_not_enc.c_str());

    	// Если нет и старого файла открытой БД (который остается при неправильном или аварийном выходе из программы)
    	if (file_exist_2 == -1)
    	{
    		//start_line++;
			//set_color(36);
    		//std::cout << "\tТакой открытой БД еще нет" << std::endl;
    	fflush (stdout);
    		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(36);
    		std::cout << "\tПридумайте пароль к вашей БД.";
    		fflush (stdout);

    		//std::getline(std::cin,password);
    		password = input_password();
    	
    		
    	}
    	else
    	{
    		start_line++;
			set_color(36);
    		std::cout << "\tНайдена БД в открытом виде." << std::endl;
    		start_line++;
    		set_color(33);
    		std::cout << "\tВ прошлом сеансе использования был некорректный выход из программы." << std::endl;
    		start_line++;
    		set_color(31);
    		std::cout << "\tЭто прямая угроза безопасности ваших данных !!! " << std::endl;
    		start_line++;
    		set_color(36);
    		std::cout << "\tИзбегайте подобных ситуаций. " << std::endl;
    		start_line++;
    	}
    }



    
    // Если база данных есть
    if (file_exist == 1)
    	{
    		// Делаем backup зашифрованной базы данных (на случай, если из программы будет неправильный выход)
    		// Чтобы вернуться к нормальной работе при сбое (потере зашифрованной БД) необходимо удалить из названия ".backup"
    		std::string programm_name_str = "cp " + file_name_to_DB_enc + " " + file_name_to_DB_enc + ".backup"; // Скорее всего это depricated уже, но на всякий случай оставлю
    		char* ptr; // для совместимости
		call_Linux_programm(programm_name_str.c_str(),ptr);
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(36);
		std::cout << "Резервное копирование зашифрованной БД произведено успешно в файл: ";
		set_color(33);
		std::cout << file_name_to_DB_enc << ".backup";
		fflush (stdout);
    		// Если он существует, то необходимо расшифровать его
    		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(36);
    		std::cout << "\tВведите пароль от базы данных.";
    		fflush (stdout);
    		
    		//std::getline(std::cin,password);
    		
    		// Функция получения пароля (для защиты от keylogger'a)
    		password = input_password();
    		
    		// Вызываем функцию расшифрования
    		decrypt_file (file_name_to_DB_enc.c_str(), file_name_to_DB.c_str(), password.c_str());
    		// Удаляем зашифрованный файл БД
    		std::string programm_name_str_1 = "rm " + file_name_to_DB_enc;
		call_Linux_programm(programm_name_str_1.c_str(),ptr);

    	}

    	
    	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(36);
    	std::cout << "Вы ввели пароль: |";
    	set_color(33);
    	std::cout << password ;
    	set_color(36);
    	std::cout << "|";
    	fflush (stdout);
	sleep(3);
    
    set_cursor_position (start_line + 1, 4);
    start_line++;	    
    std::cout << "Проверка существования и открытия файла БД ...";
    fflush (stdout);
    sleep(1);
    int check_result = Create_or_Enter_to_DB(file_name_to_DB.c_str());
    
    PATH_Date_Base = file_name_to_DB.c_str();
    
    if (check_result != 0)
        {
            set_cursor_position (start_line + 1, 4);
    	     start_line++;
    	     set_color(31);
            std::cout << "При открытии/создании БД произошла ошибка.";
            fflush (stdout);
            exit(1);
        }
    else if (check_result == 0)
        {
            set_cursor_position (start_line + 1, 4);
    	    start_line++;
    	    set_color(32);
            std::cout << "БД успешно открыта/создана.";
            fflush (stdout);
        }

// --------------------- Попытка открытия или создания БД --- END ----------------------












// 2

// --------------------- Инициализация всех серверов и клиентов --- BEGIN ----------------------
	write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 2]\n");

	// ================================================================================================================================================
	// ================================================================================================================================================
    
 	// --- Открытие отдельного потока для периодической отправки сообщений на ЦС и получение акутального списка доступных IP-адресов в сети --- BEGIN ---
 	
 		pthread_t thread1;
		//printf("Host_name = %s\n", Hostname_Center_Server);
		//printf("Port = %d\n", port1);	
		struct IP_and_PORT struct_ip_port;
		struct_ip_port.ip = Hostname_Center_Server;
		struct_ip_port.port = port1;	

		// Вызываем функцию func_say_hello_and_get_user_list_function в отдельном потоке
		if (use_i2p == "FALSE")
		{
			/*
		int result0 = pthread_create(&thread1, NULL, func_say_hello_and_get_user_list_function, &struct_ip_port);
   			if (result0 != 0) 
   				{
     					perror("Creating the func_say_hello_and_get_user_list_function thread");
     					return EXIT_FAILURE;
   				}
   				*/
 		}
 		else
 		{
 			// ...
 		}
 		//printf("%s","Ожидание закрытия потока func_say_hello_and_get_user_list_function...\n");
 		
 	// --- Открытие отдельного потока для периодической отправки сообщений на ЦС и получение акутального списка доступных IP-адресов в сети --- END ---
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================
 	
 	// Задержка (чтобы успел прийти список доступных IP-адресов сети от ЦС - "user_list_client.txt")
 	sleep(5);
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================

 











 	
 	 // --- Открытие отдельного потока для отправки hash-ей последних транзакций и локальной копии блокчейна --- BEGIN ---
 	pthread_t thread2; // for clearnet
 	pthread_t threads_hash_blockchain[MAX_count_of_users]; // for i2p
 	if (use_i2p == "FALSE")
 	{
 		struct port_and_path_to_DB struct0;
 		struct0.port = port2;
 		struct0.path_to_db = file_name_to_DB.c_str();
		// Вызываем функцию hash_and_blockchain_server_part_function в отдельном потоке
		int result1 = pthread_create(&thread2, NULL, hash_and_blockchain_server_part_function, &struct0);
   		if (result1 != 0) 
   			{
     				perror("Creating the hash_and_blockchain_server_part_function thread");
     				return EXIT_FAILURE;
   			}
   			sleep(5);
	}
	



	/*

		std::string path = "Keys/" + MY_NAME + "/PRIVATE_hash_and_blockchain.txt";
		const char* desinatin_key_file = path.c_str();
		

		char buffer[MAX_DESTINATION_LENGTH];
    	memset(buffer, 0, MAX_DESTINATION_LENGTH);
    	int size = read_from_file_to_buffer_1(desinatin_key_file, buffer, MAX_DESTINATION_LENGTH);
    	buffer[size-1] = '\0';
    	std::string mydest_buf_(buffer);
    	std::string mydest_ = mydest_buf_;


    	std::cout << "mydest_hash_and_blockchain = ||" << mydest_ << "||" << std::flush;
    	std::cout << "" << std::endl;


    	printf("create session hash_and_blockchain ...\n");
    	SAM::StreamSession session_1 ("session_server_1", "127.0.0.1", SAMPort, mydest_);
    	std::shared_ptr<SAM::StreamSession> session_ptr_1(&session_1);
    	//session_1.stopForwardingAll();

    	printf("create session hash_and_blockchain client_1...\n");
    	SAM::StreamSession session_client_1("session_client_1", "127.0.0.1", SAMPort);
    	std::shared_ptr<SAM::StreamSession> session_client_1_ptr_1(&session_client_1);
    	session_ptr_hash_blockchain = session_client_1_ptr_1;
    	//session_client_1.stopForwardingAll();


    	int i = 0;
    	for (i; i < MAX_count_of_users; i++)
		{
 			struct i2p_session_str struct0;
 			struct0.session_ptr = session_ptr_1;
			struct0.number = i;
			int result1 = pthread_create(&threads_hash_blockchain[i], NULL, hash_and_blockchain_server_part_function_i2pd, &struct0);
   			if (result1 != 0) 
   				{
     				perror("Creating the i2p_socket_thread threads_hash_blockchain failed.\n");
					printf("Creating the i2p_socket_thread threads_hash_blockchain[%d] failed.\n", i);
     				return EXIT_FAILURE;
   				}

			printf("threads_hash_blockchain[%d] created successfull.\n", i);
			sleep(1);
		}

	*/
	
	
		//printf("%s","Ожидание закрытия потока hash_and_blockchain_server_part_function...\n");
		
	// --- Открытие отдельного потока для отправки hash-ей последних транзакций и локальной копии блокчейна --- END ---	
 	 	
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================
 	
 	// --- Открытие отдельного потока для участия в алгоритме консенсуса (обработке запросов send_message) и формирования локальной копии блокчейна --- BEGIN ---
 	
 	
		pthread_t thread3; // for clearnet
		pthread_t threads_send_message[MAX_count_of_users]; // for i2p

		if (use_i2p == "FALSE")
 		{
		struct path_to_DB_and_port structure;
		structure.path_to_db = file_name_to_DB.c_str();
		structure.port = port3;
		//printf("structure.port = %d\n", structure.port);
		//sleep(5);
		int result = pthread_create(&thread3, NULL, func_send_message_server_part_function, &structure);
   		if (result != 0) 
   			{
     				perror("Creating the func_send_message_server_part_function thread");
     				return EXIT_FAILURE;
   			}
   			sleep(5);
   		}
   		

   		/*
   			std::string path_ = "Keys/" + MY_NAME + "/PRIVATE_send_message.txt";
   			const char* desinatin_key_file_ = path_.c_str();
		

			char buffer_[MAX_DESTINATION_LENGTH];
    		memset(buffer_, 0, MAX_DESTINATION_LENGTH);
    		int size_ = read_from_file_to_buffer_1(desinatin_key_file_, buffer_, MAX_DESTINATION_LENGTH);
    		buffer_[size_-1] = '\0';
    		std::string mydest_buf(buffer_);
    		std::string mydest = mydest_buf;



    		std::cout << "mydest_send_message = ||" << mydest << "||" << std::flush;
    		std::cout << "" << std::endl;


    		printf("create session send_message ...\n");
    		SAM::StreamSession session_2 ("session_server_2", "127.0.0.1", SAMPort, mydest);
    		std::shared_ptr<SAM::StreamSession> session_ptr_2(&session_2);
    		//session_2.stopForwardingAll();

    		printf("create session send_message client_2...\n");
    		SAM::StreamSession session_client_2("session_client_2", "127.0.0.1", SAMPort);
    		std::shared_ptr<SAM::StreamSession> session_client_2_ptr_2(&session_client_2);
    		session_ptr_send_message = session_client_2_ptr_2;
    		//session_client_2.stopForwardingAll();   	

    		int i_ = 0;
    		for (i_; i_ < MAX_count_of_users; i_++)
			{
 				struct i2p_session_str struct0;
 				struct0.session_ptr = session_ptr_2;
				struct0.number = i_;
				int result1 = pthread_create(&threads_send_message[i_], NULL, send_message_server_part_function_i2pd, &struct0);
   				if (result1 != 0) 
   				{
     				perror("Creating the i2p_socket_thread threads_hash_blockchain failed.\n");
					printf("Creating the i2p_socket_thread threads_hash_blockchain[%d] failed.\n", i_);
     				return EXIT_FAILURE;
   				}

				printf("threads_hash_blockchain[%d] created successfull.\n", i_);
				sleep(1);
			}
   		

		*/









   		//printf("%s","Ожидание закрытия потока func_send_message_server_part_function...\n");	
 	  
 	// --- Открытие отдельного потока для участия в алгоритме консенсуса (обработке запросов send_message) и формирования локальной копии блокчейна --- END --- 
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================    

// --------------------- Инициализация всех серверов и клиентов --- END ----------------------





// -------------------------- (03.08.21)------------------------------------------------
// --- Открытие отдельного потока для периодического обновления локального блокчейна --- BEGIN ---
 		pthread_t thread4;
 		if (use_i2p == "FALSE")
 		{
 		struct port_and_path_to_DB struct04;
 		struct04.port = port2;
 		struct04.path_to_db = file_name_to_DB.c_str();
		// Вызываем функцию hash_and_blockchain_server_part_function в отдельном потоке
		/*
		int result14 = pthread_create(&thread4, NULL, periodically_get_actually_blockchain_copy_function, &struct04);
   		if (result14 != 0) 
   			{
     				perror("Creating the periodically_get_actually_blockchain_copy_function thread");
     				return EXIT_FAILURE;
   			}
   		*/
		}
		else
		{
			// ...
		}
	
	
		//printf("%s","Ожидание закрытия потока periodically_get_actually_blockchain_copy_function...\n");
		
// --- Открытие отдельного потока для периодического обновления локального блокчейн --- END ---
// -------------------------- (03.08.21)------------------------------------------------





	//if (use_i2p != "FALSE")
		//sleep(300); // Чтобы все LeaseSet'ы могли опубликоваться для созданных через SAM сессий!









// 3

// --------------------- Получение акутальной копии блокчейна в БД --- BEGIN ----------------------

	write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 3]\n");

	// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети
	if (use_i2p == "FALSE")
		get_actual_blockchain_copy(port2);
	else
    {
        get_actual_blockchain_copy_i2pd();
    }
		
    
	
	// Очищение старой копии BLOCKCHAIN_TABLE
	Clear_Blockchain_Table(file_name_to_DB.c_str());
	
	
	
	/*
	int default_id = 1;
        std::string encrypt_message_default = "1;1;1;0;encrypt_message;";
        std::string default_hesh_before = "0123456789";
                
                // Хэш будем считать вот так
                //std::string default_this_hash = get_hash (encrypt_message_default);
                // Но пока не можем поэтому пока что напишем вот так
        std::string default_this_hash = "9876543210";

        int default_blockchain_add = add_record_to_local_blockchain(file_name_to_DB.c_str(), default_id, encrypt_message_default.c_str(), default_hesh_before.c_str(), default_this_hash.c_str());
	*/
	
	
	// Заполнение БД из файла
	const char* path_to_buffer_file = "actual_blockchain.txt";
	blockchain_from_file_to_DB (file_name_to_DB.c_str(), path_to_buffer_file);
	
	
	std::string last_available_id;
	get_last_available_id_in_blockchain(file_name_to_DB.c_str(), &last_available_id);
	int lai = atoi(last_available_id.c_str());
	
	// Запросить количество транзакций в новом блокчейне
	char last_id_c[BUF_SIZE];
     	memset(last_id_c, 0, BUF_SIZE);
	get_how_much_id (file_name_to_DB.c_str(), last_id_c);
	int lis = atoi(const_cast<const char*>(last_id_c));
	
	//std::cout << "lai = " << lai << std::endl;
	//std::cout << "lis = " << lis << std::endl;
	
	// Проверка на то, нужно ли обнулять ключи всем пользователям (это если за время отстутствия произошло обнуление глобального блокчейна)
	if (lai > lis)
		{
			//std::cout << "lai > lis" << std::endl;
			// Обнуление ключей всех пользователей
			set_last_analysed_id_in_blockchain(file_name_to_DB.c_str(), 1);
			set_last_available_id_in_blockchain(file_name_to_DB.c_str(), 1);
			key_seek_zero_for_all_users();
		}
	
	
	
	

// --------------------- Получение акутальной копии блокчейна в БД --- END ----------------------










// 4

// --------------------- Анализ блокчейна на предмет входящий сообщений --- BEGIN ----------------------
	write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 4]\n");
	analyse_blockchain_for_input_messages(file_name_to_DB.c_str());

// --------------------- Анализ блокчейна на предмет входящий сообщений --- END ----------------------




	//std::cout << "\n\t ----- Инициализация БД и всех серверных потоков завершена успешно. ---- \n" << std::endl;




// Мини-проверочка создания директории для файлов, которые необходимо отправить

			std::string dir_name = "OUTPUT_FILES";

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
    					char* res_buf; // Просто для совместимости 
    					call_Linux_programm(linux_command.c_str(), res_buf);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					set_cursor_position (start_line + 1, 4);
    	     				start_line++;
    	     				std::cout << "Невозможно получить доступ к директории OUTPUT_FILES";
    					exit(1);
				}


// Мини-проверочка создания директории для файлов, которые необходимо отправить на хранение в распределенное хранилище

			std::string dir_name1 = "BIG_FILES_OUTPUT";

			DIR* dir1 = opendir(dir_name1.c_str());
			if (dir1)
				{
    					/* Directory exists. */
    					//std::cout << "Директория для файлов существует." << std::endl;
    					closedir(dir1);
				}
			else if (ENOENT == errno)
				{
					//std::cout << "Директория для файлов не существует." << std::endl;
    					/* Directory does not exist. */
    					
    					std::string linux_command1 = "mkdir " + dir_name1;
    					char* res_buf1; // Просто для совместимости 
    					call_Linux_programm(linux_command1.c_str(), res_buf1);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					set_cursor_position (start_line + 1, 4);
    	     				start_line++;
    	     				std::cout << "Невозможно получить доступ к директории OUTPUT_FILES";
    					exit(1);
				}

// Мини-проверочка создания директории для файлов, которые необходимо запросить из распределенного хранилища

			std::string dir_name2 = "BIG_FILES_INPUT";

			DIR* dir2 = opendir(dir_name2.c_str());
			if (dir2)
				{
    					/* Directory exists. */
    					//std::cout << "Директория для файлов существует." << std::endl;
    					closedir(dir2);
				}
			else if (ENOENT == errno)
				{
					//std::cout << "Директория для файлов не существует." << std::endl;
    					/* Directory does not exist. */
    					
    					std::string linux_command2 = "mkdir " + dir_name2;
    					char* res_buf2; // Просто для совместимости 
    					call_Linux_programm(linux_command2.c_str(), res_buf2);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					set_cursor_position (start_line + 1, 4);
    	     				start_line++;
    	     				std::cout << "Невозможно получить доступ к директории OUTPUT_FILES";
    					exit(1);
				}


// Мини-проверочка создания директории для файлов, которые необходимо запросить из распределенного хранилища

			std::string dir_name3 = "TEMP_DIR";

			DIR* dir3 = opendir(dir_name3.c_str());
			if (dir3)
				{
    					/* Directory exists. */
    					//std::cout << "Директория для файлов существует." << std::endl;
    					closedir(dir3);
				}
			else if (ENOENT == errno)
				{
					//std::cout << "Директория для файлов не существует." << std::endl;
    					/* Directory does not exist. */
    					
    					std::string linux_command3 = "mkdir " + dir_name3;
    					char* res_buf3; // Просто для совместимости 
    					call_Linux_programm(linux_command3.c_str(), res_buf3);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					set_cursor_position (start_line + 1, 4);
    	     				start_line++;
    	     				std::cout << "Невозможно получить доступ к директории OUTPUT_FILES";
    					exit(1);
				}



			// Проверка на существование директории и ее создание если ее нет
			std::string dir_name4 = "INPUT_FILES";
			DIR* dir4 = opendir(dir_name4.c_str());
			if (dir4)
				{
    					/* Directory exists. */
    					//std::cout << "Директория для файлов существует." << std::endl;
    					closedir(dir4);
				}
			else if (ENOENT == errno)
				{
					//std::cout << "Директория для файлов не существует." << std::endl;
    					/* Directory does not exist. */
    					std::string linux_command4 = "mkdir " + dir_name4;
    					char res_buf4[100]; // Просто для совместимости 
    					call_Linux_programm(linux_command4.c_str(), res_buf4);
    					//std::cout << "А теперь существует." << std::endl;
				}
			else
				{
    					/* opendir() failed for some other reason. */
    					printf("%s\n","Ошибка при проверке существования директории");
    					exit(1);
				}




	int height_animation = height_square/6;
	int width_animation = width_square/6;
	int animaction_delay_us = 100000;


set_cursor_position (height_animation + 7, width_square/2 - 6);
start_line++;
set_color(33);
printf("Инициализация сетевых сервисных потоков ...");
usleep(100000);




	
	// Запуск анимации
	int number_repeat_animation = 2;
	int e = 0;
	for (e;e<number_repeat_animation;e++)
		{
			loading_animation (0, height_animation + 10, width_animation + 55, height_animation, width_animation, animaction_delay_us);
		}
	
	// Остановка анимации
	loading_animation (1, height_animation + 10, width_animation + 55, height_animation, width_animation, animaction_delay_us);
	
	set_cursor_position (4 + height_animation + 5 + 10 + 1, 4 + width_square/2 - 3);
	set_color(37);
	printf("%s","Приятного пользования!");
	fflush (stdout);

	usleep(2000000);


int control_flag = 0; // 0 - concole, 1 - file

write_to_log_file (LOG_FILE_NAME, "Meessanger [первый запуск while(1)]\n");

while(1) // Начало while(1)
{







	//char* ptr; // для совместимости
	//call_Linux_programm("clear",ptr);
	
	int cl = 0;
	int cl_1 = 0;
	
	for(cl; cl<height_square + 5 ; cl++)
		{
			for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");
					
					
				}
			cl_1 = 0;	
		}
	
	fflush (stdout);
	set_cursor_position (2, 0);
	fflush (stdout);
	
	// Отображение главного меню
	
	set_color(36);
	square(height_square - 13, width_square - 15);
	
	start_line = 3;
	set_cursor_position (start_line, width_square/4 + 2);
	set_color(36);
	printf("%s","Ваше имя: \"");
	set_color(34);
	printf("%s",MY_NAME.c_str());
	set_color(36);
	printf("%s","\"");
	
	
	start_line = 4;
	set_cursor_position (start_line+1, width_square/4);
	set_color(33);

	printf("Список пользователей");
	start_line++;
	set_cursor_position (start_line + 1, width_square/4 - 3);
	start_line++;
	set_color(31);
	printf("************************");
	fflush (stdout);
	
	

// 6

// --------------------- Вывод списка моих пользователей с количеством непрочитанных сообщений --- BEGIN ----------------------

	write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 6]\n");


    	// Выводим список добавленных пользователей

    	user_list_and_pointer struct1;

    	int res0 = return_my_user_list (file_name_to_DB.c_str(), &struct1);

    	if (res0 == -1)
        	{
            	std::cout << "При выводе списка добавленных пользователей произошла ошибка." << std::endl; 
            	exit(1);       
        	}
    	else if (res0 == 0)
        	{
        
        		// Вывод значений struct1 ...
   
   			//std::cout << "\n ВЫВОД ЗНАЧЕНИЙ struct1.user_list ... \n" << std::endl;
   			
   			set_cursor_position (start_line + 2, 4);
   			start_line++;
   			start_line++;

			// Запрос количества транзакций
			char last_id_c_transaction[BUF_SIZE];
			memset(last_id_c_transaction, 0, BUF_SIZE);
    			get_how_much_id (PATH_Date_Base, last_id_c_transaction);
	
			int last_id_transaction = atoi((const char*) last_id_c_transaction);

			set_color(36);
			std::cout << "Количество транзакций в текущем глобальном блокчейне: " ;
			set_color(32);
			if (last_id_transaction > (int)(limit_number_of_transaction/2))
			set_color(34);			
			if (last_id_transaction > (limit_number_of_transaction - (int)(limit_number_of_transaction/3)))
			set_color(31);
			
			std::cout << last_id_transaction;


			set_cursor_position (start_line + 1, 4);
   			start_line++;
   			
   			int j = 0;
   			for (j;j<struct1.pointer;j++)
   				{
   					// Запрашиваем количество непрочитанных сообщений с пользователем struct1.user_list[j]
                    			int new_messages_now = get_count_new_message(file_name_to_DB.c_str(),struct1.user_list[j]);
                    			int user_key_size = get_key_size_for_user(PATH_Date_Base, struct1.user_list[j].c_str());
                    			int current_seek = get_key_seek_for_user(PATH_Date_Base, struct1.user_list[j].c_str());
         				set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(37);
					std::cout << j + 1 << ". " ;
					set_color(36);
					std::cout << struct1.user_list[j] << " (";
					set_color(33);
					std::cout << new_messages_now;
					set_color(36);
					std::cout << ") ";
					set_color(37);
					std::cout << "[";

					if (current_seek <= (int)(user_key_size * 0.33))
						set_color(33);
					if ((current_seek > (int)(user_key_size * 0.33)) && (current_seek <= (int)(user_key_size * 0.66)))
						set_color(34);
					if (current_seek > (int)(user_key_size * 0.66))
						set_color(31);
					std::cout << current_seek;
					set_color(37);
					std::cout << "/";
					set_color(31);
					std::cout << user_key_size;
					set_color(37);
					std::cout << "]";
					if (current_seek > (int)(user_key_size * 0.66))
					{
						set_color(31);
						std::cout << "	ВНИМАНИЕ !!!";
					}
						
					
   				}  
        		set_color(37);
        		set_cursor_position (start_line + 1, 4);
			start_line++;
            		std::cout << "====================================================================="; 
        	}

// --------------------- Вывод списка моих пользователей с количеством непрочитанных сообщений --- END ----------------------









// 7

// --------------------- Выбор дальнейших действий и их исполнение --- BEGIN ----------------------
	write_to_log_file (LOG_FILE_NAME, "Meessanger [пункт 7]\n");

	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(34);
	std::cout << "\tВыберите одну из команд из нижепереисленного списка: ";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	set_color(37);
	std::cout << "\t\tДобавить нового пользователя - 1;";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tПосмотреть переписку с конкретным пользователем и отправить ему сообщение - 2;";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tПосмотреть переписку с конкретным пользователем и отправить ему файл - 3;";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tОбновить ключ для конкретного пользователя - 4;";
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tОтправить файл в распределенное хранилище - 5;" << std::endl;
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tЗапросить файл из распределенного хранилища - 6;" << std::endl;
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tСменить управение (Файл <---> Консоль) - 7;" << std::endl;
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tОбновить локальную копию блокчейна - 8;" << std::endl;
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tОбновить окно - 9;" << std::endl;
	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "\t\tВыйти из программы - 10;" << std::endl;

	std::string answer;

	set_cursor_position (start_line + 1, 4);
	start_line++;
	std::cout << "---------------------------------------------------------------------";


	set_cursor_position (start_line + 2, 4);
	start_line++;
	start_line++;
	set_color(36);
	std::cout << "Команда для выполнения: ";
	set_color(32);
	fflush (stdout);


//--------------------(04.08.21)--------------------------------

if (control_flag == 1)
	{
	





	int ret_ = getline_with_block_from_file(answer);
	if (ret_ == 1)
	{
		std::cout << "При чтении из файла произошла ошибка." << std::endl;
		fflush (stdout);
		continue;
	}
	while (answer == "")
	{
		ret_ = getline_with_block_from_file(answer);
		if (ret_ == 1)
		{
		std::cout << "При чтении из файла произошла ошибка." << std::endl;
		fflush (stdout);
		}
			
	}


	set_color(32);
	std::cout << answer;
	fflush (stdout);
//	std::cout << "" << std::endl;

	if ((answer != "1") and (answer != "2") and (answer != "3") and (answer != "4") and (answer != "5") and (answer != "6") and (answer != "7") and (answer != "8") and (answer != "9") and (answer != "10"))
    	{

    		//set_cursor_position (start_line + 1, 4);
		//start_line++;
        	//std::cout << "Введено некорректное значение функции. Работа программы будет прекращена с ошибкой.";
//       	exit(1);
		set_cursor_position (start_line + 2, 4);
		set_color(31);
		std::cout << "Введено некорректное значение функции." << std::endl;
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(32);
		std::cout << "Проверьте введенные данные." << std::endl;
		fflush (stdout);
		sleep(2);
		continue;


/*
//-----------------------------------
write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 8]\n");
            	 // Зашифровываем файл БД перед уходом (если выйти из программы НЕ таким способом, то все данные, полученные после последнего запуска БД будут утеряны!)
            	 // Поэтому, обязательно выходить правильно из программы
            	 std::string path_DB_str (PATH_Date_Base);
            	 std::string path_enc_DB_str  = path_DB_str + ".enc";
            	 encrypt_file (PATH_Date_Base, path_enc_DB_str.c_str(), password.c_str());   
            	 
            	 // Удаляем открытый файл БД (чтобы не висел там нигде)
            	 std::string string_programm  = "rm " + path_DB_str;
            	 char* ptr; // для совместимости
		 call_Linux_programm(string_programm.c_str(),ptr);
		 
		 // Удаляем файл действительных IP-адресов (потому, что когда мы зайдем в след.раз, то он уже будет другой)
            	 std::string string_programm1  = "rm user_list_client.txt";
            	 char* ptr1; // для совместимости
		 call_Linux_programm(string_programm1.c_str(),ptr1);
            	 
            	 
            	 
            	 int cl = 0;
		int cl_1 = 0;
	
		for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
					{
						set_cursor_position (cl, cl_1);
						printf("%s"," ");
					
					
					}
				cl_1 = 0;	
			}
	
		fflush (stdout);
		set_cursor_position (0, 0);
		fflush (stdout);
         	 set_color(32);
                std::cout << "Работа программы будет завершена с ошибкой, Возможна потеря данных и рассинхронизация ключей шифрования !!!. " << std::endl;
                return 0; 
//-----------------------------------
*/

 


    	}	
	else 
	{
    		if (answer == "1")
        		{
        			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 1]\n");
                    		// ----------------------------------------------------------------------------

    				// Добавление нового пользователя
				set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "Введите имя нового пользователя для добавления в ваши контакты: ";
    				std::string new_user_name;
    				set_color(32);
				fflush (stdout);
				


				ret_ = getline_with_block_from_file(new_user_name);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
				while (new_user_name == "")
				{
					ret_ = getline_with_block_from_file(new_user_name);
					if (ret_ == 1)
					{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
					}	
				}
				set_color(32);
				std::cout << new_user_name;
				fflush (stdout);
    			set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(36);
    			std::cout << "Введите путь к файлу с ключом для нового пользователя: ";
    			fflush (stdout);
    			//set_cursor_position (start_line + 1, 4);
				//start_line++;
    			std::string path_to_key_file;
    			set_color(32);
				fflush (stdout);


    				ret_ = getline_with_block_from_file(path_to_key_file);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
				while (path_to_key_file == "")
				{
					ret_ = getline_with_block_from_file(path_to_key_file);
						if (ret_ == 1)
						{
							std::cout << "При чтении из файла произошла ошибка." << std::endl;
							fflush (stdout);
						}	
				}


				set_color(32);
				std::cout << path_to_key_file;
				fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    				std::cout << "Добавление нового пользователя...";
				fflush (stdout);
				set_cursor_position (start_line + 1, 4);
					start_line++;
    				int add_res = create_table_for_new_user (file_name_to_DB.c_str(), new_user_name, path_to_key_file);

    				if (add_res == -1)
        				{
						set_cursor_position (start_line + 1, 4);
						start_line++;
						set_color(31);
            					std::cout << "При добавлении нового пользователя произошла ошибка." << std::endl;
						fflush (stdout);
						sleep(2);
            					//exit(1);
        				}
    				else if (add_res == 0)
        				{
        					//set_cursor_position (start_line + 1, 4);
						//start_line++;
						set_color(32);
            					std::cout << "Добавление нового пользователя успешно завершено." << std::endl;
            					fflush (stdout);
            					sleep(2);
        				}

				// ----------------------------------------------------------------------------
        		}




        if (answer == "2")
            {
            
            	write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 2]\n");
            	user_messages_list_and_pointer struct2;
            
            	set_cursor_position (start_line + 2, 4);
		start_line++;
		start_line++;
		set_color(36);
                std::cout << "Введите имя пользователя, с которым хотите прочитать историю переписки: ";
                std::string user_name;
                set_color(32);
		fflush (stdout);
                ret_ = getline_with_block_from_file(user_name);
		if (ret_ == 1)
			{
				std::cout << "При чтении из файла произошла ошибка." << std::endl;
				fflush (stdout);
			}
		while (user_name == "")
		{
			ret_ = getline_with_block_from_file(user_name);
			if (ret_ == 1)
			{
				std::cout << "При чтении из файла произошла ошибка." << std::endl;
				fflush (stdout);
			}	
		}
                

		set_color(32);
		std::cout << user_name;
		fflush (stdout);
               // char* ptr; // для совместимости
		//call_Linux_programm("clear",ptr);
                
	int cl = 0;
	int cl_1 = 0;
	
	for(cl; cl<height_square + 5 ; cl++)
		{
			for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");
					
					
				}
			cl_1 = 0;	
		}
	
	fflush (stdout);
	set_cursor_position (2, 0);
	fflush (stdout);
                set_color(34);
                std::cout << "\n\t\tИстория переписки с пользователем: ";
                set_color(32);
                std::cout << user_name << std::endl;
		fflush (stdout);
                int res1 = return_my_messages_with_user (file_name_to_DB.c_str(), user_name, &struct2);

		//printf("%s\n","olo");

                if (res1 == -1)
                    {

// --------------------------------------------------------------------------
			int cl = 0;
			int cl_1 = 0;
	
			for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");	
				}
			cl_1 = 0;	
			}
	
			fflush (stdout);
			set_cursor_position (2, 0);
			fflush (stdout);

			set_color(36);
			square(height_square - 13, width_square - 15);
			start_line = 2;

			set_cursor_position (start_line + 2, 4);
			start_line++;

// --------------------------------------------------------------------------
			set_color(31);
                        std::cout << "При загрузке истории переписки с данным пользователем произошла ошибка." << std::endl; 
			set_cursor_position (start_line + 2, 4);
			start_line++;
			set_color(32);
			std::cout << "Проверте правильность введенных данных. " << std::endl;                
			fflush(stdout);
			sleep(2);
			
                         //exit(1);   
                    }
                else if (res1 == 0)
                    {
                    
                    	//printf("%s\n","olo");
                    
                    	// Запрашиваем количество непрочитанных сообщений
                    	int new_messages_now = get_count_new_message(file_name_to_DB.c_str(),user_name);

			// Обнуляем количество непрочитанных сообщений
			
			update_new_message_count_to_user(file_name_to_DB.c_str(), user_name, 0);
                    	set_color(36);
                    	std::cout << "\n\tС пользователем ";
                    	set_color(32);
                    	std::cout << user_name;
                    	set_color(36);
                    	std::cout << " у вас ";
                    	set_color(37);
                    	std::cout << new_messages_now;
                    	set_color(36);
                    	std::cout << " непрочитанных сообщений.\n" << std::endl;
                    
                    	// Вывод значений struct2 ...
   
   			//std::cout << "\n ВЫВОД ЗНАЧЕНИЙ struct2.list_messages ... \n" << std::endl;
   			int j = 0;
   			for (j;j<struct2.pointer;j++)
   				{
   				
   					// Красиво отображаем переписку
   					if (strcmp(struct2.list_hwo_sender[j].c_str(),MY_NAME.c_str()) != 0)
   						{
   							set_color(35);
   							std::cout << "[";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "] <";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << ">: \"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\"" << std::endl;
   						}
   					else
   						{
   							set_color(35);
   							std::cout << "\t\t\t\t\t\t\t\t\"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\" :<";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << "> [";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "]" << std::endl;
   						}	
   				} 
                    
                    	set_color(35);
                         std::cout << "-------------------------------------------------------------\n " << std::endl;     
                         set_color(33);
                         std::cout << "Для выхода в главное меню введите ";
                         set_color(37);
                         std::cout << "\"";
                         set_color(31);
                         std::cout << "ESC";
                         set_color(37);
                         std::cout << "\"\n" << std::endl;
                         // Вот тут можно отправить сообщение этому пользователю ...
                         set_color(36);
                         std::cout << "Введите сообщение данному пользователю: ";
                         std::string message;
                         set_color(33);
			 fflush (stdout);
                         ret_ = getline_with_block_from_file(message);
			if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
			while (message == "")
			{
				ret_ = getline_with_block_from_file(message);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}	
			}
			  set_color(32);
			  std::cout << message;
			  fflush (stdout);



			  // Вот тут проверка на выход
			  if (message == "ESC") // Press "Esc"
			  {
			  	continue;
			  }



			  std::cout << "" << std::endl;
			  fflush (stdout);
			  // Вот функция отправки сообщения
			  
			
			
			  // Функция получения номера последнего id
     				char last_id[BUF_SIZE];
     				memset(last_id, 0, BUF_SIZE);
     				get_how_much_id (file_name_to_DB.c_str(), last_id);
     				const char* last_id_str = last_id; 
     				int new_id_number = atoi(last_id);
     				new_id_number++; 
			  
			  
			  // Узнаем текущую дату и время
			  
			  
			     // current date/time based on current system
   			time_t now = time(0);

   			tm *ltm = localtime(&now);



			
			std::string date_time = "Date: " + std::to_string(ltm->tm_mday) +  "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(1900 + ltm->tm_year) + " | Time: " +
										 std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string( ltm->tm_sec);
 		  	
			  
			std::string encrypt_message_str =  std::to_string(new_id_number) + ";" + user_name + ";" + MY_NAME + ";" + date_time + ";" + message + ";";
			const char* text_message = encrypt_message_str.c_str();
			const char* path_to_buf_file = "buf_file.txt";
			
		
			// Зашифровываем сообщение перед отправкой
			int res_enc = 0;
			const char* encrypt_message = encrypt_text_message (user_name.c_str(), text_message, &res_enc);
			if (res_enc == 0)
				continue;
			
			set_color(32);
            std::cout << "Отправка сообщения... ---> ";
            fflush(stdout);
            //set_cursor_position (start_line + 1, 4);
        	//start_line++;

			// Непосредственно отправка сообщения
			int res_send;
			if (use_i2p == "FALSE")
				res_send = send_message (file_name_to_DB.c_str(), encrypt_message, port3, path_to_buf_file);
			else
				res_send = send_message_i2pd (encrypt_message, path_to_buf_file);
			sleep(1);
			if (res_send == 0)
				{
					//printf("%s\n","olo1");
					blockchain_from_file_to_DB (file_name_to_DB.c_str(), path_to_buf_file);
					//printf("%s\n","olo2");
					analyse_blockchain_for_input_messages(file_name_to_DB.c_str());
					//printf("%s\n","olo3");
					
					
					// Отдельно добавляем запись, которая была в открытом виде в пользовательскую таблицу


					add_record_to_user_table(file_name_to_DB.c_str(), user_name, new_id_number, MY_NAME, user_name, date_time, message);					
					
					set_color(32);
                       		std::cout << "Ваше сообщение успешно отправлено." << std::endl;
					fflush (stdout);
                       		sleep(2);  
                        	} 
                       else if (res_send == 1)
                       	{
                       		// seek-ключа нужно назад присвоить какой был
                       		
                       		// Запрос текущего
                       		int cur_seek = get_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str());
                       		              		
                       		// Считаем длину передаваемого сообщения
                       		int len_message = strlen(text_message);
                       		
                       		// Вычитаем длину из текущего смещения
                       		
                       		int different = cur_seek - len_message;
                       		
                       		// Разницу присваеваем
                       		set_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str(), different);	
                       		                       		
                       		//printf("%s\n","Смещение ключа возвращено на прежнее место.");
                       		if (use_i2p == "FALSE")
								get_actual_blockchain_copy(port2);
							else
								get_actual_blockchain_copy_i2pd();
    
        					// Очищение старой копии BLOCKCHAIN_TABLE
        					Clear_Blockchain_Table(PATH_Date_Base);
    
        					// Заполнение БД из файла
        					const char* path_to_buffer_file = "actual_blockchain.txt";
       						blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
        					analyse_blockchain_for_input_messages(PATH_Date_Base);
            				sleep(1);
                       		
                       		std::cout << "Ваше сообщение НЕ отправлено, но Blockchain обновлен. Попытайтесь еще раз." << std::endl;
							fflush (stdout);
                       		sleep(12); 
                       	}
                       
                       else 
                       	{
                       	
                       		std::cout << "При отправке сообщения ПРОИЗОШЛА НЕИЗВЕСТНАЯ ОШИБКА." << std::endl;
				fflush (stdout);
                       		exit(1);
                       	
                       	}	  
                         
                                       
                    }
                
            }









        if (answer == "3")
            {
            write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 3]\n");
            	user_messages_list_and_pointer struct2;

            
            	set_cursor_position (start_line + 2, 4);
		start_line++;
		start_line++;
		set_color(36);
                std::cout << "Введите имя пользователя, с которым хотите прочитать историю переписки: ";
                std::string user_name;
                set_color(32);
		fflush (stdout);
                ret_ = getline_with_block_from_file(user_name);
		if (ret_ == 1)
			{
				std::cout << "При чтении из файла произошла ошибка." << std::endl;
				fflush (stdout);
			}
		while (user_name == "")
		{
			ret_ = getline_with_block_from_file(user_name);
			if (ret_ == 1)
			{
				std::cout << "При чтении из файла произошла ошибка." << std::endl;
				fflush (stdout);
			}	
		}
                
		set_color(32);
		std::cout << user_name;
		fflush (stdout);

               // char* ptr; // для совместимости
		// call_Linux_programm("clear",ptr);


  	int cl = 0;
	int cl_1 = 0;
	
	for(cl; cl<height_square + 5 ; cl++)
		{
			for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");
					
					
				}
			cl_1 = 0;	
		}
	
	fflush (stdout);
	set_cursor_position (2, 0);
	fflush (stdout);


		set_color(34);
                std::cout << "\n\t\tИстория переписки с пользователем: ";
                set_color(32);
                std::cout << user_name << std::endl;
		fflush (stdout);
                int res1 = return_my_messages_with_user (file_name_to_DB.c_str(), user_name, &struct2);

		//printf("%s\n","olo_file");

                if (res1 == -1)
                    {
                        // --------------------------------------------------------------------------
			int cl = 0;
			int cl_1 = 0;
	
			for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");	
				}
			cl_1 = 0;	
			}
	
			fflush (stdout);
			set_cursor_position (2, 0);
			fflush (stdout);

			set_color(36);
			square(height_square - 13, width_square - 15);
			start_line = 2;

			set_cursor_position (start_line + 2, 4);
			start_line++;

// --------------------------------------------------------------------------
			set_color(31);
                        std::cout << "При загрузке истории переписки с данным пользователем произошла ошибка." << std::endl; 
			set_cursor_position (start_line + 2, 4);
			start_line++;
			set_color(32);
			std::cout << "Проверте правильность введенных данных. " << std::endl;                
			fflush(stdout);
			sleep(2);
			   
                    }
                else if (res1 == 0)
                    {
                    
                    	//printf("%s\n","olo_file");
                    
                    	// Запрашиваем количество непрочитанных сообщений
                    	int new_messages_now = get_count_new_message(file_name_to_DB.c_str(),user_name);

			// Обнуляем количество непрочитанных сообщений
			
			update_new_message_count_to_user(file_name_to_DB.c_str(), user_name, 0);
                    	
                    	set_color(36);
                    	std::cout << "\n\tС пользователем ";
                    	set_color(32);
                    	std::cout << user_name;
                    	set_color(36);
                    	std::cout << " у вас ";
                    	set_color(37);
                    	std::cout << new_messages_now;
                    	set_color(36);
                    	std::cout << " непрочитанных сообщений.\n" << std::endl;
                    
                    	// Вывод значений struct2 ...
   
   			//std::cout << "\n ВЫВОД ЗНАЧЕНИЙ struct2.list_messages ... \n" << std::endl;
   			int j = 0;
   			for (j;j<struct2.pointer;j++)
   				{
   					// Красиво отображаем переписку
   					if (strcmp(struct2.list_hwo_sender[j].c_str(),MY_NAME.c_str()) != 0)
   						{
   							set_color(35);
   							std::cout << "[";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "] <";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << ">: \"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\"" << std::endl;
   						}
   					else
   						{
   							set_color(35);
   							std::cout << "\t\t\t\t\t\t\t\t\"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\" :<";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << "> [";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "]" << std::endl;
   						}
   				} 
                    
                         set_color(35);
                         std::cout << "-----------------------------------------------------\n" << std::endl;     
                          set_color(33);
                         std::cout << "Для выхода в главное меню введите ";
                         set_color(37);
                         std::cout << "\"";
                         set_color(31);
                         std::cout << "ESC";
                         set_color(37);
                         std::cout << "\"\n" << std::endl;
                         
                         
                         
                         // Вот тут можно отправить сообщение этому пользователю ...
                         set_color(36);
                         std::cout << "Файл должен находиться в директории";
                         set_color(37);
                         std::cout<<" <OUTPUT_FILES>\n";
                         std::string dir_name = "OUTPUT_FILES";
                         set_color(36);
                         std::cout << "Введите путь к файлу для отправки данному пользователю: ";
    			  std::string path_to_key_file;
    			  set_color(33);
			 fflush (stdout);
    			  ret_ = getline_with_block_from_file(path_to_key_file);
			if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
			while (path_to_key_file == "")
			{
				ret_ = getline_with_block_from_file(path_to_key_file);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}	
			}
    			  
    			  set_color(32);
			  std::cout << path_to_key_file;
			  fflush (stdout);

			  // Вот тут проверка на выход
			  if (path_to_key_file == "ESC") // Press "Esc"
			  {
			  	continue;
			  }


    			  std::string filename_for_sending = dir_name + "/" + path_to_key_file;
    			  set_color(36);
    			  std::cout << "\nПользователю будет отправлен файл: ";
    			  set_color(33);
    			  std::cout << filename_for_sending << std::endl;
                          fflush (stdout);
                         
                         
                         // Считываем этот файл в буффер и формируем строку необходимого формата для отправки файла
                         
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
    std::string filename_in_for_check =  path + filename_for_sending;
    int check_res = checkFile(filename_in_for_check.c_str());
    if (check_res == -1)
    {
        //set_cursor_position (start_line + 1, 4);
        //start_line++;
        set_color(31); 
        std::cout << "Файл '" << filename_in_for_check << "' не найден." << std::endl;
        //set_cursor_position (start_line + 1, 4);
        //start_line++;
        set_color(32);
        std::cout << "Проверьте правильность введенных данных." << std::endl;
        fflush(stdout);
        sleep(5);
        continue;                
    }


                         int file_size = filesize(filename_for_sending.c_str());
                         
                         // Проверка на максимальный размер файла
                         if (file_size > MAX_TRANSACTION_LENGTH)
                         	{
                         		std::cout << "Размер файла превышает максимально допустимый размер для отправки (1 Мб). Пожалуйста, выберете файл подходящего размера." << std::endl;
								fflush (stdout);
                         		continue;
                         	
                         	}
                         
                         
                         



                         /*
                         char buffer_key [file_size+1];
                         memset(buffer_key,0,file_size);
                         read_file_into_memory(filename_for_sending.c_str(), buffer_key, file_size);
                         buffer_key [file_size] = '\0';
                         const char* buffer_key_c = const_cast<const char*>(buffer_key);
                         std::string buffer_key_str (buffer_key_c);
                         */
                         std::string buffer_key_str;
                         int res_str_bin = bin_file_to_string(filename_for_sending.c_str(), &buffer_key_str);
                         if (res_str_bin != 1)
                         {
                         	printf("Ошибка при выполнении функции 'bin_file_to_string'.\n");
                         	fflush(stdout);
                         	sleep(5);
                         	continue;
                         }
                         //std::cout << "buffer_key_str = |" << buffer_key_str << "|" <<std::endl;
                         //std::cout << "buffer_key_str.length() = |" << buffer_key_str.length() << "|" <<std::endl;
    					 //fflush(stdout);
    					 //sleep(15);
                         
                         /*
                         char buffer_key [file_size+1];
                         memset(buffer_key,0,file_size);
                         read_file_into_memory(filename_for_sending.c_str(), buffer_key, file_size);
                         buffer_key [file_size] = '\0';
                         
                         const char* buffer_key_c = const_cast<const char*>(buffer_key);
                         
                         std::string buffer_key_str (buffer_key_c);
                         */
		  
			  // Вот функция отправки сообщения
			  
			  
			  // Функция получения номера последнего id
     				char last_id[BUF_SIZE];
     				memset(last_id, 0, BUF_SIZE);
     				get_how_much_id (file_name_to_DB.c_str(), last_id);
     				const char* last_id_str = last_id; 
     				int new_id_number = atoi(last_id);
     				new_id_number++; 
			  
			  
			  // Узнаем текущую дату и время
			  
			  
			     // current date/time based on current system
   			time_t now = time(0);

   			tm *ltm = localtime(&now);



			
			std::string date_time = "Date: " + std::to_string(ltm->tm_mday) +  "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(1900 + ltm->tm_year) + " | Time: " +
										 std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string( ltm->tm_sec);
	
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			std::string encrypt_message_str_0 =  "$file$" + buffer_key_str;
			
			std::string encrypt_message_str =  std::to_string(new_id_number) + ";" + user_name + ";" + MY_NAME + ";" + date_time + ";" + encrypt_message_str_0 + ";";
			
			const char* text_message = encrypt_message_str.c_str();
			const char* path_to_buf_file = "buf_file.txt";
			
		
			// Зашифровываем сообщение перед отправкой
			int res_enc = 0;
			const char* encrypt_message = encrypt_text_message (user_name.c_str(), text_message, &res_enc);
			if (res_enc == 0)
				continue;



			set_color(32);
            std::cout << "Отправка файла... ---> ";
            fflush(stdout);
            //set_cursor_position (start_line + 1, 4);
        	//start_line++;

			sending_file_flag = 1;
			// Непосредственно отправка сообщения
			int res_send;
			if (use_i2p == "FALSE")
				res_send = send_message (file_name_to_DB.c_str(), encrypt_message, port3, path_to_buf_file);
			else
				res_send = send_message_i2pd(encrypt_message, path_to_buf_file);
			sleep(1);
			if (res_send == 0)
				{
					//printf("%s\n","olo1_file");
					blockchain_from_file_to_DB (file_name_to_DB.c_str(), path_to_buf_file);
					//printf("%s\n","olo2_file");
					analyse_blockchain_for_input_messages(file_name_to_DB.c_str());
					//printf("%s\n","olo3_file");
					
					
					// Отдельно добавляем запись, которая была в открытом виде в пользовательскую таблицу

					std::string message_for_this_user_in_my_DB = "FILE. Path_to_file: " + filename_for_sending;

					add_record_to_user_table(file_name_to_DB.c_str(), user_name, new_id_number, MY_NAME, user_name, date_time, message_for_this_user_in_my_DB);					
					
					set_color(34);
                       		std::cout << "Файл успешно отправлен." << std::endl;
				fflush (stdout);
                       		sleep(2);  
                        	} 
                       else if (res_send == 1)
                       	{
                       		// seek-ключа нужно назад присвоить какой был
                       		
                       		// Запрос текущего
                       		int cur_seek = get_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str());
                       		              		
                       		// Считаем длину передаваемого сообщения
                       		int len_message = strlen(text_message);
                       		
                       		// Вычитаем длину из текущего смещения
                       		
                       		int different = cur_seek - len_message;
                       		
                       		// Разницу присваеваем
                       		set_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str(), different);	
                       		                       		
                       		//printf("%s\n","Смещение ключа возвращено на прежнее место.");
                       		if (use_i2p == "FALSE")
								get_actual_blockchain_copy(port2);
							else
								get_actual_blockchain_copy_i2pd();
    
        					// Очищение старой копии BLOCKCHAIN_TABLE
        					Clear_Blockchain_Table(PATH_Date_Base);
    
        					// Заполнение БД из файла
        					const char* path_to_buffer_file = "actual_blockchain.txt";
       						blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
        					analyse_blockchain_for_input_messages(PATH_Date_Base);
            				sleep(1);
                       		
                       		std::cout << "Ваше сообщение НЕ отправлено, но Blockchain обновлен. Попытайтесь еще раз." << std::endl;
							fflush (stdout);
                       		sleep(12); 
                       		
                       	}
                       
                       else 
                       	{
                       	
                       		std::cout << "При отправке сообщения ПРОИЗОШЛА НЕИЗВЕСТНАЯ ОШИБКА." << std::endl;
				fflush (stdout);
                       		exit(1);
                       	
                       	}	  
                         
                                       
                    }
                
            }















    		if (answer == "4")
        		{
        		
        			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 4]\n");
                    		// ----------------------------------------------------------------------------

    				// Обновление файла ключа пользователя

		            	set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "Введите имя пользователя для которого хотите обновить ключ: ";
    				std::string new_user_name;
    				set_color(34);
				fflush (stdout);
    				ret_ = getline_with_block_from_file(new_user_name);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
				while (new_user_name == "")
				{
					ret_ = getline_with_block_from_file(new_user_name);
					if (ret_ == 1)
					{
						std::cout << "При чтении из файла произошла ошибка." << std::endl;
						fflush (stdout);
					}	
				}



				// Проверка на существование такого пользователя
				int j = 0;
				int flag_have_user = 0;
   				for (j;j<struct1.pointer;j++)
   				{
   					if (struct1.user_list[j] == new_user_name)
   						flag_have_user = 1;
   				}
   				if (flag_have_user != 1)
   				{
   					set_color(31);
			  		std::cout << "Пользователя '" << new_user_name << "' нет в ваших контактах.";
			  		fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
					start_line++;
					std::cout << "Проверьте правильность введенных данных";
			  		fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
					start_line++;
					sleep(4);
					continue;
   				}


				set_color(32);
			  	std::cout << new_user_name;
			  	fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(36);
    				std::cout << "Введите путь к новому файлу с ключем для данного пользователя: ";
				fflush (stdout);
    				std::string path_to_key_file;
    				set_color(34);
    				ret_ = getline_with_block_from_file(path_to_key_file);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
    				while (path_to_key_file == "")
				{
					ret_ = getline_with_block_from_file(path_to_key_file);
					if (ret_ == 1)
					{
						std::cout << "При чтении из файла произошла ошибка." << std::endl;
						fflush (stdout);
					}	
				}
				set_color(32);
			  	std::cout << path_to_key_file;
			  	set_cursor_position (start_line + 1, 4);
				start_line++;
			  	fflush (stdout);
    				int update_res = update_key_file_for_user (file_name_to_DB.c_str(), new_user_name.c_str(), path_to_key_file.c_str());

    				if (update_res == -1)
        				{
        					set_cursor_position (start_line + 1, 4);
        					start_line++;
						set_color(31);
            					std::cout << "При обновлении файла-ключа произошла ошибка.";
						fflush(stdout);
						sleep(2);
            					//exit(1);
        				}
    				else if (update_res == 0)
        				{
        					set_cursor_position (start_line + 1, 4);
						start_line++;
						set_color(32);
            					std::cout << "Ключ для данного пользователя успешно обновлен.";
            					fflush(stdout);
            					sleep(2);
        				}

				// ----------------------------------------------------------------------------
        		}

	
	if (answer == "5")
		{
			// Отправить файл на хранение в распределенное хранилище
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 5]\n");

			
				set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "[ Путь относительно директории <BIG_FILES_OUTPUT> ]";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    				std::cout << "Введите имя файла, который необходимо отправить в распределенное хранилище: ";
    				std::string filename_for_sharing;
    				set_color(32);
				fflush (stdout);
    				ret_ = getline_with_block_from_file(filename_for_sharing);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
				while (filename_for_sharing == "")
				{
					ret_ = getline_with_block_from_file(filename_for_sharing);
					if (ret_ == 1)
					{
						std::cout << "При чтении из файла произошла ошибка." << std::endl;
						fflush (stdout);
					}	
				}

				set_color(32);
			  	std::cout << filename_for_sharing;
			  	fflush (stdout);
				std::string filename_for_sharing_reference = "OUTPUT_FILES/" + filename_for_sharing + "_REF";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    			std::cout << "Отправка файла на хранение в распределенное хранилище...";
    			//set_cursor_position (start_line + 1, 4);
				//start_line++;
				fflush (stdout);
				// Распределяем файл по сети
    				int share_res = share_file(filename_for_sharing.c_str(), filename_for_sharing_reference.c_str());

				if (share_res == 1)
				{
					continue;
				}

				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
    				std::cout << "Отправка файла на хранение в распределенное хранилище успешно завершена.";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
				std::cout << "Файл-ссылка расположен по пути: " << filename_for_sharing_reference;
				fflush(stdout);
				sleep(5);

			
		}
	
	
	if (answer == "6")
		{
			// Запросить файл из распределенного хранилища
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 6]\n");

			set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(33);
    				std::cout << "[ По умолчанию файлы-ссылки могут находиться в директориях <OUTPUT_FILES> или <INPUT_FILES> ]";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				std::cout << "[ Вводите путь к файлу-ссылке относительно исполняемой директории ] ";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
				std::cout << "Введите название файла-ссылки: ";
    				std::string filename_reference_file;
    				set_color(32);
				fflush (stdout);
    				ret_ = getline_with_block_from_file(filename_reference_file);
				if (ret_ == 1)
				{
					std::cout << "При чтении из файла произошла ошибка." << std::endl;
					fflush (stdout);
				}
				while (filename_reference_file == "")
				{
					ret_ = getline_with_block_from_file(filename_reference_file);
					if (ret_ == 1)
					{
						std::cout << "При чтении из файла произошла ошибка." << std::endl;
						fflush (stdout);
					}	
				}
				set_color(32);
			  	std::cout << filename_reference_file;
			  	fflush (stdout);
				//std::string filename_for_union_file = "UNION_FILE_from__" + filename_reference_file; 
    			set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    			std::cout << "Запрос оригинала файла из распределенного хранилища ...";
    			//set_cursor_position (start_line + 1, 4);
				//start_line++;
				fflush (stdout);
				// Запрос файла из сети
    				int res_union = union_file(filename_reference_file.c_str());
				if (res_union == 1)
				{
					continue;
				}
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
				std::cout << "Запрос файла из распределенного хранилища успешно завершен. ";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				//std::size_t pos1 = filename_reference_file.find("//");
    				//std::size_t pos2 = filename_reference_file.find("_REF");
				std::cout << "Файл-оригинал расположен по пути: " << "BIG_FILES_INPUT/" << "<Имя_файла_оригинала>" << std::endl;
				fflush(stdout);
				sleep(5);
			
		}

	
	if (answer == "7")
		{
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 7]\n");

			if (control_flag == 1)
				{
					control_flag = 0;
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(32);
					std::cout << "Произведена смена управления программой: " << "Файл ---> Консоль" << std::endl;
					fflush(stdout);
					sleep(2);
				}

			else if (control_flag == 0)
				{
					control_flag = 1;
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(32);
					std::cout << "Произведена смена управления программой: " << "Консоль ---> Файл" << std::endl;
					fflush(stdout);
					sleep(2);
				}

			else 
				{
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(31);
					std::cout << "Сбой при смене управления." << std::endl;
					fflush(stdout);
					sleep(2);
				}

			continue;
			
		}


	if (answer == "8")
	   {
	   	
        write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 8]\n");
		set_cursor_position (start_line + 1, 4);
    		start_line++;
    		// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети

    	
		set_color(34); 
    	std::cout << "Запрос на обновление глобального блокчейна...";
    	fflush(stdout);
		set_cursor_position (start_line + 1, 4);
    	start_line++;
    	set_color(32);
		//get_actual_blockchain_copy(port2);
	
		// Очищение старой копии BLOCKCHAIN_TABLE
		if (use_i2p == "FALSE")
			get_actual_blockchain_copy(port2);
		else
			get_actual_blockchain_copy_i2pd();
		Clear_Blockchain_Table(PATH_Date_Base);
	
		// Заполнение БД из файла
		const char* path_to_buffer_file = "actual_blockchain.txt";
		blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);



        std::string last_available_id;
    get_last_available_id_in_blockchain(file_name_to_DB.c_str(), &last_available_id);
    int lai = atoi(last_available_id.c_str());
    
    // Запросить количество транзакций в новом блокчейне
    char last_id_c[BUF_SIZE];
        memset(last_id_c, 0, BUF_SIZE);
    get_how_much_id (file_name_to_DB.c_str(), last_id_c);
    int lis = atoi(const_cast<const char*>(last_id_c));
    
    //std::cout << "lai = " << lai << std::endl;
    //std::cout << "lis = " << lis << std::endl;
    
    // Проверка на то, нужно ли обнулять ключи всем пользователям (это если за время отстутствия произошло обнуление глобального блокчейна)
    if (lai > lis)
        {
            //std::cout << "lai > lis" << std::endl;
            // Обнуление ключей всех пользователей
            set_last_analysed_id_in_blockchain(file_name_to_DB.c_str(), 1);
            set_last_available_id_in_blockchain(file_name_to_DB.c_str(), 1);
            key_seek_zero_for_all_users();
        }



		analyse_blockchain_for_input_messages(PATH_Date_Base);
    	sleep(1);
		printf("%s","Обновление локального блокчейна по запросу произведено успешно.\n");
		


		//printf("%s\n","\n\tРзработчик принял решение, что использование данной функции пользователем нерационально.\n");
		sleep(2);
		continue;
	    }


	if (answer == "9")
            {
		// Обновление окна
		write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 9]\n");
		continue;
	    }

        if (answer == "10")
            {
            
            	 write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 10]\n");
            	 // Зашифровываем файл БД перед уходом (если выйти из программы НЕ таким способом, то все данные, полученные после последнего запуска БД будут утеряны!)
            	 // Поэтому, обязательно выходить правильно из программы
            	 std::string path_DB_str (PATH_Date_Base);
            	 std::string path_enc_DB_str  = path_DB_str + ".enc";
            	 encrypt_file (PATH_Date_Base, path_enc_DB_str.c_str(), password.c_str());   
            	 
            	 // Удаляем открытый файл БД (чтобы не висел там нигде)
            	 std::string string_programm  = "rm " + path_DB_str;
            	 char* ptr; // для совместимости
		 call_Linux_programm(string_programm.c_str(),ptr);
		 
		 // Удаляем файл действительных IP-адресов (потому, что когда мы зайдем в след.раз, то он уже будет другой)
            	 std::string string_programm1  = "rm user_list_client.txt";
            	 char* ptr1; // для совместимости
		 //call_Linux_programm(string_programm1.c_str(),ptr1);
            	 
            	 
            	 
            	 int cl = 0;
		int cl_1 = 0;
	
		for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
					{
						set_cursor_position (cl, cl_1);
						printf("%s"," ");
					
					
					}
				cl_1 = 0;	
			}
	
		fflush (stdout);
		set_cursor_position (0, 0);
		fflush (stdout);
         	 set_color(32);
                std::cout << "Работа программы будет завершена без ошибки. " << std::endl;
		fflush (stdout);
                return 0; 
            }
 

}


		


	}

if (control_flag == 0)
	{
		

	std::getline(std::cin, answer);
//	std::cout << "" << std::endl;

	if ((answer != "1") and (answer != "2") and (answer != "3") and (answer != "4") and (answer != "5") and (answer != "6") and (answer != "7") and (answer != "8") and (answer != "9") and (answer != "10"))
    	{

    		//set_cursor_position (start_line + 1, 4);
		//start_line++;
        	//std::cout << "Введено некорректное значение функции. Работа программы будет прекращена с ошибкой.";
//       	exit(1);
		set_cursor_position (start_line + 2, 4);
		set_color(31);
		std::cout << "Введено некорректное значение функции." << std::endl;
		set_cursor_position (start_line + 1, 4);
		start_line++;
		set_color(32);
		std::cout << "Проверьте введенные данные." << std::endl;
		fflush (stdout);
		sleep(2);
		continue;


/*
//-----------------------------------
write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 8]\n");
            	 // Зашифровываем файл БД перед уходом (если выйти из программы НЕ таким способом, то все данные, полученные после последнего запуска БД будут утеряны!)
            	 // Поэтому, обязательно выходить правильно из программы
            	 std::string path_DB_str (PATH_Date_Base);
            	 std::string path_enc_DB_str  = path_DB_str + ".enc";
            	 encrypt_file (PATH_Date_Base, path_enc_DB_str.c_str(), password.c_str());   
            	 
            	 // Удаляем открытый файл БД (чтобы не висел там нигде)
            	 std::string string_programm  = "rm " + path_DB_str;
            	 char* ptr; // для совместимости
		 call_Linux_programm(string_programm.c_str(),ptr);
		 
		 // Удаляем файл действительных IP-адресов (потому, что когда мы зайдем в след.раз, то он уже будет другой)
            	 std::string string_programm1  = "rm user_list_client.txt";
            	 char* ptr1; // для совместимости
		 call_Linux_programm(string_programm1.c_str(),ptr1);
            	 
            	 
            	 
            	 int cl = 0;
		int cl_1 = 0;
	
		for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
					{
						set_cursor_position (cl, cl_1);
						printf("%s"," ");
					
					
					}
				cl_1 = 0;	
			}
	
		fflush (stdout);
		set_cursor_position (0, 0);
		fflush (stdout);
         	 set_color(32);
                std::cout << "Работа программы будет завершена с ошибкой, Возможна потеря данных и рассинхронизация ключей шифрования !!!. " << std::endl;
                return 0; 
//-----------------------------------
*/

 


    	}	
	else 
	{
    		if (answer == "1")
        		{
        			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 1]\n");
                    		// ----------------------------------------------------------------------------

    				// Добавление нового пользователя
				set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "Введите имя нового пользователя для добавления в ваши контакты: ";
    				std::string new_user_name;
    				set_color(32);
    				std::getline(std::cin, new_user_name);
    				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(36);
    				std::cout << "Введите путь к файлу с ключом для нового пользователя: ";
    				std::string path_to_key_file;
    				set_color(32);
    				std::getline(std::cin, path_to_key_file);
    				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    				std::cout << "Добавление нового пользователя...";
    				set_cursor_position (start_line + 1, 4);
					start_line++;
    				int add_res = create_table_for_new_user (file_name_to_DB.c_str(), new_user_name, path_to_key_file);

    				if (add_res == -1)
        				{
						set_cursor_position (start_line + 1, 4);
						start_line++;
						set_color(31);
            					std::cout << "При добавлении нового пользователя произошла ошибка." << std::endl;
						fflush (stdout);
						sleep(2);
            					//exit(1);
        				}
    				else if (add_res == 0)
        				{
        					//set_cursor_position (start_line + 1, 4);
						//start_line++;
						set_color(32);
            					std::cout << "Добавление нового пользователя успешно завершено." << std::endl;
            					fflush (stdout);
            					sleep(2);
        				}

				// ----------------------------------------------------------------------------
        		}




        if (answer == "2")
            {
            
            	write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 2]\n");
            	user_messages_list_and_pointer struct2;
            
            	set_cursor_position (start_line + 2, 4);
		start_line++;
		start_line++;
		set_color(36);
                std::cout << "Введите имя пользователя, с которым хотите прочитать историю переписки: ";
                std::string user_name;
                set_color(32);
                std::getline(std::cin, user_name);
                
               // char* ptr; // для совместимости
		//call_Linux_programm("clear",ptr);
                
	int cl = 0;
	int cl_1 = 0;
	
	for(cl; cl<height_square + 5 ; cl++)
		{
			for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");
					
					
				}
			cl_1 = 0;	
		}
	
	fflush (stdout);
	set_cursor_position (2, 0);
	fflush (stdout);
                set_color(34);
                std::cout << "\n\t\tИстория переписки с пользователем: ";
                set_color(32);
                std::cout << user_name << std::endl;

                int res1 = return_my_messages_with_user (file_name_to_DB.c_str(), user_name, &struct2);

		//printf("%s\n","olo");

                if (res1 == -1)
                    {

// --------------------------------------------------------------------------
			int cl = 0;
			int cl_1 = 0;
	
			for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");	
				}
			cl_1 = 0;	
			}
	
			fflush (stdout);
			set_cursor_position (2, 0);
			fflush (stdout);

			set_color(36);
			square(height_square - 13, width_square - 15);
			start_line = 2;

			set_cursor_position (start_line + 2, 4);
			start_line++;

// --------------------------------------------------------------------------
			set_color(31);
                        std::cout << "При загрузке истории переписки с данным пользователем произошла ошибка." << std::endl; 
			set_cursor_position (start_line + 2, 4);
			start_line++;
			set_color(32);
			std::cout << "Проверте правильность введенных данных. " << std::endl;                
			fflush(stdout);
			sleep(2);
			
                         //exit(1);   
                    }
                else if (res1 == 0)
                    {
                    
                    	//printf("%s\n","olo");
                    
                    	// Запрашиваем количество непрочитанных сообщений
                    	int new_messages_now = get_count_new_message(file_name_to_DB.c_str(),user_name);

			// Обнуляем количество непрочитанных сообщений
			
			update_new_message_count_to_user(file_name_to_DB.c_str(), user_name, 0);
                    	set_color(36);
                    	std::cout << "\n\tС пользователем ";
                    	set_color(32);
                    	std::cout << user_name;
                    	set_color(36);
                    	std::cout << " у вас ";
                    	set_color(37);
                    	std::cout << new_messages_now;
                    	set_color(36);
                    	std::cout << " непрочитанных сообщений.\n" << std::endl;
                    
                    	// Вывод значений struct2 ...
   
   			//std::cout << "\n ВЫВОД ЗНАЧЕНИЙ struct2.list_messages ... \n" << std::endl;
   			int j = 0;
   			for (j;j<struct2.pointer;j++)
   				{
   				
   					// Красиво отображаем переписку
   					if (strcmp(struct2.list_hwo_sender[j].c_str(),MY_NAME.c_str()) != 0)
   						{
   							set_color(35);
   							std::cout << "[";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "] <";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << ">: \"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\"" << std::endl;
   						}
   					else
   						{
   							set_color(35);
   							std::cout << "\t\t\t\t\t\t\t\t\"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\" :<";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << "> [";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "]" << std::endl;
   						}	
   				} 
                    
                    	set_color(35);
                         std::cout << "-------------------------------------------------------------\n " << std::endl;     
                          set_color(33);
                         std::cout << "Для выхода в главное меню введите ";
                         set_color(37);
                         std::cout << "\"";
                         set_color(31);
                         std::cout << "ESC";
                         set_color(37);
                         std::cout << "\"\n" << std::endl;
                         // Вот тут можно отправить сообщение этому пользователю ...
                         set_color(36);
                         std::cout << "Введите сообщение данному пользователю: ";
                         std::string message;
                         set_color(33);
                         std::getline(std::cin, message);
			  std::cout << "" << std::endl;
			  

			  if (message == "ESC") // Press "Esc"
			  {
			  	 continue;
			  }

			  // Вот функция отправки сообщения
			  
			
			
			  // Функция получения номера последнего id
     				char last_id[10]; // BUF_SIZE
     				memset(last_id, 0, 10); // BUF_SIZE
     				get_how_much_id (file_name_to_DB.c_str(), last_id);
     				const char* last_id_str = last_id; 
     				int new_id_number = atoi(last_id);
     				new_id_number++; 
			  
			  
			  // Узнаем текущую дату и время
			  
			  
			     // current date/time based on current system
   			time_t now = time(0);

   			tm *ltm = localtime(&now);



			
			std::string date_time = "Date: " + std::to_string(ltm->tm_mday) +  "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(1900 + ltm->tm_year) + " | Time: " +
										 std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string( ltm->tm_sec);
 		  	
			  
			std::string encrypt_message_str =  std::to_string(new_id_number) + ";" + user_name + ";" + MY_NAME + ";" + date_time + ";" + message + ";";
			const char* text_message = encrypt_message_str.c_str();
			const char* path_to_buf_file = "buf_file.txt";
			//user_pause();
		
			// Зашифровываем сообщение перед отправкой
			int res_enc = 0;
			const char* encrypt_message = encrypt_text_message (user_name.c_str(), text_message, &res_enc);
			if (res_enc == 0)
				continue;
			//printf("encrypt_message = %|%s|\n",encrypt_message);
			// Непосредственно отправка сообщения
			//user_pause();

			set_color(32);
            std::cout << "Отправка сообщения... --- > ";
            fflush(stdout);
            //set_cursor_position (start_line + 1, 4);
        	//start_line++;

			int res_send;
			if (use_i2p == "FALSE")
				res_send = send_message (file_name_to_DB.c_str(), encrypt_message, port3, path_to_buf_file);
			else
				res_send = send_message_i2pd(encrypt_message, path_to_buf_file);
			sleep(1);
			if (res_send == 0)
				{
					//printf("%s\n","olo1");
					blockchain_from_file_to_DB (file_name_to_DB.c_str(), path_to_buf_file);
					//printf("%s\n","olo2");
					analyse_blockchain_for_input_messages(file_name_to_DB.c_str());
					//printf("%s\n","olo3");
					
					
					// Отдельно добавляем запись, которая была в открытом виде в пользовательскую таблицу


					add_record_to_user_table(file_name_to_DB.c_str(), user_name, new_id_number, MY_NAME, user_name, date_time, message);					
					
					set_color(32);
                    std::cout << "Ваше сообщение успешно отправлено." << std::endl;
                    sleep(2);  
                } 
                       else if (res_send == 1)
                       	{
                       		// seek-ключа нужно назад присвоить какой был
                       		
                       		// Запрос текущего
                       		int cur_seek = get_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str());
                       		              		
                       		// Считаем длину передаваемого сообщения
                       		int len_message = strlen(text_message);
                       		
                       		// Вычитаем длину из текущего смещения
                       		
                       		int different = cur_seek - len_message;
                       		
                       		// Разницу присваеваем
                       		set_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str(), different);	
                       		                       		
                       		//printf("%s\n","Смещение ключа возвращено на прежнее место.");
                       		if (use_i2p == "FALSE")
								get_actual_blockchain_copy(port2);
							else
								get_actual_blockchain_copy_i2pd();
                       		
    
        					// Очищение старой копии BLOCKCHAIN_TABLE
        					Clear_Blockchain_Table(PATH_Date_Base);
    
        					// Заполнение БД из файла
        					const char* path_to_buffer_file = "actual_blockchain.txt";
       						blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
        					analyse_blockchain_for_input_messages(PATH_Date_Base);
            				sleep(1);
                       		
                       		std::cout << "Ваше сообщение НЕ отправлено, но Blockchain обновлен. Попытайтесь еще раз." << std::endl;
							fflush (stdout);
                       		sleep(12); 
                       		
                       		
                       	}
                       
                       else 
                       	{
                       	
                       		std::cout << "При отправке сообщения ПРОИЗОШЛА НЕИЗВЕСТНАЯ ОШИБКА." << std::endl;
                       		exit(1);
                       	
                       	}	  
                         
                                       
                    }
                
            }









        if (answer == "3")
            {
            write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 3]\n");
            	user_messages_list_and_pointer struct2;

            
            	set_cursor_position (start_line + 2, 4);
		start_line++;
		start_line++;
		set_color(36);
                std::cout << "Введите имя пользователя, с которым хотите прочитать историю переписки: ";
                std::string user_name;
                set_color(32);
                std::getline(std::cin, user_name);
                
               // char* ptr; // для совместимости
		// call_Linux_programm("clear",ptr);


  	int cl = 0;
	int cl_1 = 0;
	
	for(cl; cl<height_square + 5 ; cl++)
		{
			for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");
					
					
				}
			cl_1 = 0;	
		}
	
	fflush (stdout);
	set_cursor_position (2, 0);
	fflush (stdout);


		set_color(34);
                std::cout << "\n\t\tИстория переписки с пользователем: ";
                set_color(32);
                std::cout << user_name << std::endl;

                int res1 = return_my_messages_with_user (file_name_to_DB.c_str(), user_name, &struct2);

		//printf("%s\n","olo_file");

                if (res1 == -1)
                    {
                        // --------------------------------------------------------------------------
			int cl = 0;
			int cl_1 = 0;
	
			for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
				{
					set_cursor_position (cl, cl_1);
					printf("%s"," ");	
				}
			cl_1 = 0;	
			}
	
			fflush (stdout);
			set_cursor_position (2, 0);
			fflush (stdout);

			set_color(36);
			square(height_square - 13, width_square - 15);
			start_line = 2;

			set_cursor_position (start_line + 2, 4);
			start_line++;

// --------------------------------------------------------------------------
			set_color(31);
                        std::cout << "При загрузке истории переписки с данным пользователем произошла ошибка." << std::endl; 
			set_cursor_position (start_line + 2, 4);
			start_line++;
			set_color(32);
			std::cout << "Проверте правильность введенных данных. " << std::endl;                
			fflush(stdout);
			sleep(2);
			   
                    }
                else if (res1 == 0)
                    {
                    
                    	//printf("%s\n","olo_file");
                    
                    	// Запрашиваем количество непрочитанных сообщений
                    	int new_messages_now = get_count_new_message(file_name_to_DB.c_str(),user_name);

			// Обнуляем количество непрочитанных сообщений
			
			update_new_message_count_to_user(file_name_to_DB.c_str(), user_name, 0);
                    	
                    	set_color(36);
                    	std::cout << "\n\tС пользователем ";
                    	set_color(32);
                    	std::cout << user_name;
                    	set_color(36);
                    	std::cout << " у вас ";
                    	set_color(37);
                    	std::cout << new_messages_now;
                    	set_color(36);
                    	std::cout << " непрочитанных сообщений.\n" << std::endl;
                    
                    	// Вывод значений struct2 ...
   
   			//std::cout << "\n ВЫВОД ЗНАЧЕНИЙ struct2.list_messages ... \n" << std::endl;
   			int j = 0;
   			for (j;j<struct2.pointer;j++)
   				{
   					// Красиво отображаем переписку
   					if (strcmp(struct2.list_hwo_sender[j].c_str(),MY_NAME.c_str()) != 0)
   						{
   							set_color(35);
   							std::cout << "[";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "] <";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << ">: \"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\"" << std::endl;
   						}
   					else
   						{
   							set_color(35);
   							std::cout << "\t\t\t\t\t\t\t\t\"";
   							set_color(33);
   							std::cout << struct2.list_messages[j];
   							set_color(35);
   							std::cout << "\" :<";
   							set_color(37);
   							std::cout << struct2.list_hwo_sender[j];
   							set_color(35);
   							std::cout << "> [";
   							set_color(36);
   							std::cout << struct2.list_time_of_sending[j];
   							set_color(35);
   							std::cout << "]" << std::endl;
   						}
   				} 
                    
                         set_color(35);
                         std::cout << "-----------------------------------------------------\n" << std::endl;     
                          set_color(33);
                         std::cout << "Для выхода в главное меню введите ";
                         set_color(37);
                         std::cout << "\"";
                         set_color(31);
                         std::cout << "ESC";
                         set_color(37);
                         std::cout << "\"\n" << std::endl;
                         
                         
                         
                         // Вот тут можно отправить сообщение этому пользователю ...
                         set_color(36);
                         std::cout << "Файл должен находиться в директории";
                         set_color(37);
                         std::cout<<" <OUTPUT_FILES>\n";
                         std::string dir_name = "OUTPUT_FILES";
                         set_color(36);
                         std::cout << "Введите путь к файлу для отправки данному пользователю: ";
    			  std::string path_to_key_file;
    			  set_color(33);
    			  std::getline(std::cin, path_to_key_file);
    			  // Вот тут проверка на выход
			  	  if (path_to_key_file == "ESC") // Press "Esc"
			  	  {
			  	  	continue;
			  	  }
    			  
    			  std::string filename_for_sending = dir_name + "/" + path_to_key_file;
    			  set_color(36);
    			  std::cout << "\nПользователю будет отправлен файл: ";
    			  set_color(33);
    			  std::cout << filename_for_sending << std::endl;
                         
                         
                         


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
    std::string filename_in_for_check =  path + filename_for_sending;
    int check_res = checkFile(filename_in_for_check.c_str());
    if (check_res == -1)
    {
        //set_cursor_position (start_line + 1, 4);
        //start_line++;
        set_color(31); 
        std::cout << "Файл '" << filename_in_for_check << "' не найден." << std::endl;
        //set_cursor_position (start_line + 1, 4);
        //start_line++;
        set_color(32);
        std::cout << "Проверьте правильность введенных данных." << std::endl;
        fflush(stdout);
        sleep(3);
        continue;                
    }


                         // Считываем этот файл в буффер и формируем строку необходимого формата для отправки файла
                         
                         
                         int file_size = filesize(filename_for_sending.c_str());
                         
                         // Проверка на максимальный размер файла
                         if (file_size > MAX_TRANSACTION_LENGTH)
                         	{
                         		std::cout << "Размер файла превышает максимально допустимый размер для отправки (1 Мб). Пожалуйста, выберете файл подходящего размера." << std::endl;
                         		continue;
                         	
                         	}
                         
                         



                         /*
                         char buffer_key [file_size+1];
                         memset(buffer_key,0,file_size);
                         read_file_into_memory(filename_for_sending.c_str(), buffer_key, file_size);
                         buffer_key [file_size] = '\0';
                         const char* buffer_key_c = const_cast<const char*>(buffer_key);
                         std::string buffer_key_str (buffer_key_c);
                         */
                         std::string buffer_key_str;
                         int res_str_bin = bin_file_to_string(filename_for_sending.c_str(), &buffer_key_str);
                         if (res_str_bin != 1)
                         {
                         	printf("Ошибка при выполнении функции 'bin_file_to_string'.\n");
                         	fflush(stdout);
                         	sleep(5);
                         	continue;
                         }
                         //std::cout << "buffer_key_str = |" << buffer_key_str << "|" <<std::endl;
                         //std::cout << "buffer_key_str.length() = |" << buffer_key_str.length() << "|" <<std::endl;
    					 //fflush(stdout);
    					 //sleep(15);




		  
			  // Вот функция отправки сообщения
			  
			  
			  // Функция получения номера последнего id
     				char last_id[10]; // BUF_SIZE
     				memset(last_id, 0, 10); // BUF_SIZE
     				get_how_much_id (file_name_to_DB.c_str(), last_id);
     				const char* last_id_str = last_id; 
     				int new_id_number = atoi(last_id);
     				new_id_number++; 
			  
			  
			  // Узнаем текущую дату и время
			  
			  
			     // current date/time based on current system
   			time_t now = time(0);

   			tm *ltm = localtime(&now);



			
			std::string date_time = "Date: " + std::to_string(ltm->tm_mday) +  "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(1900 + ltm->tm_year) + " | Time: " +
										 std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string( ltm->tm_sec);
	
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			std::string encrypt_message_str_0 =  "$file$" + buffer_key_str;
			
			std::string encrypt_message_str =  std::to_string(new_id_number) + ";" + user_name + ";" + MY_NAME + ";" + date_time + ";" + encrypt_message_str_0 + ";";
			
			const char* text_message = encrypt_message_str.c_str();
			const char* path_to_buf_file = "buf_file.txt";
			
		
			// Зашифровываем сообщение перед отправкой
			int res_enc = 0;
			const char* encrypt_message = encrypt_text_message (user_name.c_str(), text_message, &res_enc);
			if (res_enc == 0)
				continue;

			set_color(32);
            std::cout << "Отправка файла... ---> ";
            fflush(stdout);
            //set_cursor_position (start_line + 1, 4);
        	//start_line++;

			sending_file_flag = 1;
			// Непосредственно отправка сообщения
			int res_send;
			if (use_i2p == "FALSE")
				res_send = send_message (file_name_to_DB.c_str(), encrypt_message, port3, path_to_buf_file);
			else
				res_send = send_message_i2pd(encrypt_message, path_to_buf_file);
			sleep(1);
			if (res_send == 0)
				{
					//printf("%s\n","olo1_file");
					blockchain_from_file_to_DB (file_name_to_DB.c_str(), path_to_buf_file);
					//printf("%s\n","olo2_file");
					analyse_blockchain_for_input_messages(file_name_to_DB.c_str());
					//printf("%s\n","olo3_file");
					
					
					// Отдельно добавляем запись, которая была в открытом виде в пользовательскую таблицу

					std::string message_for_this_user_in_my_DB = "FILE. Path_to_file: " + filename_for_sending;

					add_record_to_user_table(file_name_to_DB.c_str(), user_name, new_id_number, MY_NAME, user_name, date_time, message_for_this_user_in_my_DB);					
					
					set_color(34);
                       		std::cout << "Файл успешно отправлен." << std::endl;
                       		sleep(2);  
                        	} 
                       else if (res_send == 1)
                       	{
                       		// seek-ключа нужно назад присвоить какой был
                       		
                       		// Запрос текущего
                       		int cur_seek = get_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str());
                       		              		
                       		// Считаем длину передаваемого сообщения
                       		int len_message = strlen(text_message);
                       		
                       		// Вычитаем длину из текущего смещения
                       		
                       		int different = cur_seek - len_message;
                       		
                       		// Разницу присваеваем
                       		set_key_seek_for_user(file_name_to_DB.c_str(), user_name.c_str(), different);	
                       		                       		
                       		//printf("%s\n","Смещение ключа возвращено на прежнее место.");
                       		 if (use_i2p == "FALSE")
								get_actual_blockchain_copy(port2);
							else
								get_actual_blockchain_copy_i2pd();
    
        					// Очищение старой копии BLOCKCHAIN_TABLE
        					Clear_Blockchain_Table(PATH_Date_Base);
    
        					// Заполнение БД из файла
        					const char* path_to_buffer_file = "actual_blockchain.txt";
       						blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);
        					analyse_blockchain_for_input_messages(PATH_Date_Base);
            				sleep(1);
                       		
                       		std::cout << "Ваше сообщение НЕ отправлено, но Blockchain обновлен. Попытайтесь еще раз." << std::endl;
							fflush (stdout);
                       		sleep(12); 

                       	}
                       
                       else 
                       	{
                       	
                       		std::cout << "При отправке сообщения ПРОИЗОШЛА НЕИЗВЕСТНАЯ ОШИБКА." << std::endl;
                       		exit(1);
                       	
                       	}	  
                         
                                       
                    }
                
            }















    		if (answer == "4")
        		{
        		
        			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 4]\n");
                    		// ----------------------------------------------------------------------------

    				// Обновление файла ключа пользователя

		            	set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "Введите имя пользователя для которого хотите обновить ключ: ";
    				std::string new_user_name;
    				set_color(34);
    				std::getline(std::cin, new_user_name);



    				// Проверка на существование такого пользователя
				int j = 0;
				int flag_have_user = 0;
   				for (j;j<struct1.pointer;j++)
   				{
   					if (struct1.user_list[j] == new_user_name)
   						flag_have_user = 1;
   				}
   				if (flag_have_user != 1)
   				{
   					set_color(31);
			  		std::cout << "Пользователя '" << new_user_name << "' нет в ваших контактах.";
			  		fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
					start_line++;
					std::cout << "Проверьте правильность введенных данных";
			  		fflush (stdout);
    				set_cursor_position (start_line + 1, 4);
					start_line++;
					sleep(4);
					continue;
   				}


    				
    				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(36);
    				std::cout << "Введите путь к новому файлу с ключем для данного пользователя: ";
    				std::string path_to_key_file;
    				set_color(34);
    				std::getline(std::cin, path_to_key_file);
    				set_cursor_position (start_line + 1, 4);
					start_line++;
    				int update_res = update_key_file_for_user (file_name_to_DB.c_str(), new_user_name.c_str(), path_to_key_file.c_str());

    				if (update_res == -1)
        				{
        					set_cursor_position (start_line + 1, 4);
        					start_line++;
						set_color(31);
            					std::cout << "При обновлении файла-ключа произошла ошибка.";
						fflush(stdout);
						sleep(2);
            					//exit(1);
        				}
    				else if (update_res == 0)
        				{
        					set_cursor_position (start_line + 1, 4);
						start_line++;
						set_color(32);
            					std::cout << "Ключ для данного пользователя успешно обновлен.";
            					fflush(stdout);
            					sleep(2);
        				}

				// ----------------------------------------------------------------------------
        		}

	
	if (answer == "5")
		{
			// Отправить файл на хранение в распределенное хранилище
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 5]\n");

			
				set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(36);
    				std::cout << "[ Путь относительно директории <BIG_FILES_OUTPUT> ]";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    				std::cout << "Введите имя файла, который необходимо отправить в распределенное хранилище: ";
    				std::string filename_for_sharing;
    				set_color(32);
    				std::getline(std::cin, filename_for_sharing);
				std::string filename_for_sharing_reference = "OUTPUT_FILES/" + filename_for_sharing + "_REF";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    			std::cout << "Отправка файла на хранение в распределенное хранилище...";
    			//set_cursor_position (start_line + 1, 4);
				//start_line++;
				fflush(stdout);
				// Распределяем файл по сети
    				int share_res = share_file(filename_for_sharing.c_str(), filename_for_sharing_reference.c_str());

				if (share_res == 1)
				{
					continue;
				}

				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
    			std::cout << "Отправка файла на хранение в распределенное хранилище успешно завершена.";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
				std::cout << "Файл-ссылка расположен по пути: " << filename_for_sharing_reference;
				fflush(stdout);
				sleep(5);

			
		}
	
	
	if (answer == "6")
		{
			// Запросить файл из распределенного хранилища
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 6]\n");

			set_cursor_position (start_line + 2, 4);
				start_line++;
				start_line++;
				set_color(33);
    				std::cout << "[ По умолчанию файлы-ссылки могут находиться в директориях <OUTPUT_FILES> или <INPUT_FILES> ]";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				std::cout << "[ Вводите путь к файлу-ссылке относительно исполняемой директории ] ";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
				std::cout << "Введите название файла-ссылки: ";
    			std::string filename_reference_file;
    			set_color(32);
    			std::getline(std::cin, filename_reference_file);
				//std::string filename_for_union_file = "UNION_FILE_from__" + filename_reference_file; 
    			set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(34);
    			std::cout << "Запрос оригинала файла из распределенного хранилища ...";
    			//set_cursor_position (start_line + 1, 4);
				//start_line++;
    			fflush(stdout);
				// Запрос файла из сети
    				int res_union = union_file(filename_reference_file.c_str());
				if (res_union == 1)
				{
					continue;
				}
				set_cursor_position (start_line + 1, 4);
				start_line++;
				set_color(32);
				std::cout << "Запрос файла из распределенного хранилища успешно завершен. ";
				set_cursor_position (start_line + 1, 4);
				start_line++;
				//std::size_t pos1 = filename_reference_file.find("//");
    				//std::size_t pos2 = filename_reference_file.find("_REF");
				std::cout << "Файл-оригинал расположен по пути: " << "BIG_FILES_INPUT/" << "<Имя_файла_оригинала>" << std::endl;
				fflush(stdout);
				sleep(5);
			
		}

	
	if (answer == "7")
		{
			write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 7]\n");
			if (control_flag == 1)
				{
					control_flag = 0;
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(32);
					std::cout << "Произведена смена управления программой: " << "Файл ---> Консоль" << std::endl;
					fflush(stdout);
					sleep(2);
				}

			else if (control_flag == 0)
				{
					control_flag = 1;
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(32);
					std::cout << "Произведена смена управления программой: " << "Консоль ---> Файл" << std::endl;
					fflush(stdout);
					sleep(2);
				}

			else 
				{
					set_cursor_position (start_line + 1, 4);
					start_line++;
					set_color(31);
					std::cout << "Сбой при смене управления." << std::endl;
					fflush(stdout);
					sleep(2);
				}

			continue;
			
		}


	if (answer == "8")
	   {
        write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 8]\n");
		set_cursor_position (start_line + 1, 4);
    		start_line++;
    		// Запись в файл "actual_blockchain.txt" актуальной на данный момент копии блокчейна в сети

    	set_color(34); 
    	std::cout << "Запрос на обновление глобального блокчейна...";
    	fflush(stdout);
		set_cursor_position (start_line + 1, 4);
    	start_line++;
    	set_color(32);

    	
    	if (use_i2p == "FALSE")
			get_actual_blockchain_copy(port2);
		else
			get_actual_blockchain_copy_i2pd();
		// Очищение старой копии BLOCKCHAIN_TABLE
		Clear_Blockchain_Table(PATH_Date_Base);
	
		// Заполнение БД из файла
		const char* path_to_buffer_file = "actual_blockchain.txt";
		blockchain_from_file_to_DB (PATH_Date_Base, path_to_buffer_file);




        std::string last_available_id;
    get_last_available_id_in_blockchain(file_name_to_DB.c_str(), &last_available_id);
    int lai = atoi(last_available_id.c_str());
    
    // Запросить количество транзакций в новом блокчейне
    char last_id_c[BUF_SIZE];
        memset(last_id_c, 0, BUF_SIZE);
    get_how_much_id (file_name_to_DB.c_str(), last_id_c);
    int lis = atoi(const_cast<const char*>(last_id_c));
    
    //std::cout << "lai = " << lai << std::endl;
    //std::cout << "lis = " << lis << std::endl;
    
    // Проверка на то, нужно ли обнулять ключи всем пользователям (это если за время отстутствия произошло обнуление глобального блокчейна)
    if (lai > lis)
        {
            //std::cout << "lai > lis" << std::endl;
            // Обнуление ключей всех пользователей
            set_last_analysed_id_in_blockchain(file_name_to_DB.c_str(), 1);
            set_last_available_id_in_blockchain(file_name_to_DB.c_str(), 1);
            key_seek_zero_for_all_users();
        }





		analyse_blockchain_for_input_messages(PATH_Date_Base);
    	sleep(1);
		printf("%s","Обновление локального блокчейна по запросу произведено успешно.\n");
		

    	//printf("%s\n","\n\tРзработчик принял решение, что использование данной функции пользователем нерационально.\n");	
		sleep(2);
		continue;
	    }


	if (answer == "9")
            {
		// Обновление окна
		write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 9]\n");
		continue;
	    }

        if (answer == "10")
            {
            
            	/*
            	 write_to_log_file (LOG_FILE_NAME, "Meessanger [функция 10]\n");

            	 printf("closing_sockets begin...\n");
            	 for (int i=0; i < MAX_count_of_users; i++)
            	 {
            	 	socket_send_message[i]->close();
            	 	socket_hash_and_blockchain[i]->close();	
            	 }
				 printf("closing_sockets end...\n");

				 printf("stopForwardingAll begin...\n");
            	 session_1.stopForwardingAll();
            	 session_2.stopForwardingAll();
            	 session_client_1.stopForwardingAll();
            	 session_client_2.stopForwardingAll();
            	 printf("stopForwardingAll end...\n");


            	 */
            	 // Зашифровываем файл БД перед уходом (если выйти из программы НЕ таким способом, то все данные, полученные после последнего запуска БД будут утеряны!)
            	 // Поэтому, обязательно выходить правильно из программы
            	 std::string path_DB_str (PATH_Date_Base);
            	 std::string path_enc_DB_str  = path_DB_str + ".enc";
            	 encrypt_file (PATH_Date_Base, path_enc_DB_str.c_str(), password.c_str());   
            	 
            	 // Удаляем открытый файл БД (чтобы не висел там нигде)
            	 std::string string_programm  = "rm " + path_DB_str;
            	 char* ptr; // для совместимости
		 call_Linux_programm(string_programm.c_str(),ptr);
		 
		 // Удаляем файл действительных IP-адресов (потому, что когда мы зайдем в след.раз, то он уже будет другой)
            	 std::string string_programm1  = "rm user_list_client.txt";
            	 char* ptr1; // для совместимости
		 //call_Linux_programm(string_programm1.c_str(),ptr1);
            	 
            	 
            	 
            	 int cl = 0;
		int cl_1 = 0;
	
		for(cl; cl<height_square + 5 ; cl++)
			{
				for(cl_1; cl_1 < width_square + 40; cl_1++)
					{
						set_cursor_position (cl, cl_1);
						printf("%s"," ");
					
					
					}
				cl_1 = 0;	
			}
	
		fflush (stdout);
		set_cursor_position (0, 0);
		fflush (stdout);
         	 set_color(32);
                std::cout << "Работа программы будет завершена без ошибки. " << std::endl;
                return 0; 
            }
 

}
	}

//--------------------(04.08.21)--------------------------------




// --------------------- Вывод списка моих пользователей с количеством непрочитанных сообщений --- END ----------------------



} // Конец while(1)




// Last

// --------------------- Закрытие всех серверов и клиентов --- BEGIN ----------------------

	// ================================================================================================================================================
	// ================================================================================================================================================
	    
 	// --- Закрытие отдельного потока для периодической отправки сообщений на ЦС и получение акутального списка доступных IP-адресов в сети --- BEGIN ---
 		if (use_i2p == "FALSE")
 		{
 		int result2 = pthread_join(thread1, NULL);
   		if (result2 != 0) 
   			{
     				perror("Joining the func_say_hello_and_get_user_list_function thread");
     				printf("%s\n","Ошибка при закрытии потока thread1");
     				return EXIT_FAILURE;
   			}
   		}
   		else
   		{

   		}		
 	// --- Закрытие отдельного потока для периодической отправки сообщений на ЦС и получение акутального списка доступных IP-адресов в сети --- END --- 
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================
 	
 	 // --- Закрытие отдельного потока для отправки hash-ей последних транзакций и локальной копии блокчейна --- BEGIN ---
 	 if (use_i2p == "FALSE")
 		{
		int result3 = pthread_join(thread2, NULL);
   		if (result3 != 0) 
   			{
     				perror("Joining the hash_and_blockchain_server_part_function thread");
     				printf("%s\n","Ошибка при закрытии потока thread2");
     				return EXIT_FAILURE;
   			}
   		}
   		else
   		{
   			// ...
   		}	
 	// --- Закрытие отдельного потока для отправки hash-ей последних транзакций и локальной копии блокчейна --- END --- 
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================
 	
 	// --- Закрытие отдельного потока для участия в алгоритме консенсуса (обработке запросов send_message) и формирования локальной копии блокчейна --- BEGIN ---
 	if (use_i2p == "FALSE")
 		{
 		int result4 = pthread_join(thread3, NULL);
   		if (result4 != 0) 
   			{
     				perror("Joining the func_send_message_server_part_function thread");
     				printf("%s\n","Ошибка при закрытии потока thread3");
     				return EXIT_FAILURE;
   			}	
 	  	}
 	  	else
 	  	{
 	  		// ...
 	  	}
 	// --- Закрытие отдельного потока для участия в алгоритме консенсуса (обработке запросов send_message) и формирования локальной копии блокчейна --- END --- 
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================

	// --- Закрытие отдельного потока для периодического обновления локального блокчейна --- BEGIN ---
 	if (use_i2p == "FALSE")
 		{
 		int result44 = pthread_join(thread4, NULL);
   		if (result44 != 0) 
   			{
     				perror("Joining the periodically_get_actually_blockchain_copy_function thread");
     				printf("%s\n","Ошибка при закрытии потока thread4");
     				return EXIT_FAILURE;
   			}	
 	  }
 	  else
 	  {
 	  	// ...
 	  }
 	// --- Закрытие отдельного потока для периодического обновления локального блокчейна --- END --- 
 	
 	// ================================================================================================================================================
 	// ================================================================================================================================================
 	
// --------------------- Закрытие всех серверов и клиентов --- END ----------------------





    //std::cout << "\nКОНЕЦ_ПРОГРАММЫ." << std::endl;
    return 0;
}
