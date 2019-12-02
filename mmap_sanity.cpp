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

//concatenates string s with itself n times
std::string repeat(std::string s, int n)
{
  std::string s1 = s;
  for (int i=1; i<n;i++)
    s += s1;
  return s;
}

void err_quit(const char *msg)
{
  printf("%s",msg);
  exit(1);
}

int main (int argc, char *argv[])
{
  int fdout;
  char *dst;
  int mode = 0x0777;

  if (argc != 2)
  err_quit (("usage: a.out nullbytes"));

  if ((fdout = open (argv[1], O_RDWR, mode )) < 0){
    printf ("can't create %s for writing\n", argv[1]);
    return 0;
  }

  std::string test = repeat("Hello World!\n",1000);
  const int length = strlen(test.c_str());

  /*
  if ((dst = (char*)mmap (0,  length*num+num, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == (char*)-1){
      printf ("mmap error for output");
      return 0;
  }*/
   std::cout << "Num: " << num << std::endl;
   std::cout << "Length: " << length << std::endl;
 std::cout << "Total size: " << length*num << std::endl;
 /*  if ((dst = (char*)mmap (0,  length*num+num, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == (char*)MAP_FAILED){
      printf ("mmap error for output with code %d", dst);
      return 0;
  }
  */
  if ((dst = (char*)mmap (0,  length*num+num, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED){
       printf ("mmap error for output with code");
       return 0;
  }

  for (unsigned long long i = 0; i < num ; i++){
    //std::cout << "i: " << i << std::endl;
    if (i % 1000 == 0){
      std::cout << "i: " << i << std::endl;
    }
    memcpy(dst+i*length,test.c_str(),length);
  }
  return 0;

} /* main */
