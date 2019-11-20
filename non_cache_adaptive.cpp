#include <iostream>
#include <stxxl/vector>
#include <string>
#include <stdlib.h>
#include <unistd.h>
const int B = 64;

#define TYPE int
const int CACHE = 4; //pages per cache_adaptive
const int PAGE_SIZE = 32; //blocks per page
const int BLOCK_SIZE_IN_BYTES = 8192; //block size in bytes
const stxxl::uint64 length = 2048;
typedef stxxl::VECTOR_GENERATOR<TYPE,PAGE_SIZE,CACHE,BLOCK_SIZE_IN_BYTES>::result vector_type;
typedef stxxl::vector<TYPE, PAGE_SIZE, stxxl::lru_pager<CACHE>,BLOCK_SIZE_IN_BYTES>::iterator itr;
const bool mem_profile = false;
char* cgroup_name = NULL;

const int CONV_CACHE = 128; //pages per cache_adaptive
const int CONV_PAGE_SIZE = 4; //blocks per page
const int CONV_BLOCK_SIZE_IN_BYTES = 4194304; //block size in bytes
typedef stxxl::VECTOR_GENERATOR<TYPE,CONV_PAGE_SIZE,CONV_CACHE,CONV_BLOCK_SIZE_IN_BYTES>::result conv_vector_type;
typedef stxxl::vector<TYPE, CONV_PAGE_SIZE, stxxl::lru_pager<CONV_CACHE>,CONV_BLOCK_SIZE_IN_BYTES>::iterator conv_itr;

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

void print_io_data(std::vector<int>& data, std::string header){
  std::cout << header;
  std::string command = std::string("cat /proc/") + std::to_string(getpid()) + std::string("/io");
  std::string command_output = exec(command);
  std::vector<std::string> splitted_output = split(command_output, " ");
  for (unsigned int i = 0; i < splitted_output.size(); ++i){
    if ( splitted_output[i].find(std::string("read_bytes:")) != std::string::npos) {
      std::cout << "Bytes read: " << (std::stoi(splitted_output[i+1]) - data[0]);
      data[0] = std::stoi(splitted_output[i+1]);
    }else if ( splitted_output[i].find(std::string("write_bytes:")) != std::string::npos) {
      std::cout << ", Bytes written: " << (std::stoi(splitted_output[i+1]) - data[1]) << "\n\n";
      data[1] = std::stoi(splitted_output[i+1]);
      break;
    }
  }
}

void limit_memory(const char* string1, const char* string2){
  int memory_in_bytes = (int)atof(string1)*1024*1024;
  std::string string = std::to_string(memory_in_bytes);
  std::string command = std::string("echo ") + string + std::string(" > /var/cgroups/") + string2 + std::string("/memory.limit_in_bytes");
  int return_code = system(command.c_str());
  if (return_code != 0){
    std::cout << "Error. Unable to set cgroup memory " << string << "\n";
    exit(1);
  }
  std::cout << "Limiting cgroup memory: " << string << " bytes\n";
}

void conv_RM_2_ZM_RM( conv_itr x, conv_itr xo, int n, int no ){
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			for ( int j = 0; j < n; j++ )
				( *x++ ) = ( *xo++ );

			xo += ( no - n );
		}
	}
	else
	{
		int nn = ( n >> 1 );
		int nn2 = nn * nn;

		const int m11 = 0;
		int m12 = m11 + nn2;
		int m21 = m12 + nn2;
		int m22 = m21 + nn2;

		conv_RM_2_ZM_RM( x, xo, nn, no );
		conv_RM_2_ZM_RM( x+m12, xo + nn, nn, no );
		conv_RM_2_ZM_RM( x+m21, xo + nn * no, nn, no );
		conv_RM_2_ZM_RM( x+m22, xo + nn * no + nn, nn, no );
	}
}

