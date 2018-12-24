#include <string.h>
#include <aio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

#define FILE_SIZE 60 //source file size initialize sets to 60 byte

struct packet   // required to send arguments together to each process
{
    int offset;
    int size;
    int tid;
};

char *source_path, *dest_path;
pthread_t *threads;

void createFiles();   //create source file with randomly content and creating destination file
void takePaths(char *source, char *dest, char *s, char *d);  //to take path for files
void *readAndWrite(void *packet);  //Reading from source file and writing to destinatio file


int main(int argc, char *argv[])
{
	
	int numberofThread, i;
    unsigned long sizeofThread;
    struct packet *packetforThread;
    
	source_path = (char*) malloc(150 * sizeof (char));
	dest_path = (char*) malloc(150 * sizeof (char));

	if (argc != 4)  //if user give wrong inputs 
    {
        printf(RED "Wrong input, i.e: ./2013510105.c - - 8 \n" RESET);
        return 0;
    }

    takePaths(argv[1], argv[2], source_path, dest_path); // function to take file paths and sets variables
    createFiles();  //function to create source.txt with randomly content
    
    numberofThread = atof(argv[3]);  //taking thread count from terminal
    
    threads = (pthread_t*) malloc(numberofThread * sizeof (pthread_t)); //allocate for threads

    sizeofThread = FILE_SIZE / numberofThread; //size of each thread processing
  
    for (i = 0; i < numberofThread; i++) //creating packet contents
    {
        packetforThread[i].size = sizeofThread;
        packetforThread[i].offset = sizeofThread * i;  //initializing position for reading from each thread
        packetforThread[i].tid = i;

        if ((FILE_SIZE % numberofThread) > 0 && (i == numberofThread - 1)){         // if the file size is not exactly divided by the number of threads,                                                              
            packetforThread[i].size = sizeofThread + (FILE_SIZE % numberofThread);  //then assign the rest to the last thread.
        }

        //printf("%d. thread has the task of reading %d bytes \n", i+1, packetforThread[i].size);
    }

    for (i = 0; i < numberofThread; i++){
        pthread_create(threads + i, NULL, readAndWrite, (void *)&packetforThread[i]);
    }
    for (i = 0; i < numberofThread; i++){
        pthread_join(*(threads + i), NULL);
    }

    printf("\n");
    printf("Writing is complated successfully. The files are ready \n");

    free(source_path);  //empty memory
    free(dest_path);       

return 0;
}



void *readAndWrite(void *packet){

struct packet *this = packet; //void packet unwrapping into this

int s_fd, d_fd, err, ret;  
struct aiocb aio_r, aio_w;

char data[this->size+1];  

s_fd = open(source_path, O_RDONLY);
    if (s_fd == -1) {
        perror("open"); }

    memset(&aio_r, 0, sizeof(aio_r));
    memset(&data, 0, sizeof(data));
    aio_r.aio_fildes = s_fd;
    aio_r.aio_buf = data;
    aio_r.aio_offset = this->offset;
    aio_r.aio_nbytes = this->size;
    
    aio_r.aio_lio_opcode = LIO_READ;

    if (aio_read(&aio_r) == -1) {
        printf("Error at aio_read(): %s\n",
        strerror(errno));
        exit(2);
    }
    
    while (aio_error(&aio_r) == EINPROGRESS) {  }
    err = aio_error(&aio_r);
    ret = aio_return(&aio_r);

    if (err != 0) {
        printf ("Error at aio_error() : %s\n", strerror (err));
        close (s_fd);
        exit(2);
    }

    double percent = ((double)this->size / (double)FILE_SIZE ) *100;  //Calculating percentage each thread
    
    printf(BLU "%d. "RESET"thread, readed " YEL "%%%2.1f" RESET" data (" RED "%d byte" RESET ") from source file\n", this->tid + 1, percent, this->size);
    


d_fd = open(dest_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (d_fd == -1) {
        perror("open");
    }

    memset(&aio_w, 0, sizeof(aio_w));
    aio_w.aio_fildes = d_fd;
    aio_w.aio_buf = data;
    aio_w.aio_offset = this->offset;
    aio_w.aio_nbytes = this->size;
    if (aio_write(&aio_w) == -1) {
        printf(" Error at aio_write(): %s\n", strerror(errno));
        close(d_fd);
        exit(2);
    }
    while (aio_error(&aio_w) == EINPROGRESS) { }
    err = aio_error(&aio_w);
    ret = aio_return(&aio_w);

    if (err != 0) {
        printf ("Error at aio_error() : %s\n", strerror (err));
        close (d_fd);
        exit(2);
    }
   
    close(s_fd);
    close(d_fd);
}

void takePaths(char *source, char *dest, char *s, char *d){
	char cwd[100];
	if (strcmp(source,"-")==0)
	{
   		getcwd(cwd, sizeof(cwd));  //take the currently direction
   		strcpy(s, cwd);
        strcat(s, "/source.txt");  //path and name castcade
	} else{
		strcpy(s, source);           // take the direction that given by user
        strcat(s, "/source.txt");
	}

	if (strcmp(dest,"-")==0)
	{
		getcwd(cwd, sizeof(cwd));
   		strcpy(d, cwd);
        strcat(d, "/dest.txt");
	}else{
		strcpy(d, dest);
        strcat(d, "/dest.txt");
	}
	
}

void createFiles(){

int size=FILE_SIZE;
char *str =(char*)malloc(size + 1);
int i;
srand(time(NULL));
const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    
for (i = 0; i < size; i++) {     // creating randomly contents
    int key = rand() % (int) (sizeof(charset)-1);
    str[i] = charset[key];
}
str[size] = '\0';

int s_fd = creat(source_path, O_RDWR);   // creating source file
write(s_fd, str, FILE_SIZE);
close(s_fd);
chmod(source_path, S_IREAD | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); //changing permissions for the file
printf("Source file was created successfully \n");
printf("\n");

int d_fd = creat(dest_path, O_RDWR);  //creating destination file
close(d_fd);
chmod(dest_path, S_IREAD | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); //changing permissions for the file

free(str);
}