#pragma once

#include <stdio.h>  
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <semaphore.h> 
#include <memory.h>
#include <time.h>
#include <sqlite3.h>


#define BUF_SIZE 1000001 //300000
#define HASH_LENGTH 100
#define MAX 10 // 100
#define HEX_LENGTH 10000
#define ANSWER_LENGTH 1000001 //300000 // ~10кБ (чтобы если что вместилось несколько блоков)
#define MAX_TRANSACTION_LENGTH 1000001 //300000
#define MAX_PORT_ADDRESS_LENGTH 100




extern void* func_say_hello_and_get_user_list_function(void*);

struct port_and_path_to_DB {
	int port;
	const char* path_to_db;

}; //Тут стоит точка с запятой!


struct str_thread {
    struct sockaddr_in x;
    struct sockaddr_in z;
    int y;
    const char* path_to_db;
}; //Тут стоит точка с запятой!


struct IP_and_PORT {

	const char* ip;
	int port;
}; //Тут стоит точка с запятой!


struct path_to_DB_and_port {

	const char* path_to_db;
	int port;
};




// Функции из С файла

int write_to_file_c (char* buf, int size);
int write_to_file_name_c (char* buf, int size, const char* filename);
int read_from_file_to_buffer(const char* filename, char* buffer);
void create_empty_file(const char* filename);
void call_Linux_programm(const char* programm_name, char* res_buf); // на данный момент не используется, но оставлю - вдруг понадобиться
int64_t getFileSize(const char* file_name);
void get_hash_id_container (const char* path_to_DB, const char* id_str, char* hash_last_id_container);
void get_how_much_id (const char* path_to_DB, char* last_id_str);
int get_transaction_body_where_id_c(const char* path_to_DB, const char* id_str, char* transaction_body_id);
void get_hash_id_container (const char* path_to_DB, const char* id_str, char* hash_id_container);
int send_message (const char* path_to_db, const char* encrypt_message, int port, const char* path_to_buf_file);
int check_id_hash_server_part(const char* path_to_db, int number_port);
int get_actual_blockchain_copy (int port_number);
//void run_send_message_server_part_function(int port_number);
int get_hash_or_blockchain(const char* hostname, int port_number, const char* hash_or_blockchain, char* buffer);
int blockchain_from_DB_to_file_c (const char* path_to_DB, int from_id, const char* path_to_buffer_file, char* buffer);
int blockchain_from_DB_to_file_c_correct (const char* path_to_DB, int from_id, const char* path_to_buffer_file, char* buffer);
void * func_say_hello_and_get_user_list_function(void* arg);
void * hash_and_blockchain_server_part_function(void * arg);
void * func_send_message_server_part_function(void * arg);
void * periodically_get_actually_blockchain_copy_function(void * arg);
int periodically_get_actually_blockchain_copy(int number_port, const char* path_to_db);
void int_to_unsigned_char_array_4 (unsigned int digit, unsigned char* uc_int);
int unsigned_char_array_4_to_int (unsigned char* uc_int);
int write_to_log_file (const char* filename, const char* message);


// Функции из C++ файла

int analyse_blockchain_for_input_messages(const char* path_to_DB);
int add_record_to_local_blockchain(const char* path_to_db, int id, const char* encrypt_message_c, const char* hesh_before_c, const char* hesh_this_c);
int Clear_Blockchain_Table(const char* path_to_db);
int blockchain_from_file_to_DB (const char* path_to_DB, const char* path_to_buffer_file);
void get_hash_from_string (const char* string, char* result_string);
void check_clear_blockchain(void);
void set_cursor_position (int x, int y);
void set_color(int color_number);

