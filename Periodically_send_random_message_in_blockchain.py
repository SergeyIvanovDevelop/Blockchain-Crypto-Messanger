import fcntl
import time
import sys
from secrets import randbelow
import random

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


def main():
	if len(sys.argv) < 2:
		print("Usage: python3 Peridically_send_random_message_in_blockcgain <pseudo_user_name>")
		print("Notice: <pseudo_user_name> must be added in programm !!!")
		exit(1)
	pseudo_user_name = sys.argv[1]
	while (True):
		command = "2"
		write_with_blocking(command)
		time.sleep(3)
		command = pseudo_user_name
		write_with_blocking(command)
		time.sleep(3)
		# Длина псевдо-сообщения
		lenght = random.randint(1, 100) #randbelow(1, 100)
		ascii_arr = ""
		for i in range(0, lenght):
			# Наиболее распространенные ASCII-символы
			rand_ascii = random.randint(48, 122) #randbelow(48, 122)
			ascii_arr += chr(rand_ascii)
		# Для наглядности	
		message = ascii_arr
		print('new random message = ', message)
		command = message
		write_with_blocking(command)
		time.sleep(3)
		# Timeout между отправкой сообщений
		rand_timeout_in_seconds = random.randint(60, 200) #randbelow(60, 200)
		time.sleep(rand_timeout_in_seconds)
		
# Executive part
main()
	
