/* This balloon program allows one to change the memory of a program.
*/
#include<iostream>
#include<chrono>
#include<cstdlib>
#include<thread>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<string>
#include<fcntl.h>
#include<fstream>

const unsigned long long MEMORY_VAL = 100*1024*1024;
const unsigned long long MEMORY_VAL_2 = 50;
unsigned long long MEMORY = MEMORY_VAL;
std::chrono::system_clock::time_point t_start;

/*this program accepts as input the total cgroup memory, the number of balloons being run, and the target memory we wish
to use in our main program. Everything is in bytes.
Note that at a minimum, the main algorithm will use at least C/(B+1) memory.

*/
int main(){
  std::cout << "Starting balloon program" << std::endl;

  int fdout;

  if ((fdout = open ("balloon_data", O_RDWR, 0x0777 )) < 0){
    printf ("can't create file for writing\n");
    return 0;
  }

  char* dst;
  if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }

  t_start = std::chrono::system_clock::now();

  while(true){
    *(dst + rand()%MEMORY) = 1;
    std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();
  	auto wall_time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    if (wall_time > 120000){
      t_start = std::chrono::system_clock::now();
      munmap(dst,MEMORY);
      if (MEMORY == MEMORY_VAL){
        MEMORY = MEMORY_VAL_2;
      }else{
        MEMORY = MEMORY_VAL;
      }
      std::cout << "Changing memory to " << MEMORY << std::endl;
      if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
           printf ("mmap error for output with code");
           return 0;
      }
      std::cout << "Done changing memory " << MEMORY << std::endl;
    }
  }
  std::cout << "Balloon done executing." << std::endl;
}
