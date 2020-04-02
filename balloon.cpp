/* This is a simple balloon program that alternates its memory usage between 50MB and 100MB
every 5 seconds.
*/
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<fstream>

const unsigned long long MEMORY_VAL = 500*1024*1024;
const unsigned long long MEMORY_VAL_2 = 50;
unsigned long long MEMORY = MEMORY_VAL_2;
std::chrono::system_clock::time_point t_start;

int main(){
  std::cout << "Starting balloon" << std::endl;

  int fdout;

  if ((fdout = open ("balloon_data", O_RDWR, 0x0777 )) < 0){
    printf ("can't create nullbytes for writing\n");
    return 0;
  }

  char* dst;
  if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }
  bool flag = true;
  unsigned long long i = 0;

  t_start = std::chrono::system_clock::now();

  while(flag){
    *(dst + rand()%MEMORY) = 1;

    std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();
  	auto wall_time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    if (wall_time > 60000){
      t_start = std::chrono::system_clock::now();
      std::cout << "Five seconds have passed. Changing memory." << std::endl;
      munmap(dst,MEMORY);
      if (MEMORY == MEMORY_VAL){
        MEMORY = MEMORY_VAL_2;
      }else{
        MEMORY = MEMORY_VAL;
      }
      if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
           printf ("mmap error for output with code");
           return 0;
      }
    }
  }
  std::cout << "Balloon done executing." << std::endl;
}
