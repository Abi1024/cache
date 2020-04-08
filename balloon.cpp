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

const unsigned long long MEMORY_VAL = 500*1024*1024;
const unsigned long long MEMORY_VAL_2 = 50;
unsigned long long TARGET_MEMORY = 0;
unsigned long long CGROUP_MEMORY = 0;
int NUM_BALLOONS = 0;
int BALLOON_ID = 0;
std::chrono::system_clock::time_point t_start;

/*this program accepts as input the total cgroup memory, the number of balloons being run, and the target memory we wish
to use in our main program. Everything is in bytes.
Note that at a minimum, the main algorithm will use at least C/(B+1) memory.

*/
unsigned long set_memory_in_bytes(unsigned long cgroup_memory, unsigned long target_memory, int num_balloons){
  if (TARGET_MEMORY == 0 || CGROUP_MEMORY == 0 || NUM_BALLOONS == 0){
    std::cout << "Invalid value! Error!\n";
    exit(1);
  }
  if (cgroup_memory > target_memory){
    long result = (cgroup_memory - target_memory)/num_balloons - 2*1024*1024;
    return result >= 0 ? result : 0;
  }
  else{
    return 0;
  }
}

int main(int argc, char *argv[]){
  if (argc < 6){
    std::cout << "Insufficient arguments! Usage: balloon.cpp <memory_profile_type>"
    "<cgroup_memory> <memory_profile_file>/<desired_memory> <num_balloons> <balloon_id>\n";
    exit(1);
  }
  std::cout << "Starting balloon program" << std::endl;

  int fdout;

  std::string filename = "balloon_data" + BALLOON_ID;
  if ((fdout = open (filename.c_str(), O_RDWR, 0x0777 )) < 0){
    printf ("can't create nullbytes for writing\n");
    return 0;
  }

  int memory_profile_type = atoi(argv[1]);

  if (memory_profile_type == 0){
      TARGET_MEMORY = atol(argv[3])*1024*1024;
  }else{
    std::cout << "Wrong memory profile type" << std::endl;
    exit(1);
  }

  CGROUP_MEMORY = atol(argv[2])*1024*1024;
  NUM_BALLOONS = atoi(argv[4]);
  BALLOON_ID = atoi(argv[5]);
  unsigned long long mmap_memory = set_memory_in_bytes(CGROUP_MEMORY, TARGET_MEMORY, NUM_BALLOONS);
  std::cout << "Each balloon is mmapping " << mmap_memory << std::endl;
  std::cout << "cgroup memory " << CGROUP_MEMORY  << std::endl;
  std::cout << "num balloons " <<   NUM_BALLOONS  << std::endl;
  char* dst;
  if (((dst = (char*) mmap(0, mmap_memory, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }
  bool flag = true;

  t_start = std::chrono::system_clock::now();

  while(flag){
    *(dst + rand()%mmap_memory) = 1;
    /*
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
    */
  }
  std::cout << "Balloon done executing." << std::endl;
}
