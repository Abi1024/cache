#pragma once
#include<string>
#include<sys/types.h>
#include<iostream>
#include<memory>
#include<vector>
#include<unistd.h>


namespace CacheHelper{
  int MM_BASE_SIZE = 128;
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

  void print_io_data(std::vector<long>& data, std::string header){
    std::cout << header;
    std::string command = std::string("cat /proc/") + std::to_string(getpid()) + std::string("/io");
    std::string command_output = exec(command);
    std::vector<std::string> splitted_output = split(command_output, " ");
    for (unsigned int i = 0; i < splitted_output.size(); ++i){
      if ( splitted_output[i].find(std::string("read_bytes:")) != std::string::npos) {
        std::cout << "Bytes read: " << (std::stol(splitted_output[i+1]) - data[0]);
        data[0] = std::stol(splitted_output[i+1]);
      }else if ( splitted_output[i].find(std::string("write_bytes:")) != std::string::npos) {
        std::cout << ", Bytes written: " << (std::stol(splitted_output[i+1]) - data[1]) << "\n\n";
        data[1] = std::stol(splitted_output[i+1]);
        break;
      }
    }
  }

  unsigned long print_rss_value(){
    std::string command = std::string("cat /proc/") + std::to_string(getpid()) + std::string("/stat");
    std::string command_output = exec(command);
    std::vector<std::string> splitted_output = split(command_output, " ");
    return std::stol(splitted_output[23])*4096;
  }

  //limits the memory, memory in bytes and

  void limit_memory(unsigned long long memory_in_bytes, const char* string2){
    //std::cout << "Entering limit memory function\n";
    std::string string = std::to_string(memory_in_bytes);
    std::string command = std::string("bash -c \"echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.limit_in_bytes\"");
    //std::cout << "Command: " << command << std::endl;
    int return_code = system(command.c_str());
    //std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
    if (return_code != 0){
      std::cerr << "Error. Unable to set cgroup memory " << string << " Code: " << return_code << "\n";
      std::cerr << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
      //exit(1);
    }
    //std::cout << "Limiting cgroup memory: " << string << " bytes\n";
  }





}
