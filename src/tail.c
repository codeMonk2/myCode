#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include<string.h>
#include <fcntl.h>
#include <sys/queue.h>

TAILQ_HEAD(tailhead, entry) head;

struct entry {
   char buf[100];
   TAILQ_ENTRY( entry )
}

void saveLine( void **head, char *line ) {
   if (isFull( *head ) ) {
      dequeue( *head );
   }
   enqueue( *head, line );
}

int main(int argc, char **argv)
{
   FILE *fp = fopen( argv[1], "r" );
   if (!fp ) return -1;
   char line[100] = {'\0'};
   while( fgets( line, 100, fp ) ) {
      printf("%s\n", line );
      memset( line, '\0', 100 );
   }
   fclose( fp );
}
