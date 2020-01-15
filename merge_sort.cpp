/* C++ program for Merge Sort */
#include<iostream>
#include<vector>
#include<queue>
#include<cstring>
#include<algorithm>

const int NUM_WAYS = 15;

class Int
{
	public:
		int val;
    Int(int i = 0): val(i) {}
    friend bool operator> (const Int &c1, const Int &c2);
		friend bool operator< (const Int &c1, const Int &c2);
		friend bool operator== (const Int &c1, const Int &c2);
};

bool operator> (const Int &c1, const Int &c2)
{
    return c1.val > c2.val;
}

bool operator< (const Int &c1, const Int &c2)
{
    return c1.val < c2.val;
}

bool operator== (const Int &c1, const Int &c2)
{
    return c1.val == c2.val;
}

template <class T>
class HeapNode {
  public:
    T element;
    int index;
    HeapNode(T e, int i): element(e), index(i) {}
};

// Comparison object to be used to order the heaps
template <class T>
class HeapNodeComparator {
public:
    bool operator() (const HeapNode<T> lhs, const HeapNode<T> rhs) const {
        return lhs.element > rhs.element;
    }
};

template <class T>
void _merge_sort(T* inFirst, T* inLast,int d, T* space) {
  int size = inLast - inFirst;
	//std::cout << "In Merge sort function of size: " << size << std::endl;
  // Base case
  if (size <= 1<<d || size <= NUM_WAYS){
		//std::cout << "Exiting Merge sort function of size: " << size << std::endl;
    std::sort(inFirst, inLast);
    return;
  }

  //recursion
	int split = size/NUM_WAYS;
	//std::cout << "SPLIT: " << split << std::endl;

	T* starting = inFirst;
	T* ending = inFirst;
	T* starting_indices[NUM_WAYS];
	T* ending_indices[NUM_WAYS];
	for (int i = 0; i < NUM_WAYS; i++){
		starting = ending;
    ending = starting + size/NUM_WAYS + (i < (size % NUM_WAYS));
    if (ending > inLast){
      ending = inLast;
    }
		starting_indices[i] = starting;
		ending_indices[i] = ending;
		//std::cout << "Starting: " << starting-inFirst << " Ending: " << ending-inFirst << "\n";

    _merge_sort(starting,ending,d,space);
  }

	/*std::cout << "Done with recursive step. Merged array:\n";
	for (int i = 0; i < size; i++){
    std::cout << (inFirst+i)->val << "\t";
  }*/

	//std::cout << "\nIn merging step\n";

  //merging step

  std::priority_queue<HeapNode<T>, std::vector<HeapNode<T>>, HeapNodeComparator<T>> heap;

  for (int i = 0; i < NUM_WAYS; i++){
    HeapNode<T> node = HeapNode<T>(HeapNode<T>(*starting_indices[i],i));
		//std::cout << "Pushing node to heap with value: " << node.element.val << " and index: " << (starting_indices[node.index]-inFirst) << std::endl;
    heap.push(node);
  }

  for (int i = 0; i < size; i++){
		//std::cout << "Current size of heap: " << heap.size() << std::endl;
    HeapNode<T> node = heap.top();
		//std::cout << "Popping node with value: " << node.element.val << std::endl;
    *(space++) = node.element;
    heap.pop();
    T* new_index = ++starting_indices[node.index];

		//std::cout << "New index: " << (new_index - inFirst) << std::endl;
    if (new_index < ending_indices[node.index]) {
			//std::cout << "Pushing element to heap with value: " << new_index->val << std::endl;
      heap.push(HeapNode<T>(*new_index,node.index));
    }
  }

	space -= size;

  if (heap.size() != 0){
    std::cout << "Error! Heap still has entries!.\n";
    exit(0);
  }

  std::memcpy(inFirst, space, (inLast-inFirst)*sizeof(T));
	//std::cout << "Exiting Merge sort function of size: " << size << std::endl;
}

template <class T>
void merge_sort(T* inFirst, T* inLast, int d=3) {
  int size = inLast - inFirst;
  // Allocate temporary space used as the output buffer of the funnels .
  T* space = new T[size];
  _merge_sort(inFirst, inLast, d, space);
  delete[] space;
}

int main(){
	srand (time(NULL));
	int fdout;

	const int num_elements = 100;

	Int array[num_elements];

 	std::cout << "Running external merge sort sort on an array of size: " << (int)num_elements << "\n";
	//Integer_comparator comp;

  for (unsigned long long i = 0; i < num_elements; i++){
    //records[i] = Integer(57-i);
		array[i] = Int(rand() % 1000);
		std::cout << array[i].val << "\t";
  }

	std::cout << "\n";

	std::cout << "Running merge sort now.\n";
	merge_sort<class Int>(&array[0], &array[num_elements], 2);
	std::cout << "\nDone with merge sort.\n";
  for (int i = 0; i < num_elements; i++){
    std::cout << array[i].val << "\t";
  }

	std::cout << std::endl;
  return 0;
}
