#include <iostream>
#include <stxxl/vector>
const int B = 64;

#define TYPE int
const int CACHE = 1;
const stxxl::uint64 n = 1000;
typedef stxxl::VECTOR_GENERATOR<TYPE,4,CACHE>::result vector_type;
typedef stxxl::vector<TYPE, 4, stxxl::lru_pager<CACHE> >::iterator itr;

int main(){
  vector_type array;
  //vector_type array2;
	std::cout << "running test program with matrices of size: " << (int)n << "x" << (int)n << "\n";
  stxxl::timer start_p1;
	start_p1.start();

	stxxl::stats* Stats = stxxl::stats::get_instance();
	stxxl::stats_data stats_begin(*Stats);
		stxxl::block_manager * bm = stxxl::block_manager::get_instance();

    for (stxxl::uint64 i = 0; i < n*n; i++)
    {
      array.push_back(i);
      //array2.push_back(i);
    }

    for (stxxl::uint64 i = 0; i < n*n; i++)
    {
      array[i] = 1;
    }

    for (stxxl::uint64 i = 0; i < n*n; i++)
    {
      array[i] = 2;
    }

	  start_p1.stop();



	std::cout << "done multiplying matrix\n";
	STXXL_MSG("[LOG] IO Statistics for sorting: "<<(stxxl::stats_data(*Stats) - stats_begin));
	STXXL_MSG("[LOG] Max MB allocated:  " << bm->get_maximum_allocation()/(1024*1024));
	std::cout << "[LOG] Total loading time: " <<(start_p1.mseconds()/1000) << "\n";
  //std::cout << "Result array\n";
  /*for (stxxl::uint64 i = 0 ; i < n*n; i++){
    std::cout << result[i] << " ";
  }*/
  //std::cout << std::endl;
  return 0;
}
