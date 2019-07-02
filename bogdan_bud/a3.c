#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>



int main(int argc, char **argv){
	
	
	
	int fdReq;
    int fdResp;

    char respPipe[] = "RESP_PIPE_45784";
    char reqPipe[] = "REQ_PIPE_45784";
    
    
    int fifo = mkfifo(respPipe, 0777);
    if(fifo < 0) {
        perror("ERROR\ncannot create the response pipe");
        exit(1);
    }

   
    fdReq = open(reqPipe, O_RDONLY);
    if(fdReq < 0) {
        perror("ERROR\ncannot open the request pipe");
        exit(1);
    }

    
    fdResp = open(respPipe, O_WRONLY);
    if(fdResp < 0) {
        perror("ERROR\ncannot open the response pipe");
        exit(1);
    }

    char msg[] = " CONNECT";
    msg[0] = 7;
    write(fdResp, msg, 8);

		
	while(1){
		
		int file_desc;
		char *data;
		int size;
		
		unsigned int mem_space;
		char *sharedChar ;
		int shmId;
		
		unsigned char count;
        read(fdReq, &count, 1);

        char buf[100];
        read(fdReq, &buf, count);
        buf[count] = 0;
        
		if(strcmp(buf,"PING")==0)
		{	
			char c=4;
			unsigned int nr=45784;
			
			write(fdResp,&c,1);
			write(fdResp,"PING",4);
			write(fdResp,&c,1);
			write(fdResp,"PONG",4);
						
			write(fdResp, &nr, sizeof(nr));
			
		}
		else if(strcmp(buf,"CREATE_SHM")==0){
			
			read(fdReq, &mem_space, sizeof(unsigned int));
			
			shmId = shmget(17143, mem_space, IPC_CREAT | 0664);
			sharedChar=(char*)shmat(shmId, NULL, 0);
			if(shmId < 0) {
				char c=10;
				write(fdResp,&c,1);
				write(fdResp,"CREATE_SHM",10);
				c=5;
				write(fdResp,&c,1);
				write(fdResp,"ERROR",5);
				return 1;
				}
			else{
				
				char c=10;
				write(fdResp,&c,1);
				write(fdResp,"CREATE_SHM",10);
				c=7;
				write(fdResp,&c,1);
				write(fdResp,"SUCCESS",7);
					
			}
			
		}
		else if(strcmp(buf,"WRITE_TO_SHM")==0){
			unsigned int offset;
			unsigned int value;
			read(fdReq, &offset, sizeof(unsigned int));
			read(fdReq, &value, sizeof(unsigned int));
			
			if(offset>0 && offset<=mem_space-sizeof(unsigned int)){
			memcpy(sharedChar+offset,&value,sizeof(value));
			
			char c=12;
			write(fdResp,&c,1);
			write(fdResp,"WRITE_TO_SHM",12);
			c=7;
			write(fdResp,&c,1);
			write(fdResp,"SUCCESS",7);
			}
			else{
				char c=12;
				write(fdResp,&c,1);
				write(fdResp,"WRITE_TO_SHM",12);
				c=5;
				write(fdResp,&c,1);
				write(fdResp,"ERROR",5);
				return 1;
				}
				
				
	
			
		}else if(strcmp(buf,"MAP_FILE")==0){
			
			
			unsigned char c;
			read(fdReq, &c, 1);

			char file[100];
			read(fdReq, &file, c);
			
			file[c] = 0;
			
			file_desc = open(file, O_RDONLY);
			size = lseek(file_desc, 0, SEEK_END);
			data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, file_desc, 0);
			
			if(file_desc!=-1 && data!=(void*)-1){
				
				c=8;
				write(fdResp,&c,1);
				write(fdResp,"MAP_FILE",8);
				c=7;
				write(fdResp,&c,1);
				write(fdResp,"SUCCESS",7);
				
				
				
			}else{
				c=8;
				write(fdResp,&c,1);
				write(fdResp,"MAP_FILE",8);
				c=5;
				write(fdResp,&c,1);
				write(fdResp,"ERROR",5);
			}
				
				
				
			
			
			}else if(strcmp(buf,"READ_FROM_FILE_OFFSET")==0){
			
					unsigned int offset;
					unsigned int no_of_bytes;
					read(fdReq, &offset, sizeof(unsigned int));
					read(fdReq, &no_of_bytes, sizeof(unsigned int));
			
					if(offset + no_of_bytes>size) {
						char c=21;
						write(fdResp,&c,1);
						write(fdResp,"READ_FROM_FILE_OFFSET",21);
						c=5;
						write(fdResp,&c,1);
						write(fdResp,"ERROR",5);
						
					}else{
				
						
						
						for(int i=0;i<=no_of_bytes;i++){
							*(sharedChar + i) = data[i + offset];
						}
				
						char c=21;
						write(fdResp,&c,1);
						write(fdResp,"READ_FROM_FILE_OFFSET",21);
						c=7;
						write(fdResp,&c,1);
						write(fdResp,"SUCCESS",7);
			
					}
		
			
		}else if(strcmp(buf,"READ_FROM_FILE_SECTION")==0){
			
				unsigned int section_no;
				unsigned int offset;
				unsigned int no_of_bytes;
				read(fdReq, &section_no, sizeof(section_no));
				read(fdReq, &offset, sizeof(offset));
				read(fdReq, &no_of_bytes, sizeof(no_of_bytes));
				
				
				
				
				unsigned int sectionSize;
				memcpy(&sectionSize, data+(8+(section_no - 1) * 16 +13),sizeof(sectionSize));
				
				unsigned int sectionOffset;
				memcpy(&sectionOffset, data+(8+(section_no - 1) * 16 +9),sizeof(sectionOffset));
				
				
					
				
				
				 if(section_no <= data[8] && sectionSize > (offset + no_of_bytes)) {
					 
					 						
						for(int i = 0; i < no_of_bytes; i++) {
							sharedChar[i] = data[sectionOffset + offset + i];
						}
						
						char c=22;
						write(fdResp,&c,1);
						write(fdResp,"READ_FROM_FILE_SECTION",22);
						c=7;
						write(fdResp,&c,1);
						write(fdResp,"SUCCESS",7);
						
				
				 }else{
						char c=22;
						write(fdResp,&c,1);
						write(fdResp,"READ_FROM_FILE_SECTION",22);
						c=5;
						write(fdResp,&c,1);
						write(fdResp,"ERROR",5);
						
						
					 }
			
		}
		
		else break;
		
	}
		
	return 0;
}
