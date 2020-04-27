#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<iostream>

int main(){
  int fdout;
  if ((fdout = open ("IPCTEST", O_RDWR, 0x0777 )) < 0){
    printf ("can't create file for writing\n");
    return 0;
  }
  char* dst;
  if (((dst = (char*) mmap(0, 10, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }
  while (true){
    if (dst[0] == 'd'){
      std::cout << "Changed to d\n";
    }
  }
  return 0;
}
