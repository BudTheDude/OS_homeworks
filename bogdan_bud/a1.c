#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_PATH_LEN 257

#define __DEBUG
#ifdef __DEBUG
void debug_info (const char *file, const char *function, const int line)
{
        fprintf(stderr, "DEBUG. ERROR PLACE: File=\"%s\", Function=\"%s\", Line=\"%d\"\n", file, function, line);
}

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
        debug_info(__FILE__, __FUNCTION__, __LINE__); \
}

#else                   // with no __DEBUG just displays the error message

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
}

#endif


typedef struct head{
	char name[100];
	unsigned int type;
	unsigned int offset;
 	unsigned int size;
	
	}head;





void listDir(char *dirName,int rec,int perm,int ends_with,char *search,int list)
{
    DIR* dir;
    int ok=1;
    int ok2=1;
    struct dirent *dirEntry;
    char name[MAX_PATH_LEN];
    struct stat inode;
    dir = opendir(dirName);
    if (dir == 0) {
        ERR_MSG ("ERROR\ninvalid directory path");
        exit(4);
    }
    


    // iterate the directory contents
    while ((dirEntry=readdir(dir)) != 0) {
        // build the complete path to the element in the directory
        snprintf(name, MAX_PATH_LEN, "%s/%s",dirName,dirEntry->d_name);
        ok2=1;
	ok=1;
       
	 if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!=0){
         lstat (name, &inode);
	
	if(ends_with==1){
  	   int l=strlen(name);
	   int k=strlen(search);
	   if(strcmp(name+l-k,search)!=0)
		ok2=0;
		
           
	}
	if(perm==1){
	 if (!(inode.st_mode & S_IWUSR))
		ok=0;
	} 
	if(rec==1){
        if (S_ISDIR(inode.st_mode))
		listDir(name,rec,perm,ends_with,search,list);
	}
		
	 if(ok==1 && ok2==1){
		 int ok3=1;
		 
		 if(list==1){
			    int fd;
				if((fd=open(name, O_RDONLY))<0)
				printf("ii rau");
				char mag[30];
				read(fd,mag,2);
				
				if(strcmp(mag,"0t")!=0){
					ok3=0;
					}
				
			
				int headSize=0;
				read(fd,&headSize,2);
				
			
				int version=0;
				read(fd,&version,4);
				
				if(version>193 || version<73){
					ok3=0;
					}
			
				int nrOfSections=0;
				
					
				read(fd,&nrOfSections,1);
				if(nrOfSections>14 || nrOfSections<2){
					ok3=0;
					}
				head h;
				for(int i=0;i<nrOfSections;i++){
				
					h.type=0;
					h.offset=0;
					h.size=0;
					read(fd,h.name,7);
					read(fd,&h.type,1);
					read(fd,&h.offset,4);
					read(fd,&h.size,4);
					if(h.type!=18 && h.type!=37 && h.type!=21 && h.type!=14 && h.type!=65 && h.type!=70 && h.type!=96){
						ok3=0;
					}
					if(h.size>965)
						ok3=0;
				
			}
			 
			 
			 }
			if(ok3==1){
            printf(" %s", dirName);
            printf("/%s\n", dirEntry->d_name);}
	}
	 
	}
    }

    closedir(dir);
}


