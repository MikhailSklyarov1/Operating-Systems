#include "stdafx.h"
#include <stdio.h> 
#include <conio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <direct.h> 
#include <process.h> 

void error_message(char* message);
void pack(char* filename, bool remove_source);
void unpack(char* zipname);
void crypt(char*filename);
void uncrypt(char*filename, bool show_file);

void main(int argc,char* argv[]){
	char filename[100];
	char zipname[100];

	if(argc<2)
	{	// Не указано никаких парметров
		error_message("Unknown command");
	}
	else
		if (!strcmp(argv[1], "-p"))
		{ //если указан аргумент -p 
			if (argc < 3) //есть ли название файла
				error_message("few parameters");
			else
				if (!strcmp(argv[2], "-r")) // указан ли аргумент -r
				{
					if (argc < 4)	// есть ли название файла
					{
						error_message("few parameters");
					}
					else
					{ // Запаковка с удалением исходного файла
						strcpy(filename, argv[3]);
						pack(filename, true);
					}
				}
				else
				{	// Запаковка файла
					strcpy(filename,argv[2]);
					pack(filename, false);
				}
		}
		else if(!strcmp(argv[1],"-u"))
		{ //если указан аругмент -u
			if(argc < 3)	// указан ли файл
				error_message("few parameters");
			else // Распаковка
				strcpy(zipname,argv[2]);
			unpack(zipname);
		}
		else if(!strcmp(argv[1], "-c"))
		{ //если указан аругмент -с
			if(argc < 3) // указан ли файл
				error_message("few parameters");
			else // Шифровка
				strcpy(filename, argv[2]);
			crypt(filename);
			puts("\nDone.");
		}
		else if(!strcmp(argv[1], "-d"))
		{ //если указан аругмент -d
			if(argc < 3) // указан ли файл
				error_message("few parameters");
			else
				if(!strcmp(argv[2],"-m")) //если указан аругмент -m
					if(argc < 4) // указан ли файл
						error_message("few parameters");
					else
					{ // Расшифровка
						strcpy(filename, argv[3]);
						uncrypt(filename, true);
					}
				else
				{ // Расшифровка
					strcpy(filename,argv[2]);
					uncrypt(filename, false);
				}
			puts("\nDone.");
		}
		else
		{
			error_message("Unknown parameter");
		}
}

void error_message(char* message){
	printf("%s\n",message);
	getch();
	exit(0);
}

void pack(char* filename, bool remove_source){
	char	strong_filename[1000],
			strong_zipname[1000],
			cur_dir[1000],
			param1[1000],
			param2[1000];
	_getcwd(cur_dir,1000);
	// vvedeno ne absolutnoe imia faila, nado ego preobrazovat'
	if(filename[1]!=':'){
		strcpy(strong_filename,cur_dir);
		strcat(strong_filename,"\\");
	}
	else
	{
		strong_filename[0] = 0;
	}

	strcat(strong_filename,filename);
	//imia archiva - toze no s rasshireniem .zip
	strcpy(strong_zipname,strong_filename);
	char* dot=strchr(strong_zipname,'.');

	if (dot != NULL)
	{
		strcpy(dot, ".zip");
	}
	else
	{
		strcat(strong_zipname, ".zip");
	}
	//parametri komandnoi stroki
	sprintf(param2,"\"%s\"",strong_filename);
	sprintf(param1,"\"%s\"",strong_zipname);
	//zashifrovat'
	crypt(strong_filename);
	//zapakovat'

	if (_spawnlp(_P_WAIT, "C:\\Program Files (x86)\\7-Zip\\7z.exe", "7z.exe", "a", param1, param2, NULL) == -1) {
		printf("\nError with packing file\n");
	}

	uncrypt(strong_filename, false);
	//удалить исходный файл, если задан флаг.
	if (remove_source)
	{
		remove(strong_filename);
		printf("udalilos'");
	}
	printf("Done_puck %s %s", param1, param2);
	getch();
}

void unpack(char* zipname){
	//raspakovat'
	char	strong_zipname[1000],
			filename[1000],
			cur_dir[1000],
			param1[1000];
	_getcwd(cur_dir,1000);
	// vvedeno ne absolutnoe imia faila, nado ego preobrazovat'
	if(zipname[1]!=':'){
		strcpy(strong_zipname,cur_dir);
		strcat(strong_zipname,"\\");
	}
	else
		strong_zipname[0]=0;
	strcat(strong_zipname,zipname);

	sprintf(param1,"\"%s\"",strong_zipname);
	//raspakovat' v tekushuu papku (flag 'x')
	spawnlp(_P_WAIT, "C:\\Program Files (x86)\\7-Zip\\7z.exe", "7z.exe","x",param1,NULL);
	char* dot=strchr(strong_zipname,'.');
	if(dot!=NULL)
		strcpy(dot,".txt");
	else
		strcat(strong_zipname,".txt");

	//rasshifrovat'
	uncrypt(strong_zipname, false);
	printf("Done");
	getch();
}

void crypt(char* filename){
	char env_var[100];
	int crypt_key = 1;
	if(getenv("CRYPT_KEY") != NULL)
	{
		strcpy(env_var, getenv("CRYPT_KEY"));
		crypt_key = atoi(env_var);
	}

	FILE* in = fopen(filename,"r");
	FILE* out = fopen(filename,"r+");
	if(in==NULL || out==NULL)
		error_message("File doesn't exist");
	char cur;
	while(fread((void*)&cur,sizeof(char),1,in)!=0){
		cur += crypt_key;
		fwrite((void*)&cur,sizeof(char),1,out);
	}
	fclose(in);
	fclose(out);
}

void uncrypt(char* filename, bool show_file){
	char env_var[100];
	int crypt_key = 1;
	if(getenv("CRYPT_KEY") != NULL)
	{
		strcpy(env_var, getenv("CRYPT_KEY"));
		crypt_key = atoi(env_var);
	}

	FILE* in = fopen(filename,"r");
	FILE* out = fopen(filename,"r+");
	if(in==NULL || out==NULL)
		error_message("File doesn't exist");
	if(show_file)
		puts("Uncrypted File:");
	char cur;
	while(fread((void*)&cur,sizeof(char),1,in)!=0){
		cur -= crypt_key;
		if(show_file)
			printf("%c", cur);
		fwrite((void*)&cur,sizeof(char),1,out);
	}
	fclose(in);
	fclose(out);
}
