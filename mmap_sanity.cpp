#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
const unsigned long long num = 1000000;

std::string repeat(std::string s, int n)
{
    // Copying given string to temparory string.
    std::string s1 = s;

    for (int i=1; i<n;i++)
        s += s1; // Concatinating strings
    return s;
}

void err_quit(const char *msg)
{
    printf("%s",msg);
    exit(1);
}

int main (int argc, char *argv[])
{
 int fdin, fdout;
 char *dst;
 struct stat statbuf;
 int mode = 0x0777;

 if (argc != 2)
   err_quit (("usage: a.out <tofile>"));

 /* open the input file */
 /*
 if ((fdin = open (argv[1], O_RDONLY)) < 0)
   {printf("can't open %s for reading", argv[1]);
    return 0;
   }
   */

 /* open/create the output file */
 if ((fdout = open (argv[1], O_RDWR, mode )) < 0)//edited here
   {printf ("can't create %s for writing\n", argv[1]);
    return 0;
   }

 /* find size of input file */
 /*
 if (fstat (fdin,&statbuf) < 0)
   {printf ("fstat error");
    return 0;
   }
  */

 /* go to the location corresponding to the last byte */
 /*if (lseek (fdout, strlen(test.c_str()) , SEEK_SET) == -1)
   {printf ("lseek error");
    return 0;
   }

 /* write a dummy byte at the last location */
 /*
 if (write (fdout, "", 1) != 1)
   {printf ("write error");
     return 0;
   }

 /*
 if ((src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0))
   == (void*) -1)
   {printf ("mmap error for input");
    return 0;
   }
   */

 /* mmap the output file */

   std::string test = repeat("Hello World!\n",1000);
  const int length = strlen(test.c_str());
  if ((dst = (char*)mmap (0,  length*num+num, PROT_READ | PROT_WRITE,
   MAP_SHARED, fdout, 0)) == (char*)-1)
   {printf ("mmap error for output");
    return 0;
   }



  for (unsigned long long i = 0; i < num ; i++){
    //std::cout << "i: " << i << std::endl;
    if (i % 1000 == 0){
          std::cout << "i: " << i << std::endl;
    }
    memcpy(dst+i*length,test.c_str(),length);
  }


 /* this copies the input file to the output file */
/*
 memcpy (dst+4, test.c_str(),  strlen(test.c_str()));
 */
 return 0;

} /* main */
