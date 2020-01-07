#include <sys/types.h>
#include <sys/stat.h>
#include <array>
#include <memory>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
const unsigned long long num = 1000000;

std::string exec(std::string cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        std::cout << "Error. Pipe does not exist!\n";
        exit(1);
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void limit_memory(long memory_in_bytes, const char* string2){
  std::cout << "Entering limit memory function\n";
  std::string string = std::to_string(memory_in_bytes);
  std::string command = std::string("bash -c \"echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.limit_in_bytes\"");
  std::cout << "Command: " << command << std::endl;
  std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  std::cout << "Meminfo: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.stat")) << std::endl;
  int return_code = system(command.c_str());
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup memory " << string << " Code: " << return_code << "\n";
    std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  }
  std::cout << "Limiting cgroup memory: " << string << " bytes\n";
}

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

  if (argc < 2){
    std::cout << "Insufficient arguments!\n";
    exit(1);
  }

  if ((fdout = open ("nullbytes", O_RDWR, mode )) < 0){
    printf ("can't create %s for writing\n", argv[1]);
    return 0;
  }

  limit_memory(std::stol(argv[1])*1024*1024,"cache-test");

  std::string test = repeat("Hello World!\n",100);
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
  if ((dst = (char*)mmap (0,  10000000000, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (char*)MAP_FAILED){
       printf ("mmap error for output with code");
       return 0;
  }

    for (int j = 0; j < 10; j++){
      for (unsigned long long i = 0; i < num ; i++){
        //std::cout << "i: " << i << std::endl;
        if (i % 100000 == 0){
          std::cout << "i: " << i << std::endl;
        }
        memcpy(dst+i*length,test.c_str(),length);
      }

      if (j == 5){
        limit_memory(10000000,"cache-test");
        //exec(std::string("bash -c \" sync && echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure\" "));
      }

    }
    return 0;

} /* main */
