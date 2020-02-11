#include"CacheHelper.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<ctime>
#include<cstring>
#include<cmath>
#include<fstream>
#define TYPE int

bool mem_profile = false;
int mem_profile_depth = 3;
const int progress_depth = 4;

std::ofstream out;
std::clock_t start;
double duration;
unsigned long length = 0;
char* cgroup_name;
long long starting_memory = -1;
std::vector<long> io_stats = {0,0};

//x is output, y is auxiiliary memory, u and v are inputs
void mm( TYPE* x, TYPE* u, TYPE* v, TYPE* y, int n0, int n)
{
	if ( n <= CacheHelper::MM_BASE_SIZE )
	{
		for ( int i = 0; i < n; i++ )
		{
			TYPE* vv = v;
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
			n3 >>= 1;
			depth_trace += " ";
			limit++;
		}
    if (limit < progress_depth){
      std::cout << depth_trace << "Running matrix multiply with depth: " << limit;
  		std::cout << " value of n: " << n << std::endl;
    }
		int nn = ( n >> 1 );
		int nn2 = nn * nn;

		const int m11 = 0;
		int m12 = m11 + nn2;
		int m21 = m12 + nn2;
		int m22 = m21 + nn2;

    int n2 = n0;
    TYPE* y2 = y;
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

		if (mem_profile && limit < mem_profile_depth){
        //std::cout << "Depth: " << limit << std::endl;
        duration = (int)(1000.0 * (std::clock() - start) / CLOCKS_PER_SEC);
        //std::cout << "Duration: " << duration << std::endl;
        out << duration << " " << (long)3*n*n*4 << std::endl;
        //std::cout << "Writing the output \n";
				CacheHelper::limit_memory((long)3*n*n*4+10000,cgroup_name);
        //std::cout << "Limited the memory" << std::endl;
		}

    for (int i = 0; i < n*n; i++){
      x[i] += y2[i];
			y2[i] = 0;
    }

    if (mem_profile && limit < mem_profile_depth){
        //std::cout << "Depth2: " << limit << std::endl;
        duration = (int)(1000.0 * (std::clock() - start) / CLOCKS_PER_SEC);
        //std::cout << "Duration: " << duration << std::endl;
        out << duration << " " << starting_memory << std::endl;
				CacheHelper::limit_memory(starting_memory,cgroup_name);
		}

	}
}

//y is auxiliary memory
void mm_root(TYPE* x, TYPE* u, TYPE* v, TYPE* y, int n){
	std::cout << "Start of root call\n";
  int extra_memory = 0;
  int n2 = n;
  while (n2 > CacheHelper::MM_BASE_SIZE){
    extra_memory += n2*n2;
    n2 >>= 1;
  }
  std::cout << "extra_memory " << extra_memory << "\n";
  /*for (int i = 0; i < extra_memory; i++){
    y[i] = 0;
  }*/
	//MODIFY MEMORY WITH CGROUP
	CacheHelper::limit_memory(starting_memory,cgroup_name);
  CacheHelper::print_io_data(io_stats, "Printing I/O statistics AFTER loading output matrix to cache @@@@@ \n");
	std::cout << "===========================================\n";
	std::cout << "About to multiply\n";
	start = std::clock();
  mm(x, u, v, y, n, n);
}

int main(int argc, char *argv[]){

	if (argc < 5){
		std::cout << "Insufficient arguments! Usage: non_cache_adaptive <memory_profile> <matrix_width> <memory_limit> <cgroup_name>\n";
		exit(1);
	}

  cgroup_name = new char[strlen(argv[4]) + 1]();
  strncpy(cgroup_name,argv[4],strlen(argv[4]));
	std::cout << "printing cgroup name " << cgroup_name << std::endl;
  starting_memory = std::stol(argv[3])*1024*1024;

  length = std::stol(argv[2]);

	std::ofstream mm_out = std::ofstream("mm_out.txt",std::ofstream::out | std::ofstream::app);

	std::string memory_profile = "";
	std::string mem_profile_filename = "";
	switch(std::stoi(argv[1])) {
		case 0: //constant memory
			memory_profile = " CONSTANT memory";
			mem_profile = false;
			break;
		case 1: //worst case memory
			memory_profile = " WORST-CASE memory";
			mem_profile = true;
			mem_profile_depth = std::ceil(log( (long)length*length*4*3 / ((double)starting_memory))/log(4));
			std::cout << "MEMORY PROFILE DEPTH: " << mem_profile_depth << std::endl;
			//mem_profile_depth = 3;
		  mem_profile_filename = "mem_profile" + std::to_string(length) + ".txt";
			std::cout << "Memory profile filename: " << mem_profile_filename << std::endl;
			out = std::ofstream(mem_profile_filename.c_str(),  std::ofstream::out);
			break;
		case 2:
			memory_profile = " RANDOM memory";
			mem_profile = false;
			break;
		case 3:
			memory_profile = " WORST-CASE but replay";
			mem_profile = false;
			break;
		default:
			break;
	}
	std::cout << "Running non-cache_adaptive matrix multiply with matrices of size: " << (int)length << "x" << (int)length << "\n";
  std::vector<long> io_stats = {0,0};
  CacheHelper::print_io_data(io_stats, "Printing I/O statistics at program start @@@@@ \n");



	int fdout;

  if ((fdout = open ("nullbytes", O_RDWR, 0x0777 )) < 0){
    printf ("can't create nullbytes for writing\n");
    return 0;
  }

  TYPE* dst;
  if (((dst = (TYPE*) mmap(0, sizeof(TYPE)*length*length*5, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (TYPE*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }

	/*
	for (unsigned int i = 0 ; i < 3*length*length; i++){
		std::cout << dst[i] << "\t";
	}
	std::cout << "\n";
	*/

  double duration;
	mm_root(dst,dst+length*length,dst+length*length*2,dst+length*length*3,length);

	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

	std::cout << "===========================================\n";
	std::cout << "Total multiplication time: " << duration << "\n";
	std::cout << "===========================================\n";

  std::cout << "Data: " << (unsigned int)dst[length*length/2/2+length] << std::endl;
	std::cout << "===========================================\n";
	std::cout << "===========================================\n";

  CacheHelper::print_io_data(io_stats, "Printing I/O statistics AFTER matrix multiplication @@@@@ \n");
	std::cout << "Result memory profile stored in: " << mem_profile_filename << std::endl;
		mm_out << "Cache-non-adaptive " << memory_profile << "," << argv[3] << "," << length << "," << duration << "," << io_stats[0] << "," << io_stats[1] << std::endl;
	/*std::cout << "Result array\n";
  for (unsigned int i = 0 ; i < length*length; i++){
    std::cout << dst[i] << " ";
  }
  std::cout << std::endl;
	*/
  return 0;
}
