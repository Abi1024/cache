#include <iostream>

int main(){
  const int N = 100;
  const int ways = 4;
  int arr[ways];
  int start = 0;
  for (int i = 0; i < ways; i++){
    arr[i] = N/ways + (i < (N % ways));
    start = 0;
    std::cout << arr[i] << "\t";
    end = 0 + arr[i];
  }
  std::cout << "\nDone\n";
  return 0;
}
