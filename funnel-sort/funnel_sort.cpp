#include "funnelSort.h"
#include <iostream>
#include <cstdlib>

class Integer
{
	public:
		int val;
    Integer(int i = 0): val(i) {}
};

class Integer_comparator
{
	public:
		bool operator () (const Integer& a, const Integer& b) const
		{
			return (a.val < b.val);
		}
};

int main(){
	srand (time(NULL));
  const int num_elements = 100;
	Integer_comparator comp;
	Integer* records = new Integer[num_elements];
  for (int i = 0; i < num_elements; i++){
    //records[i] = Integer(57-i);
		records[i] = Integer(rand() % 1000);
		std::cout << records[i].val << "\t";
  }
	std::cout << "\n==================================================================\n";
	FunnelSort::sort<class Integer, class Integer_comparator>(&records[0], &records[num_elements], comp);
  for (int i = 0; i < num_elements; i++){
    std::cout << records[i].val << "\t";
  }
	std::cout << std::endl;
  delete[] records;
  return 0;
}
