#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include<string.h>
#include <fcntl.h>


#if 0 // Grep using regexes
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_regerror (int errcode, size_t length, regex_t *compiled);

int
main (int argc, char *argv[])
{
  regex_t regex;
  int result;

  if (argc < 3)
    {
      // The number of passed arguments is lower than the number of
      // expected arguments.
      fputs ("Missing command line arguments\n", stderr);
      return EXIT_FAILURE;
    }

  result = regcomp (&regex, argv[1], REG_EXTENDED);
  if (result)
    {
      // Any value different from 0 means it was not possible to 
      // compile the regular expression, either for memory problems
      // or problems with the regular expression syntax.
      if (result == REG_ESPACE)
        fprintf (stderr, "%s\n", strerror(ENOMEM));
      else
        fputs ("Syntax error in the regular expression passed as first argument\n", stderr);
      return EXIT_FAILURE;               
    }
  for (int i = 2; i < argc; i++)
    {
      result = regexec (&regex, argv[i], 0, NULL, 0);
      if (!result)
        {
          printf ("'%s' matches the regular expression\n", argv[i]);
        }
      else if (result == REG_NOMATCH)
        {
          printf ("'%s' doesn't the regular expression\n", argv[i]);
        }
      else
        {
          // The function returned an error; print the string 
          // describing it.
          // Get the size of the buffer required for the error message.
          size_t length = regerror (result, &regex, NULL, 0);
          print_regerror (result, length, &regex);       
          return EXIT_FAILURE;
        }
    }

  /* Free the memory allocated from regcomp(). */
  regfree (&regex);
  return EXIT_SUCCESS;
}

void
print_regerror (int errcode, size_t length, regex_t *compiled)
{
  char buffer[length];
  (void) regerror (errcode, compiled, buffer, length);
  fprintf(stderr, "Regex match failed: %s\n", buffer);
}

#endif


void match_pattern(char *argv[])
{
    int fd,r,j=0, lineNo = 0;
    char temp,line[100];
    FILE *fp = fopen( argv[2], "r");
    while( fgets( line, sizeof(line), fp ) ) {
       lineNo++;
       if(strstr(line,argv[1])!=NULL)
          printf("%s:%d %s\n",argv[2], lineNo, line);
       memset(line,'\0',sizeof(line));
    }
    fclose( fp );
//    if((fd=open(argv[2],O_RDONLY)) != -1)
//    {
//       while((r=read(fd,&temp,sizeof(char)))!= 0)
//        {
//            if(temp!='\n')
//            {
//                line[j]=temp;
//                j++;
//            }
//            else
//            {
//                line[j]='\0';
//                if(strstr(line,argv[1])!=NULL)
//                    printf("%s\n",line);
//                memset(line,'\0',sizeof(line));
//                j=0;
//            }
//
//        }
//    }   
}

int
main(int argc,char *argv[])
{
    struct stat stt;
    if(argc==3)
    {
        if(stat(argv[2],&stt)==0)
            match_pattern(argv);
        else 
        {
            perror("stat()");
            exit(1);
        }
    }
    return 0;
}
