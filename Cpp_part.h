#pragma once

#include <unistd.h>
#include <ctime>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iomanip>
#include <openssl/md5.h>
#include <set>
#include <ext/stdio_filebuf.h>
#include "i2psam.h"
#include <time.h>
#include <chrono>
#include <thread>
#include <array>
#include <sys/resource.h>

#define MAX_TRANSACTION_LENGTH 1000001 //300000
#define MAX_NUMBER_OF_USERS 10 // 100
#define MAX_NUMBER_OF_MESSAGES 100
#define MAX_LEN_USER_NAME 100
#define BUF_SIZE 1000001 //300000
#define HEX_LENGTH 10000
#define HASH_LENGTH 100
#define KEY_SIZE 1000001 //300000


// For i2p ------------------
#define INT_SIZE 4
#define MAX_DESTINATION_LENGTH 1000
// For i2p ------------------


// FOR SHARED_FILES_STORE ==========================================

#include <time.h>
#include <set>
#include <cstdio>

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "crypto++/cryptlib.h"
#include "crypto++/filters.h"
#include "crypto++/files.h"
#include "crypto++/hex.h"
#include "crypto++/sha.h"

#include "crypto++/modes.h"
#include "crypto++/aes.h"


extern boost::asio::deadline_timer* timer_my;
extern boost::asio::io_context* io_my;

extern const char* ARV_FN;
extern const char* ARGV_FN;

extern std::string Big_files_dir_output;
extern std::string Big_files_dir_input;
extern std::string path_to_temp_dir;

extern int bad_recieve;
extern int gen_return;

extern int port_cs;
extern int port_sc;

enum { max_length = 1024 };

typedef std::pair <int, int>  pair_int_int;
//typedef std::pair <std::string, std::string>  pair_str_str;
//typedef std::pair <std::array<char, 100>, std::array<char, 100>>  pair_str_str;








// For i2p -------------------- BEGIN (06.09.21) ----------------------
struct i2p_socket_str {
	SAM::I2pSocket* sock;
        int number;

}; //Тут стоит точка с запятой!


struct i2p_session_str {

	std::shared_ptr<SAM::StreamSession> session_ptr;
	int number;

}; //Тут стоит точка с запятой!
// For i2p -------------------- END (06.09.21) ------------------------



// FOR SHARED_FILES_STORE ==========================================


class Reference_File
{

public:
    // Конструктор по умолчанию
    Reference_File()
    {
	
        // ...
    }

    void print()
    {
        std::cout << " ===== Reference_File =====" << std::endl;

        std::cout << "\n ==================================================================== \n" << std::endl;
        std::cout << "real_number_files = " << real_number_files << std::endl;
        std::cout << "real_files = " << real_files << std::endl;
        std::cout << "part_here_file = " << part_here_file << std::endl;
        std::cout << "size_part_here_file = " << size_part_here_file << std::endl;
        std::cout << "hash_of_all_file = " << hash_of_all_file << std::endl;
        std::cout << "key_size = " << key_size << std::endl;
        std::cout << "\n ==================================================================== \n" << std::endl;

	sleep(15);


        std::cout << "key_size = " << key_size << std::endl;
        for (int i = 0; i < key_size; i++)
        {
            std::cout << "buf_key[" << i << "] = " << buf_key[i] << std::endl;
        }
        
        sleep(5);

        std::cout << "\n ==================================================================== \n" << std::endl;

        std::cout << "iv_size = " << iv_size << std::endl;

        for (int i = 0; i < iv_size; i++)
        {
            std::cout << "buf_iv[" << i << "] = " << buf_iv[i] << std::endl;
        }
	 sleep(5);
        std::cout << "\n ==================================================================== \n" << std::endl;

        for (int i = 0; i < real_number_files; i++)
        {
            if (part_here_file == i)
            {
                continue;
            }
            std::cout << "addr_1[" << i << "] = " << addr_1[i] << std::endl;
        }
        
         sleep(5);
        
        std::cout << "\n ==================================================================== \n" << std::endl;

        for (int i = 0; i < real_number_files; i++)
        {
            if (part_here_file == i)
            {
                continue;
            }
            std::cout << "addr_2[" << i << "] = " << addr_2[i] << std::endl;
        }
         sleep(5);
        std::cout << "\n ==================================================================== \n" << std::endl;

        for (int i = 0; i < real_number_files; i++)
        {
            if (part_here_file == i)
            {
                continue;
            }
            std::cout << "addr_3[" << i << "] = " << addr_3[i] << std::endl;
        }
         sleep(5);
        std::cout << "\n ==================================================================== \n" << std::endl;

        for (int i = 0; i < real_number_files; i++)
        {
            if (part_here_file == i)
            {
                continue;
            }
            std::cout << "hash_each_file[" << i << "] = " << hash_each_file[i] << std::endl;
        }
	 sleep(5);
        std::cout << "\n ==================================================================== \n" << std::endl;
        for (int i = 0; i < real_number_files; i++)
        {
            std::cout << "switch_table[" << i << "].first = " << switch_table_first[i] << std::endl;
            std::cout << "switch_table[" << i << "].second = " << switch_table_second[i] << std::endl;
            //sleep(2);
        }

        std::cout << "\n ==================================================================== \n" << std::endl;
        for (int i = 0; i < real_number_files; i++)
        {
            if (part_here_file == i)
            {
                continue;
            }
            std::cout << "switch_table_servers_filename[" << i << "].first = " << const_cast<const char*>(&switch_table_servers_filename_first[i][0]) << std::endl;
            std::cout << "switch_table_servers_filename[" << i << "][0].second = " << const_cast<const char*>(&switch_table_servers_filename_second[i][0][0]) << std::endl;
            std::cout << "switch_table_servers_filename[" << i << "][1].second = " << const_cast<const char*>(&switch_table_servers_filename_second[i][1][0]) << std::endl;
            std::cout << "switch_table_servers_filename[" << i << "][2].second = " << const_cast<const char*>(&switch_table_servers_filename_second[i][2][0]) << std::endl;
            //sleep(2);
        }
        std::cout << "\n ==================================================================== \n" << std::endl;
        std::cout << "filename = " << filename << std::endl;

        std::cout << "\n ==================================================================== \n" << std::endl;
        std::cout << "\n ==================================================================== \n" << std::endl;

    }

