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



def main():
	Path_to_Img_File = sys.argv[1]
	Path_to_Key_File1 = sys.argv[2]
	Path_to_Key_File2 = sys.argv[3]
	ret_code1 = encrypt_file_Vername_alg(Path_to_Img_File, Path_to_Key_File1)
	if (ret_code1 == -1):
		print('При шифровании файла произошла ошибка')
		return
	ret_code2 = decrypt_file_Vername_alg(Path_to_Img_File+"_encrypted", Path_to_Key_File2)
	if (ret_code2 == -1):
		print('При расшифровании файла произошла ошибка')
		return
	print('Programm finished!')
	# создание файла


main()

'''
myfile = open("hello.txt", "w")

# запись в файл
res = myfile.write("Hello friends, how are you?")
print(res, "bytes written to the file.")
# закрытие файла
myfile.close()

# чтение содержимого из файла
myfile = open("hello.txt", "r")
print("file content...")
print(myfile.read())

# устанавливает текущее местоположение файла на позицию 6 
print("file content from 6th position...")
myfile.seek(6)
print(myfile.read())

# устанавливает текущее местоположение файла на позицию 0
print("file content from 0th position...")
myfile.seek(0)
print(myfile.read(4))

# устанавливает текущее местоположение файла на позицию 12
print("file content from 12th position...")
myfile.seek(12)
print(myfile.read())
'''
	