void conv_ZM_RM_2_RM( itr x, itr xo, int n, int no )
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			for ( int j = 0; j < n; j++ )
				( *xo++ ) = ( *x++ );

			xo += ( no - n );
		}
	}
	else
	{
		int nn = ( n >> 1 );
		int nn2 = nn * nn;
		const int m11 = 0;
		int m12 = m11 + nn2;
		int m21 = m12 + nn2;
		int m22 = m21 + nn2;

		conv_ZM_RM_2_RM( x, xo, nn, no );
		conv_ZM_RM_2_RM( x+ m12, xo + nn, nn, no );
		conv_ZM_RM_2_RM( x+ m21, xo + nn * no, nn, no );
		conv_ZM_RM_2_RM( x+ m22, xo + nn * no + nn, nn, no );
	}
}

void mm( itr x, itr u, itr v, itr y, int n0, int n)
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			itr vv = v;
			for ( int j = 0; j < n; j++ )
			{
				TYPE t = 0;

				for ( int k = 0; k < n; k++ )
					t += u[ k ] * vv[ k ];

				( *x++ ) += t;
				vv += n;
			}
			u += n;
		}
	}
	else
	{
		std::string depth_trace = "";
		int n3 = length;
		int limit = 0;
		while (n3 > n || n3 == 1){
			n3 /= 2;
			depth_trace += " ";
			limit++;
		}
		std::cout << depth_trace << "Running matrix multiply with depth: " << limit;
		std::cout << " value of n: " << n << std::endl;
    //cout << "In recursive call of size: " << n << endl;
		int nn = ( n >> 1 );
		int nn2 = nn * nn;

		const int m11 = 0;
		int m12 = m11 + nn2;
		int m21 = m12 + nn2;
		int m22 = m21 + nn2;

    int n2 = n0;
    itr y2 = y;
    while (n2 > n){
      y2 += n2*n2;
      n2 >>= 1;
    }
    //cout << "y2-y in this call: " << y2-y << endl;
		mm( x + m11, u + m11, v + m11, y, n0, nn);
		mm( x + m12, u + m11, v + m12, y, n0, nn );
		mm( x + m21, u + m21, v + m11, y, n0, nn );
		mm( x + m22, u + m21, v + m12, y, n0, nn );

		mm( y2 + m11, u + m12, v + m21, y, n0, nn );
		mm( y2 + m12, u + m12, v + m22, y, n0, nn );
		mm( y2 + m21, u + m22, v + m21, y, n0, nn );
		mm( y2 + m22, u + m22, v + m22, y, n0, nn );

		if (mem_profile && limit < 3){
				limit_memory(,cgroup_name);
		}

    for (int i = 0; i < n*n; i++){
      x[i] += y2[i];
			y2[i] = 0;
    }
	}
}

void mm_root(vector_type& array, 	stxxl::block_manager * bm, itr x, itr u, itr v, int n){
	std::cout << "Start of root call\n";
  int extra_memory = 0;
  int n2 = n;
  while (n2 > B){
    extra_memory += n2*n2;
    n2 >>= 1;
  }
  std::cout << "extra_memory " << extra_memory << "\n";
  for (int i = 0; i < extra_memory; i++){
    array.push_back(0);
  }
	std::cout << "About to multiply\n";
		STXXL_MSG("[LOG] Max KB allocated:  " << bm->get_maximum_allocation()/(1024));
  mm(x, u, v, x+n*n, n, n);
}


void conv_RM_2_ZM_CM( conv_itr x, conv_itr xo, int n, int no )
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			conv_itr xx = x + i;

			for ( int j = 0; j < n; j++ )
			{
				( *xx ) = ( *xo++ );
				xx += n;
			}


			xo += ( no - n );
		}
	}
	else
	{
		int nn = ( n >> 1 );
		int nn2 = nn * nn;
		const int m11 = 0;
		int m12 = m11 + nn2;
		int m21 = m12 + nn2;
		int m22 = m21 + nn2;

		conv_RM_2_ZM_CM( x, xo, nn, no );
		conv_RM_2_ZM_CM( x+ m12, xo + nn, nn, no );
		conv_RM_2_ZM_CM( x+ m21, xo + nn * no, nn, no );
		conv_RM_2_ZM_CM( x+ m22, xo + nn * no + nn, nn, no );
	}
}

