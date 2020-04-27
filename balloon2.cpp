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
#include<vector>

const unsigned long long mem = 200*1024*1024;
unsigned long long TARGET_MEMORY = 0;
unsigned long long CGROUP_MEMORY = 0;
int NUM_BALLOONS = 0;
int BALLOON_ID = 0;
int MATRIXWIDTH = 0;
int memory_profile_type = 0;
unsigned long long MEMORY = 1;
std::chrono::system_clock::time_point t_start;
std::vector<long> times = std::vector<long>(); //this is an array containing all the timestamps in a memory profile
std::vector<long> memory_values = std::vector<long>(); //this is an array containing all the memory values in a memory profile

/*this program accepts as input the total cgroup memory, the number of balloons being run, and the target memory we wish
to use in our main program. Everything is in bytes.
Note that at a minimum, the main algorithm will use at least C/(B+1) memory.
*/

unsigned long long set_memory_in_bytes(unsigned long cgroup_memory, unsigned long target_memory, int num_balloons){
  if (TARGET_MEMORY == 0 || CGROUP_MEMORY == 0 || NUM_BALLOONS == 0){
    std::cout << "Invalid value! Error!\n";
    exit(1);
  }
  if (cgroup_memory > target_memory){
    long result = (cgroup_memory - target_memory)/num_balloons - 256*1024; //this is a magic number, don't question it
    return result >= 0 ? result : 0;
  }
  else{
    return 50; //another magic number, do not question
  }
}


int main(int argc, char *argv[]){
  std::cout << "Starting balloon program" << std::endl;
  if (argc < 7){
    std::cout << "Insufficient arguments! Usage: balloon.cpp <memory_profile_type>"
    "<cgroup_memory> <memory_profile_file>/<desired_memory> <num_balloons> <balloon_id> <matrixwidth>\n";
    exit(1);
  }
  CGROUP_MEMORY = atol(argv[2])*1024*1024;
  NUM_BALLOONS = atoi(argv[4]);
  BALLOON_ID = atoi(argv[5]);
  MATRIXWIDTH = atoi(argv[6]);
  int fdout;
  std::string filename = "balloon_data" + std::to_string(BALLOON_ID);
  std::cout << "Ballon data filename: " << filename << std::endl;
  if ((fdout = open (filename.c_str(), O_RDWR, 0x0777 )) < 0){
    printf ("can't create nullbytes for writing\n");
    return 0;
  }

  int ipcfd;
  if ((ipcfd = open ("IPCTEST", O_RDWR, 0x0777 )) < 0){
    printf ("can't create file for writing\n");
    return 0;
  }

  unsigned long* dst2;
  if (((dst2 = (unsigned long*) mmap(0, 10 , PROT_READ | PROT_WRITE, MAP_SHARED , ipcfd, 0)) == (unsigned long*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }

  memory_profile_type = atoi(argv[1]);
  TARGET_MEMORY = atol(argv[3])*1024*1024;
  std::ifstream input_mem_profile;
  std::string mem_profile_filename = "mem_profile" + std::to_string(MATRIXWIDTH) + ".txt";
  std::cout << "Mem profile filename: " << mem_profile_filename << std::endl;
  if (memory_profile_type == 1){
    std::cout << "Worst-case memory\n";
    input_mem_profile = std::ifstream(mem_profile_filename.c_str());
    bool timestamp = true;
    double value;
    while (input_mem_profile >> value){
      std::cout << "Value: " << value << std::endl;
      if (timestamp){
        std::cout << "Vector size: " << times.size() << std::endl;
        times.push_back((long)value);
      }else{
        memory_values.push_back((long)value);
      }
      timestamp =  !timestamp;
      std::cout << "End of while loop\n";
    }
    input_mem_profile.close();
    for (unsigned int i = 0; i < times.size(); i++){
      std::cout << "Time: " << times[i] << " Memory: " << memory_values[i] << std::endl;
    }
  }

  MEMORY = set_memory_in_bytes(CGROUP_MEMORY, TARGET_MEMORY, NUM_BALLOONS);

  std::cout << "Each balloon is mmapping " << MEMORY << std::endl;
  std::cout << "cgroup memory " << CGROUP_MEMORY  << std::endl;
  std::cout << "num balloons " <<   NUM_BALLOONS  << std::endl;
  char* dst;
  if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }

  t_start = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point current = std::chrono::system_clock::now();
  unsigned long index = 0;
  unsigned int counter = 0;
  while(true){
    index %= (MEMORY);
    *(dst + rand()%MEMORY) = 1;
    *(dst + index) = 1;
    index += 1000;
    std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();
    auto wall_time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    if (memory_profile_type == 1){
      if (counter < times.size() && wall_time > times[counter]){
        std::cout << "value stored in times array: " << times[counter] << std::endl;
        munmap(dst,MEMORY);
        MEMORY = set_memory_in_bytes(CGROUP_MEMORY, memory_values[counter], NUM_BALLOONS);
        std::cout << "Changing memory to " << MEMORY << std::endl;
        if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
             printf ("mmap error for output with code");
             return 0;
        }
        std::cout << "Done changing memory " << MEMORY << std::endl;
        counter++;
      }
    }else if (memory_profile_type == 2){
      if (std::chrono::duration<double, std::milli>(std::chrono::system_clock::now()-current).count() > 15000){
        current = std::chrono::system_clock::now();
        munmap(dst,MEMORY);
        if (MEMORY == 50){
          MEMORY = set_memory_in_bytes(CGROUP_MEMORY, TARGET_MEMORY, NUM_BALLOONS);
        }else{
          MEMORY = 50;
        }
        std::cout << "Changing memory to " << MEMORY << std::endl;
        if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
             printf ("mmap error for output with code");
             return 0;
        }
        std::cout << "Done changing memory " << MEMORY << std::endl;
      }
    }else if (memory_profile_type == 3){
      //std::cout << "dst2[0]: " << dst2[0] << std::endl;
      if (dst2[0] != 0 && MEMORY != set_memory_in_bytes(CGROUP_MEMORY, dst2[0], NUM_BALLOONS)){
        munmap(dst,MEMORY);
        MEMORY = set_memory_in_bytes(CGROUP_MEMORY, dst2[0], NUM_BALLOONS);
        std::cout << "Changing memory to " << MEMORY << std::endl;
        if (((dst = (char*) mmap(0, MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED)){
             printf ("mmap error for output with code");
             return 0;
        }
        std::cout << "Done changing memory " << MEMORY << std::endl;
      }

    }
  }

  std::cout << "Balloon done executing." << std::endl;
}
