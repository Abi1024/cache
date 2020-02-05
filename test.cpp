#include <iostream>
#include <cmath>
using namespace std;

int main(){
  int length = 16384;
  long long starting_memory = 40*1024*1024;
	int mem_profile_depth = std::ceil(log( (long)length*length*4*3 / ((double)starting_memory))/log(4));
  cout << mem_profile_depth << std::endl;
  return 0;
}
