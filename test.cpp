#include <iostream>
#include <stxxl/vector>
#include <vector>
#include <string>
#include <stdlib.h>
#include <unistd.h>
const int B = 64;

#define TYPE int
const int CACHE = 128; //pages per cache_adaptive
const int PAGE_SIZE = 1024; //blocks per page
const int BLOCK_SIZE_IN_BYTES = 16384; //block size in bytes
const stxxl::uint64 length = 32000;
typedef stxxl::VECTOR_GENERATOR<TYPE,PAGE_SIZE,CACHE,BLOCK_SIZE_IN_BYTES>::result vector_type;
typedef stxxl::vector<TYPE, PAGE_SIZE, stxxl::lru_pager<CACHE>,BLOCK_SIZE_IN_BYTES>::iterator itr;
//vector_type array;

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

std::vector<std::string> split(std::string mystring, std::string delimiter)
{
    std::vector<std::string> subStringList;
    std::string token;
    while (true)
    {
        size_t findfirst = mystring.find_first_of(delimiter);
        if (findfirst == std::string::npos) //find_first_of returns npos if it couldn't find the delimiter anymore
        {
            subStringList.push_back(mystring); //push back the final piece of mystring
            return subStringList;
        }
        token = mystring.substr(0, mystring.find_first_of(delimiter));
        mystring = mystring.substr(mystring.find_first_of(delimiter) + 1);
        subStringList.push_back(token);
    }
    return subStringList;
}

void print_io_data(std::vector<long long>& data, std::string header){
  std::cout << header;
  std::string command = std::string("cat /proc/") + std::to_string(getpid()) + std::string("/io");
  std::string command_output = exec(command);
  std::vector<std::string> splitted_output = split(command_output, " ");
  for (unsigned int i = 0; i < splitted_output.size(); ++i){
    if ( splitted_output[i].find(std::string("read_bytes:")) != std::string::npos) {
      std::cout << "Bytes read: " << (std::stoll(splitted_output[i+1]) - data[0]);
      data[0] = std::stoll(splitted_output[i+1]);
    }else if ( splitted_output[i].find(std::string("write_bytes:")) != std::string::npos) {
      std::cout << ", Bytes written: " << (std::stoll(splitted_output[i+1]) - data[1]) << "\n\n";
      data[1] = std::stoll(splitted_output[i+1]);
      break;
    }
  }
}

void limit_memory(const char* string1, const char* string2){
  unsigned long memory_in_bytes = (unsigned long)atof(string1)*1024*1024;
  std::string string = std::to_string(memory_in_bytes);
  std::string command = std::string("echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.limit_in_bytes");
  int return_code = system(command.c_str());
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup memory " << string << "\n";
    exit(1);
  }
  std::cout << "Limiting cgroup memory: " << string << " bytes\n";
  /*string = std::to_string(memory_in_bytes + 500*1024*1024);
  command = std::string("echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.memsw.limit_in_bytes");
  return_code = system(command.c_str());
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup swap memory " << string << "\n";
    exit(1);
  }
  std::cout << "Limiting cgroup swap memory: " << string << " bytes\n";
  */
}

int main(int argc, char *argv[]){
  if (argc < 3){
    std::cout << "Insufficient arguments! Usage: cgroup_cache_adaptive <memory_limit> <cgroup_name>\n";
    exit(1);
  }
    std::vector<long long> io_stats = {0,0};
    {
      vector_type array;
    //MODIFY MEMORY WITH CGROUP
    limit_memory(argv[1],argv[2]);
    print_io_data(io_stats, "Printing I/O statistics at program start @@@@@ \n");
  	std::cout << "running test program with matrices of size: " << (int)length << "x" << (int)length << "\n";
    stxxl::timer start_p1;
  	start_p1.start();

  	stxxl::stats* Stats = stxxl::stats::get_instance();
  	stxxl::stats_data stats_begin(*Stats);
  	stxxl::block_manager * bm = stxxl::block_manager::get_instance();

      for (stxxl::uint64 i = 0; i < length*length; i++)
      {
        array.push_back(i);
      }
      std::cout << "done setting up array\n";
      print_io_data(io_stats, "Printing I/O statistics after loading array @@@@@ \n");


      for (stxxl::uint64 i = 0; i < length*length; i++)
      {
        array[i] = i;
      }

        print_io_data(io_stats, "Printing I/O statistics after first write @@@@@ \n");


      for (stxxl::uint64 i = 1; i < length*length; i++)
      {
        array[i] = array[i-1]+1;
      }

  	  start_p1.stop();

  	STXXL_MSG("[LOG] IO Statistics for sorting: "<<(stxxl::stats_data(*Stats) - stats_begin));
  	STXXL_MSG("[LOG] Max MB allocated:  " << bm->get_maximum_allocation()/(1024*1024));
  	std::cout << "[LOG] Total loading time: " <<(start_p1.mseconds()/1000) << "\n";

    print_io_data(io_stats, "Printing I/O statistics after second write @@@@@ \n");

    std::cout << "final value " << array[7848*7848] << std::endl;
  }
  print_io_data(io_stats, "Printing I/O statistics after leaving block scope @@@@@ \n");
  return 0;
}
