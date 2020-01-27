/* C++ program for Merge Sort */
#include<iostream>
#include<vector>
#include<queue>
#include<ctime>
#include<sys/stat.h>
#include<cstdlib>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<memory>
#include<unistd.h>
#include<cstring>
#include<algorithm>
#include<string>
#include<fcntl.h>

const int NUM_WAYS = 128;
const int progress_depth = 32;
const int data_in_megabytes = 128;
const unsigned long long num_elements = data_in_megabytes*1024*1024/4;

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


std::string exec(std::string cmd){
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
  int return_code = system(command.c_str());
  //std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup memory " << string << " Code: " << return_code << "\n";
    std::cout << "Memory usage: " << exec(std::string("cat /var/cgroups/") + string2 + std::string("/memory.usage_in_bytes")) << std::endl;
  }
  std::cout << "Limiting cgroup memory: " << string << " bytes\n";
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
	std::string depth_trace = "";
	int n3 = num_elements;
	int limit = 0;
	while (n3 > size || n3 == 1){
		n3 /= NUM_WAYS;
		depth_trace += " ";
		limit++;
	}
	if (limit < progress_depth){
		std::cout << depth_trace << "Running matrix multiply with depth: " << limit;
		std::cout << " value of size: " << size << std::endl;
	}
  //recursion
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

int main(int argc, char *argv[]){
	srand (time(NULL));

	if (argc < 2){
		std::cout << "Insufficient arguments! Usage: merge_sort <memory_limit> <cgroup_name>\n";
		exit(1);
	}

	int fdout;

	if ((fdout = open ("nullbytes", O_RDWR, 0x0777 )) < 0){
		printf ("can't create nullbytes for writing\n");
		return 0;
	}

	Int* array;
	if (((array = (Int*) mmap(0, sizeof(Int)*num_elements, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (Int*)MAP_FAILED)){
			 printf ("mmap error for output with code");
			 return 0;
	 }

 	std::cout << "Running EM merge sort on an array of size: " << (int)num_elements << "\n";

	std::vector<long> io_stats = {0,0};
	print_io_data(io_stats, "Printing I/O statistics at program start @@@@@ \n");

  for (unsigned long long i = 0; i < num_elements; i++){
    //records[i] = Integer(57-i);
		array[i] = Int(rand() % 1000);
		//std::cout << array[i].val << "\t";
  }

  limit_memory(std::stol(argv[1])*1024*1024,argv[2]);

	std::cout << "\n==================================================================\n";

	std::clock_t start;
  double duration;
	start = std::clock();

	print_io_data(io_stats, "Printing I/O statistics just before sorting start @@@@@ \n");

	merge_sort<class Int>(&array[0], &array[num_elements]);
  /*for (int i = 0; i < num_elements; i++){
    std::cout << array[i].val << "\t";
  }*/
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

	std::cout << std::endl;
	print_io_data(io_stats, "Printing I/O statistics just after sorting start @@@@@ \n");
	std::cout << "Total sorting time: " << duration << "\n";
	std::cout << "Size of record: " << sizeof(Int) << std::endl;
  return 0;
}
