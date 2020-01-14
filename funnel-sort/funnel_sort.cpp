#include "funnelSort.h"
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

class Int
{
	public:
		int val;
    Int(int i = 0): val(i) {}
};

class Integer_comparator
{
	public:
		bool operator () (const Int& a, const Int& b) const
		{
			return (a.val < b.val);
		}
};

void limit_memory(long memory_in_bytes, const char* string2){
  std::cout << "Entering limit memory function\n";
  std::string string = std::to_string(memory_in_bytes);
  std::string command = std::string("bash -c \"echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.limit_in_bytes\"");
  std::cout << "Command: " << command << std::endl;
  int return_code = system(command.c_str());
  //std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup memory " << string << " Code: " << return_code << "\n";
    std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  }
  std::cout << "Limiting cgroup memory: " << string << " bytes\n";
}


int main(){
	srand (time(NULL));

	if (argc < 2){
		std::cout << "Insufficient arguments! Usage: ./a.out <memory_limit> <cgroup_name>\n";
		exit(1);
	}

	int fdout;

	if ((fdout = open ("nullbytes", O_RDWR, 0x0777 )) < 0){
		printf ("can't create nullbytes for writing\n");
		return 0;
	}

  const unsigned long long num_elements = 100000000;

	Int* array;
	if (((array = (Int*) mmap(0, sizeof(Int)*num_elements, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (Int*)MAP_FAILED)){
			 printf ("mmap error for output with code");
			 return 0;
	 }

	Integer_comparator comp;
  for (unsigned long long i = 0; i < num_elements; i++){
    //records[i] = Integer(57-i);
		array[i] = Int(rand() % 1000);
		//std::cout << array[i].val << "\t";
  }
	std::cout << "\n==================================================================\n";
	FunnelSort::sort<class Int, class Integer_comparator>(&array[0], &array[num_elements], comp);
  /*for (int i = 0; i < num_elements; i++){
    std::cout << array[i].val << "\t";
  }*/
	std::cout << std::endl;
	std::cout << "Size of record: " << sizeof(Int) << std::endl;
  return 0;
}