int main(int argc, char **argv){
    
    
    int fd;
	int rec=0;
	int perm=0;
	int ends_with=0;
	int list=0;
	char *search; 
	search="0";
	struct stat fileMetadata;

	if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("45784\n");
        }
 	
	if(strcmp(argv[1], "list") == 0){

	for(int i=2;i<argc;i++)
	{
		
		if(i!=argc-1){
		if(strcmp(argv[i],"recursive")==0)
		   rec=1;
		if(strcmp(argv[i],"has_perm_write")==0)
		   perm=1;
		if(strstr(argv[i],"name_ends_with=")!=NULL)
		   {
			ends_with=1;
			search=argv[i]+15;
                   }
			}

		if(i==argc-1){
			argv[i]=argv[i]+5;
		
		 if (stat(argv[i], &fileMetadata) < 0) {  
   		 }
        	if (S_ISDIR(fileMetadata.st_mode)) {
	
        	printf("SUCCESS\n");
        	listDir(argv[i],rec,perm,ends_with,search,list);
    		} 
		
		}
		
	}
	
        }
        
         if(strcmp(argv[1],"parse")==0){
			
			int check=1;
			char *reason;
			argv[2]+=5;
			
			if((fd=open(argv[2], O_RDONLY))<0)
				printf("ii rau");
			
			char mag[300];
			read(fd,mag,2);
			
			if(check==1){
				if(strcmp(mag,"0t")!=0){
					check=0;
					reason="magic";
					}
				}
			
			unsigned int headSize=0;
			read(fd,&headSize,2);
			
			
				
			unsigned int version=0;
			read(fd,&version,4);
			
			if(check==1){
				if(version>193 || version<73){
					check=0;
					reason="version";
					}
				}
				
			
			
			unsigned int nrOfSections=0;
			read(fd,&nrOfSections,1);
			
				if(check==1){
				if(nrOfSections>14 || nrOfSections<2){
					check=0;
					reason="sect_nr";
					}
				}
			
			head h[nrOfSections];
			unsigned int i;
			for(i=0;i<nrOfSections;i++){
				h[i].type=0;
				h[i].offset=0;
				h[i].size=0;
				read(fd,h[i].name,7);
				read(fd,&h[i].type,1);
				read(fd,&h[i].offset,4);
				read(fd,&h[i].size,4);
				
				if(check==1){
				if(h[i].type!=18 && h[i].type!=37 && h[i].type!=21 && h[i].type!=14 && h[i].type!=65 && h[i].type!=70 && h[i].type!=96){
					check=0;
					reason="sect_types";
					break;
					}
				}
				
				}
			
			if(check==1){
			printf("SUCCESS\n");
			printf("version=%d\n",version);
			printf("nr_sections=%d\n",nrOfSections);
			
			for(i=0;i<nrOfSections;i++){
				printf("section%d: ",i+1);
				printf("%s ",h[i].name);
				printf("%d ",h[i].type);
				printf("%d\n",h[i].size);
				}
			}
			if(check==0)
			{
				printf("ERROR\n");
				printf("wrong %s",reason);
				}
			}
			
		
		if(strcmp(argv[1],"extract")==0 ){
			
			argv[2]+=5;
			argv[3]+=8;
			int section=atoi(argv[3]);
			argv[4]+=5;
			int line_to_find=atoi(argv[4]);
			
			if((fd=open(argv[2], O_RDONLY))<0)
				printf("ii rau");
			
			char mag[30];
			read(fd,mag,2);
			
			int headSize=0;
			read(fd,&headSize,2);
			
			int version=0;
			read(fd,&version,4);
			
			int nrOfSections=0;
			read(fd,&nrOfSections,1);
			head h;
			for(int i=0;i<section;i++){
				
				h.type=0;
				h.offset=0;
				h.size=0;
				read(fd,h.name,7);
				read(fd,&h.type,1);
				read(fd,&h.offset,4);
				read(fd,&h.size,4);
				
			}
			
			lseek(fd, h.offset, SEEK_SET);
			
			
			char printable[h.size];
			read(fd,printable,h.size);
				
				
			
			
		
		int k=0;
		char found[200000];
		int t=0;
		for(int i=h.size-1;i>=0;i--)
		{
			if(printable[i]=='\n')
				k++;
			if(k==line_to_find-1)
				found[t++]=printable[i];
			if(k==line_to_find)
				break;
			}
			
			printf("SUCCESS\n");
			for(int i=t-1;i>=0;i--)
				printf("%c",found[i]);
		
		}
		
		if(strcmp(argv[1],"findall")==0){
			argv[2]+=5;
			list=1;
			printf("SUCCESS\n");
			listDir(argv[2],1,0,0,"no search",list);
		
		
		}
		
    }
    return 0;
}

    


