#include "CacheHelper.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<ctime>
#define TYPE int

unsigned long length = 0;
const int progress_depth = 4;

//x is the output, u and v are the two inputs
void mm( TYPE* x, TYPE* u, TYPE* v, int n )
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
			n3 /= 2;
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
		mm( x + m11, u + m11, v + m11, nn );
		mm( x + m12, u + m11, v + m12, nn );
		mm( x + m21, u + m21, v + m11, nn );
		mm( x + m22, u + m21, v + m12, nn );

		mm( x + m11, u + m12, v + m21, nn );
		mm( x + m12, u + m12, v + m22, nn );
		mm( x + m21, u + m22, v + m21, nn );
		mm( x + m22, u + m22, v + m22, nn );
	}
}

int main(int argc, char *argv[]){

  if (argc < 4){
    std::cout << "Insufficient arguments! Usage: cgroup_cache_adaptive <matrix_width> <memory_limit> <cgroup_name>\n";
    exit(1);
  }
  length = std::stol(argv[1]);
	std::cout << "Running cache_adaptive matrix multiply with matrices of size: " << (int)length << "x" << (int)length << "\n";
  std::vector<long> io_stats = {0,0};
  CacheHelper::print_io_data(io_stats, "Printing I/O statistics at program start @@@@@ \n");

  int fdout;

  if ((fdout = open ("nullbytes", O_RDWR, 0x0777 )) < 0){
    printf ("can't create nullbytes for writing\n");
    return 0;
  }

  TYPE* dst;
  if (((dst = (TYPE*) mmap(0, sizeof(TYPE)*length*length*3, PROT_READ | PROT_WRITE, MAP_SHARED , fdout, 0)) == (TYPE*)MAP_FAILED)){
       printf ("mmap error for output with code");
       return 0;
  }

	/*
	for (unsigned int i = 0 ; i < 3*length*length; i++){
		std::cout << dst[i] << "\t";
	}
	std::cout << "\n";
	*/
	CacheHelper::print_io_data(io_stats, "Printing I/O statistics AFTER loading output matrix to cache @@@@@ \n");


	std::cout << "===========================================\n";

  //MODIFY MEMORY WITH CGROUP
  CacheHelper::limit_memory(std::stol(argv[2])*1024*1024,argv[3]);

  std::clock_t start;
  double duration;
	start = std::clock();
	mm(dst,dst+length*length,dst+length*length*2,length);

	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

	std::cout << "===========================================\n";
	std::cout << "Total multiplication time: " << duration << "\n";
	std::cout << "===========================================\n";
  std::cout << "Data: " << (unsigned int)dst[length*length/2/2+length] << std::endl;
	std::cout << "===========================================\n";
	std::cout << "===========================================\n";
  CacheHelper::print_io_data(io_stats, "Printing I/O statistics AFTER matrix multiplication @@@@@ \n");
	/*std::cout << "Result array\n";
  for (unsigned int i = 0 ; i < length*length; i++){
    std::cout << dst[i] << " ";
  }
  std::cout << std::endl;
	*/
  return 0;
}
