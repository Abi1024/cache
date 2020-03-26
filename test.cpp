#include <iostream>
#include <string>
#include <tuple>
#include <list>
#include <cstdint>
#include <inttypes.h>
#include <exception>
#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

uint64_t MyTask::get_smallest_prime_factor(uint64_t n, uint64_t start ){
  for (uint64_t i = start;  i*i <= n; i++){
    if (n % i == 0){
      return i;
    }
  }
  return 1;
}

std::list<uint64_t> MyTask::get_all_prime_divisors(uint64_t n)
{
  uint64_t temp_n = n;
  std::list<uint64_t> result;
  uint64_t factor = 1;
  uint64_t start = 2;
  while ((factor = get_smallest_prime_factor(n,start)) != 1 ){
    if (result.empty() || result.back() != factor){
      result.push_back(factor);
    }
    start = factor;
    n /= factor;
  }
  if (result.empty() || (n != temp_n && result.back() != n) )
    result.push_back(n);
  result.reverse();
  return result;
}

int main(){
  std::list<uint64_t> result = get_all_prime_divisors(3600);
  //cout << "printing list" << endl;
  for (auto it = result.begin(); it != result.end(); ++it)
       cout << ' ' << *it;
  cout << "\n";
  return 0;
}
