default: i2psam gen_key net db link

base64: base64.cpp base64.h
	g++ -c base64.cpp -o base64.o

i2psam: i2psam.cpp i2psam.h
	g++ -c i2psam.cpp -o i2psam.o

gen_key: gen_key.cpp
	g++ -c gen_key.cpp -o gen_key

net: netserver.c C_part.c C_part.h 
	gcc -c netserver.c -o netserver -lpthread -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-elide-constructors
	gcc -c C_part.c -o C_part.o -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls

db: Cpp_part.cpp Messenger.cpp Cpp_part.h
	g++ -c Cpp_part.cpp -o Cpp_part.o -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls
	g++ -c Messenger.cpp -o Messenger.o -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls
	
link: C_part.o Cpp_part.o Messenger.o base64.o
	g++ C_part.o Cpp_part.o Messenger.o i2psam.o base64.o -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -lasan -lsqlite3 -lpthread -lssl -lcrypto -lboost_system -lcrypto++ -o Messenger
	
clean:
	cp key_2.txt m3.txt
	cp key_2.txt m3.txt_2
	cp key_2.txt m5.txt
	cp key_2.txt m5.txt_2
	cp m4.txt send.txt
	cp m1_2.txt m1.txt
	cp m2_2.txt m2.txt
	cp m3_2.txt m3.txt
	cp m4_2.txt m4.txt
	cp m5_2.txt m5.txt
	rm *.o *.db *.enc Messenger netserver gen_key | true
	rm *.backup | true
	rm -rf OUTPUT_FILES/ | true
	rm -rf INPUT_FILES/ | true
	rm -rf BIG_FILES_OUTPUT/ | true
	rm -rf BIG_FILES_INPUT/ | true
	rm -rf TEMP_DIR/ | true
	rm actual_blockchain.txt | true
	rm from_client_hash_blockchain_*.txt | true
	rm data_filename.txt | true
	rm from_client_get_hash_blockchain_*.txt | true
	rm from_client_send_message_*.txt | true
	rm buf_file.txt | true
	rm blockchain_buf_filename.txt | true
	#rm user_list.txt | true
	#rm user_list_client.txt | true
	rm log_file.txt | true
			
