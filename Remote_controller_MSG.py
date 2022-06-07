import fcntl
import time

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

while (True):
	command = input('Enter command: ')
	write_with_blocking(command)
	print('Command written to file succesfull')
	print('-----------------------------------\n')
	