int main(int argc, char *argv[]){
  if (argc < 3){
    std::cout << "Insufficient arguments! Usage: cgroup_cache_adaptive <memory_limit> <cgroup_name>\n";
    exit(1);
  }
	std::cout << "Running cache_adaptive matrix multiply with matrices of size: " << (int)length << "x" << (int)length << "\n";
  std::vector<int> io_stats = {0,0};
  print_io_data(io_stats, "Printing I/O statistics at program start @@@@@ \n");

	stxxl::stats* Stats = stxxl::stats::get_instance();
	stxxl::stats_data stats1(*Stats);
	stxxl::block_manager * bm = stxxl::block_manager::get_instance();
  conv_vector_type conv_array;

  //std::cout << "First input array\n";
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		conv_array.push_back(i);
		//std::cout << array[i] << " ";
	}
  print_io_data(io_stats, "Printing I/O statistics AFTER loading first matrix @@@@@\n");
  //std::cout << std::endl;
	STXXL_MSG("[LOG] Max KB allocated for one matrix:  " << bm->get_maximum_allocation()/(1024));

	for (stxxl::uint64 i = 0; i < length*length; i++){
		conv_array.push_back(0);
	}

  print_io_data(io_stats, "Printing I/O statistics AFTER loading second matrix @@@@@  \n");

  conv_RM_2_ZM_RM(conv_array.begin()+length*length,conv_array.begin(),length,length);

	print_io_data(io_stats, "Printing I/O statistics AFTER first conversion @@@@@\n");

	vector_type array;
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		array.push_back(conv_array[i+length*length]);
	}
  print_io_data(io_stats, "Printing I/O statistics AFTER copying result of first conversion to cache @@@@@\n");

	/*std::cout << "First input array in Z-MORTON\n";
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		std::cout << array[i+length*length] << " ";
	}
	std::cout << std::endl;
*/
  std::cout << "Second input array\n";
	for (stxxl::uint64 i = 0; i < length*length; i++){
		conv_array[i] = i;
	}
  print_io_data(io_stats, "Printing I/O statistics AFTER loading third matrix @@@@@ \n");
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		conv_array[i+length*length] = 0;
	}
  print_io_data(io_stats, "Printing I/O statistics AFTER loading fourth matrix @@@@@ \n");
  conv_RM_2_ZM_CM(conv_array.begin()+length*length,conv_array.begin(),length,length);
  print_io_data(io_stats, "Printing I/O statistics AFTER second conversion @@@@@ \n");
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		array.push_back(conv_array[i+length*length]);
	}
  print_io_data(io_stats, "Printing I/O statistics AFTER copying result of second conversion to cache @@@@@ \n");

  for (stxxl::uint64 i = 0 ; i < length*length; i++){
    array.push_back(0);
  }
  print_io_data(io_stats, "Printing I/O statistics AFTER loading output matrix to cache @@@@@ \n");

	stxxl::stats_data stats2(stxxl::stats_data(*Stats) - stats1);

	std::cout << "===========================================\n";

  //MODIFY MEMORY WITH CGROUP
  limit_memory(argv[1],argv[2]);

	stxxl::timer start_p1;
	start_p1.start();

  mm_root(array,bm,array.begin()+2*length*length,array.begin(),array.begin()+length*length,length);
	start_p1.stop();

	stxxl::stats_data stats3(stxxl::stats_data(*Stats) - stats2);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Matrix multiplication: "<< stats3 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated:  " << bm->get_maximum_allocation()/(1024));
	std::cout << "[LOG] Total multiplication time: " <<(start_p1.mseconds()/1000) << "\n";
	std::cout << "===========================================\n";
  print_io_data(io_stats, "Printing I/O statistics AFTER matrix multiplication @@@@@ \n");

	/*std::cout << "Result array\n";
  for (stxxl::uint64 i = 2*length*length ; i < 3*length*length; i++){
    std::cout << array[i] << " ";
  }
  std::cout << std::endl;
	*/
  return 0;
}