    // Необходимые поля
    const static int max_number_files = 100;
    const static int max_len_hash = 255; //35;
    const static int max_len_inet_addr = 40; // <-- for both (IPv4 and IPv6) || for IPv4 only --> 16;
    const static int max_len_filename = 100;

    int real_number_files; // Сколько всего файлов
    int real_files; // Сколько реальных файлов
    int fake_files; // Сколько фэйковых файлов
    int part_here_file; // Тот мини-файл, который мы оставили в Reference_File
    int size_part_here_file; // Размер того мини-файла, который остался у нас
    char hash_of_all_file[max_len_hash];
    unsigned char buf_key[CryptoPP::AES::MAX_KEYLENGTH];
    unsigned char buf_iv[CryptoPP::AES::BLOCKSIZE];
    int key_size;
    int iv_size;
    char hash_each_file[max_number_files][max_len_hash];

    // Дублирование адресов
    char addr_1[max_number_files][max_len_inet_addr];
    char addr_2[max_number_files][max_len_inet_addr];
    char addr_3[max_number_files][max_len_inet_addr];

    static const int max_data_size_of_part_here = 1000;
    unsigned char part_here_data [max_data_size_of_part_here];

    // Таблицы замены имен
    //pair_int_int switch_table [max_number_files];
    //pair_str_str switch_table_servers_filename [max_number_files];


    // --------------
    static const int max_length_filename = 256;
    static const int max_number_servers_of_store_data = 3;
    char switch_table_servers_filename_first [max_number_files][max_length_filename];
    char switch_table_servers_filename_second [max_number_files][max_number_servers_of_store_data][max_length_filename];

    int switch_table_first [max_number_files];
    int switch_table_second [max_number_files];

    // --------------



    // Имя файла для сборки
    char filename[max_len_filename];

};




struct user_list_and_pointer 
{

	std::string user_list [MAX_NUMBER_OF_USERS];
	int pointer = 0;

};


struct user_messages_list_and_pointer 
{
	std::string list_transaction_id [MAX_NUMBER_OF_MESSAGES];
	std::string list_time_of_sending [MAX_NUMBER_OF_MESSAGES];
	std::string list_hwo_sender [MAX_NUMBER_OF_MESSAGES];
	std::string list_hwo_reciever [MAX_NUMBER_OF_MESSAGES];
	std::string list_messages [MAX_NUMBER_OF_MESSAGES];
	int pointer = 0;

};


struct transaction_struct
{
	int ID;
	std::string Sender;
	std::string Reciever;
	std::string Time_of_sending;
	std::string Decrypt_message; 
};



struct transaction_string_struct
{
	int transaction_id;
	std::string transaction_body;
	int transaction_size;
	std::string before_transaction_hash;
	std::string transaction_hash;
 
};


