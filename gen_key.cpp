#include <iostream>
#include <memory.h>

//#define KEY_SIZE 1000


int write_to_file_name_c (const char* filename, char* buf, int size)
{



   // Переменная, в которую будет сохранен указатель
   // на управляющую таблицу открываемого потока данных
   FILE *mf;
     
   printf ("Открытие файла: ");
    
   // Открытие файла
   mf=fopen (filename,"w");

   // Проверка открытия файла
   if (mf == NULL) printf ("ошибка\n");
   else printf ("выполнено\n");
 
   //Запись данных в файл
   fwrite(buf, 1, size, mf);
   printf ("Запись в файл выполнена\n");

   // Закрытие файла
   fclose (mf);
   
   
   printf ("Файл закрыт\n");
   
   return 0;
}


int main(int argc, char* argv[])
{


	if (argc < 3) 
    	{
         	fprintf(stderr,"usage: %s <key_size> <output_filename>\n", argv[0]);
         	return EXIT_FAILURE;
     	}

	int KEY_SIZE = atoi(argv[1]);
	const char* filename = argv[2];


 	srand(time(0)); // инициализация генератора случайных чисел

	char key_buf[KEY_SIZE];
	memset(key_buf,0,KEY_SIZE);





		
	int i = 0;
	
	for(i;i<KEY_SIZE;i++)
		{
		
			// Формируем случайное число от -128 до 127 (диапозон значений "char")
   			   char random_symbol_ascii = -128 + rand() % 127; // получаем целое случайное число от -128 до 127 (так нужно для алгоритма)	
   			   key_buf[i] = random_symbol_ascii;
			
		}
	
	
	write_to_file_name_c (filename, key_buf, KEY_SIZE);
	
	printf("%s%s%s\n","Файл-ключ ", filename, " записан.");

	return 0;
}
