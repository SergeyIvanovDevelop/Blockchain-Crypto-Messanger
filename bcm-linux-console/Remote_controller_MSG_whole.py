import fcntl
import time
import socket  
import sys
import os

def encrypt_file_Vername_alg(Path_to_Img_File, Path_to_Key_File):
	file = open(Path_to_Img_File, "rb")
	Img_Data = file.read()
	file.close()
	Length_Img_Data = len(Img_Data)
	key_file_size = os.path.getsize(Path_to_Key_File)
	if (key_file_size < Length_Img_Data):
		print('Оставшегося ключа не хватает для шифрования файла:')
		print('\tkey_file_size (bytes) = ', key_file_size)
		print('\tLength_Img_Data (bytes) = ', Length_Img_Data)
		return -1
	file = open(Path_to_Key_File, "rb")
	Key_Data = file.read()
	file.close()
	encrypt_data = [0] * Length_Img_Data
	Img_Data = list(Img_Data)
	Key_Data = list(Key_Data)
	print('Length_Img_Data = ', Length_Img_Data)
	for i in range(0, len(encrypt_data)):
		encrypt_data[i] = Img_Data[i] ^ Key_Data[i]
	encrypt_data = bytes(encrypt_data)
	file = open(Path_to_Img_File + "_encrypted", "wb")
	file.write(encrypt_data)
	file.close()
	New_Key = Key_Data[Length_Img_Data:]
	New_Key = bytes(New_Key)
	file = open(Path_to_Key_File, "wb")
	file.write(New_Key)
	file.close()
	print('Key File update')
	print('encrypt_data = ', encrypt_data)
	print('File_encrypted')
	return 0


def decrypt_file_Vername_alg(Path_to_Img_File_Enc, Path_to_Key_File):
	file = open(Path_to_Img_File_Enc, "rb")
	Img_Data_Enc = file.read()
	file.close()
	Length_Img_Data_Enc = len(Img_Data_Enc)
	key_file_size = os.path.getsize(Path_to_Key_File)
	print('key_file_size = ', key_file_size)
	if (key_file_size < Length_Img_Data_Enc):
		print('Оставшегося ключа не хватает для расшифрования файла:')
		print('\tkey_file_size (bytes) = ', key_file_size)
		print('\tLength_Img_Data_Enc (bytes) = ', Length_Img_Data_Enc)
		return -1
	file = open(Path_to_Key_File, "rb")
	Key_Data = file.read()
	file.close()
	Img_Data_Enc = list(Img_Data_Enc)
	Key_Data = list(Key_Data)
	decrypt_data = [0] * Length_Img_Data_Enc
	for i in range(0, len(decrypt_data)):
		decrypt_data[i] = Img_Data_Enc[i] ^ Key_Data[i]
	decrypt_data = bytes(decrypt_data)
	file = open(Path_to_Img_File_Enc[:len(Path_to_Img_File_Enc)-14] + "_decrypted.jpg", "wb")
	file.write(decrypt_data)
	file.close()
	New_Key = bytes(Key_Data[Length_Img_Data_Enc:])
	file = open(Path_to_Key_File, "wb")
	file.write(New_Key)
	file.close()
	print('Key File update')
	print('decrypt_data = ', decrypt_data)
	print('File_decrypted')
	return 0



def decrypt_bytes(Path_to_Key_File, Bytes):
	Bytes_len = len(Bytes)
	key_file_size = os.path.getsize(Path_to_Key_File)
	print('key_file_size = ', key_file_size)
	if (key_file_size < Bytes_len):
		print('Оставшегося ключа не хватает для расшифрования массива байт:')
		print('\tkey_file_size (bytes) = ', key_file_size)
		print('\tBytes_len (bytes) = ', Bytes_len)
		return Bytes
	file = open(Path_to_Key_File, "rb")
	Key_Data = file.read()
	file.close()
	Bytes = list(Bytes)
	Key_Data = list(Key_Data)
	decrypt_data = [0] * Bytes_len
	for i in range(0, len(decrypt_data)):
		decrypt_data[i] = Bytes[i] ^ Key_Data[i]
	decrypt_data = bytes(decrypt_data)
	New_Key = bytes(Key_Data[Bytes_len:])
	file = open(Path_to_Key_File, "wb")
	file.write(New_Key)
	file.close()
	print('Key File update')
	print('decrypt_data = ', decrypt_data)
	print('Bytes_decrypted')
	return decrypt_data


def write_with_blocking(data_str, filename = "input_control.txt"):
	file = open(filename, "r+")
	file = open(filename, "w+")
	print('File OPEN')
	fcntl.flock(file.fileno(), fcntl.LOCK_EX)
	print('File LOCK')
	file.write(data_str)
	#fcntl.flock(file.fileno(), fcntl.LOCK_UN)
	file.close()
	print('File UNLOCK and CLOSE')

def SendFile(client_sock, filename):
    # ...
    print("Send file to Android")
    # Sending file to Android Application
    bytes_read_1 = open(filename, "rb")
    bytes_read = bytes_read_1.read()
    bytes_read_1.close()
    bytes_read_1 = open(filename, "rb")
        
    size=len(bytes_read)
    client_sock.send(size.to_bytes(4,'big'))
    #client_sock.sendall();
    #time.sleep(3)
        
    while True:
        data = bytes_read_1.read(1024)
        if data:
            client_sock.send(data)
            #client_sock.sendall(data);
            print(data)
        else : break    
    bytes_read_1.close()    


def MakeScreenshot():
	os.system("sudo fbgrab-master/fbgrab -c 1 Screenshot.jpg")
	print('Made Screenshot!')


def main():
	IP = sys.argv[1]
	Port = int(sys.argv[2])
	s = socket.socket()   
	s.bind((IP, Port))
	Port = Port  
	s.listen(10)  #clients permitted connect
	print ("server run")
	
	while (True):
		sc, addr = s.accept()
		print ("client_connected")
		#a = input('wait:')
		enc_recibido_bytes = sc.recv(1024)
		recibido_bytes = decrypt_bytes('Key.txt', enc_recibido_bytes)
		#if recibido_bytes == enc_recibido_bytes:
		#	print('При расшифровке массива байт произошла ошибка')
		#	sc.close()
		#	continue
		print('recibido_bytes (withoiut_UTF-8): ', recibido_bytes) 
		recibido = recibido_bytes.decode("utf-8") 
		print ("received:", recibido)
		print('len_recv = ', len(recibido))
		for i in range(0, len(recibido)):
			print("["+str(i)+"] = ", end = " ")
			print(recibido[i])
		print ("|" + recibido + "|")
		if recibido == 'quit':  
			break
		if recibido == 'IMG_GET':
			MakeScreenshot()			
			encrypt_file_Vername_alg('Screenshot.jpg', 'Key.txt')
			print('yyy')
			filename = 'Screenshot.jpg_encrypted' #"ScreenShot.jpg"  
			SendFile(sc, filename)
			sc.close()
			continue
		write_with_blocking(recibido)		
		#if len(recibido) != 0:
		#	print("data_len_rec = ", len(recibido)) 
		#	a = input('wait:')
		#sc.send(recibido)
		sc.close()  
	print ("bye")
	
	s.close()

# Executive part
main()


'''
while (True):
	command = input('Enter command: ')
	write_with_blocking(command)
	print('Command written to file succesfull')
	print('-----------------------------------\n')
'''
	