class Timer_Callback
{
public:
    const int* int_ptr;
    const int time_delay_sec;
    int flag;
    SAM::I2pSocket* socket_ptr;
    Timer_Callback(SAM::I2pSocket* socket_ptr, int* int_ptr, int time_delay_sec);
    ~Timer_Callback();
    void thread_func(void);
    void checkRes(void);

private:
    
};





// Прототипы функций
int Create_or_Enter_to_DB (const char* path_to_DB);
int create_table_for_new_user (const char* path_to_DB, std::string new_user_name, std::string path_to_key_file);
int return_my_user_list (const char* path_to_db, user_list_and_pointer* struct1);
int return_my_messages_with_user (const char* path_to_db, std::string user_name, user_messages_list_and_pointer* struct2);
int add_record_to_user_table(const char* path_to_db, std::string user_name, int id, std::string hwo_sender, std::string hwo_reciever, std::string time_of_sending, std::string decrypt_message);
int filesize(const char* filename);
int get_last_analysed_id_in_blockchain(const char* path_to_DB, std::string* last_analysed_id);
int set_last_analysed_id_in_blockchain(const char* path_to_DB, int id);
int try_decrypt(std::string* encrypt_mess, std::string* encrypt_mess_DEC);
int parser(std::string transaction, transaction_struct* struct_transaction);
int analyse_blockchein_for_user_name(const char* path_to_db, const char* user_name);
int get_transaction_body_where_id(const char* path_to_DB, int id, std::string* transaction_body);
int get_id_transaction(const char* path_to_DB, int id, transaction_string_struct* struct_string_transaction);
int how_many_transaction_id (const char* path_to_DB, std::string* last_id);
int read_file_into_memory(const char* filepath, char* buffer, int length);
int read_file_into_memory(const char* filepath, unsigned char* buffer, int length);
int checkFile(const char* file_name);
int blockchain_from_DB_to_file (const char* path_to_DB, int from_id, const char* path_to_buffer_file);
int write_to_file (char* buf, int size, const char* filename);
int update_new_message_count_to_user(const char* path_to_db, std::string user_name, int add_or_null);
int get_count_new_message(const char* path_to_db, std::string user_name);
const char* encrypt_text_message (const char* user_name, const char* text_message, int* res_enc);
int get_key_seek_for_user(const char* path_to_DB, const char* user_name);
int get_key_for_user(const char* path_to_DB, const char* user_name, char* key_buf);
int set_key_seek_for_user(const char* path_to_DB, const char* user_name, int current_seek);
int get_key_size_for_user(const char* path_to_DB, const char* user_name);
int update_key_file_for_user (const char* path_to_DB, const char* user_name, const char* path_to_key_file);
void encrypt_file (const char* filename_in, const char* filename_out, const char* password);
void decrypt_file (const char* filename_in, const char* filename_out, const char* password);
std::string input_password(void);
void loading_animation (int start_or_stop, int start_x, int start_y, int height, int width, int delay_us);
void square(int heigth, int width);
void user_pause (void);
long int filesize_share( FILE *fp );
std::string count_hash ( const char* filename_for_hash);
int encrypt (const char* filename_for_encrypte);
int decrypt (const char* filename_for_decrypte);
int send_file_to_server (const char* hostname, int port, const char* filename_for_sending_server);
int recieve_from_server (const char* hostname, int port, const char* filename_for_request_from_server);
void write_object_to_file(Reference_File* rf, const char* path_to_reference_file_c_str);
int read_object_from_file(Reference_File* rf, const char* path_to_reference_file_c_str);
int share_file(const char* filename_for_share_c_str, const char* filename_shared_reference_file_c_str);
int union_file(const char* filename_reference_file_c_str);
int get_last_available_id_in_blockchain(const char* path_to_DB, std::string* last_available_id);
int set_last_available_id_in_blockchain(const char* path_to_DB, int id);
void key_seek_zero_for_all_users(void);
int getline_with_block_from_file(std::string& line, std::string filename = "input_control.txt");
void * hash_and_blockchain_server_part_function_i2pd(void * arg);
void * send_message_server_part_function_i2pd(void * arg);
int get_actual_blockchain_copy_i2pd (void);
int get_hash_or_blockchain_i2pd(const char* hostname, const char* hash_or_blockchain, char* buffer);
int send_message_i2pd (const char* encrypt_message, const char* path_to_buf_file);
int read_from_file_to_buffer_1(const char* filename, char* buffer, int max_data_size);
std::string random_str(int size = 10);
int bin_file_to_string(const char* filename, std::string* string_file);
int string_to_bin_file(std::string* string_file, const char* filename);
std::string get_path();
