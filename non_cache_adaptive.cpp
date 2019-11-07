#include <iostream>
#include <stxxl/vector>
const int B = 64;

#define TYPE int
const int CACHE = 1;
const stxxl::uint64 n = 16;
typedef stxxl::VECTOR_GENERATOR<TYPE,1,CACHE>::result vector_type;
typedef stxxl::vector<TYPE, 1, stxxl::lru_pager<CACHE> >::iterator itr;

void conv_RM_2_ZM_RM( itr x, itr xo, int n, int no ){
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

    for (int i = 0; i < n*n; i++){
      x[i] += y2[i];
			y2[i] = 0;
    }
	}
}

void mm_root(itr x, itr u, itr v, int n){
  int extra_memory = 0;
  int n2 = n;
  while (n2 > B){
    extra_memory += n2*n2;
    n2 >>= 1;
  }
  //cout << "extra_memory " << extra_memory << endl;
  vector_type auxiliary_array;
  for (int i = 0; i < extra_memory; i++){
    auxiliary_array.push_back(0);
  }
  mm(x, u, v, auxiliary_array.begin(), n, n);
}


void conv_RM_2_ZM_CM( itr x, itr xo, int n, int no )
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			itr xx = x + i;

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

int main(){
	stxxl::stats* Stats = stxxl::stats::get_instance();
	stxxl::stats_data stats1(*Stats);
		stxxl::block_manager * bm = stxxl::block_manager::get_instance();
  conv_vector_type conv_array;


	std::cout << "running cache_adaptive matrix multiply with matrices of size: " << (int)length << "x" << (int)length << "\n";
  //std::cout << "First input array\n";
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		conv_array.push_back(i);
		//std::cout << array[i] << " ";
	}
  //std::cout << std::endl;
	stxxl::stats_data stats2(stxxl::stats_data(*Stats) - stats1);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] First array (array) loaded: "<< stats2 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated:  " << bm->get_maximum_allocation()/(1024));

	for (stxxl::uint64 i = 0; i < length*length; i++){
		conv_array.push_back(0);
	}
	stxxl::stats_data stats3(stxxl::stats_data(*Stats) - stats2);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Second array (input_1) loaded: "<< stats3 );
	std::cout << "===========================================\n";

  conv_RM_2_ZM_RM(conv_array.begin()+length*length,conv_array.begin(),length,length);
	stxxl::stats_data stats4(stxxl::stats_data(*Stats) - stats3);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] First matrix conversion "<< stats4 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated for first conversion:  " << bm->get_maximum_allocation()/(1024));

	vector_type array;
	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		array.push_back(conv_array[i+length*length]);
	}

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
	stxxl::stats_data stats5(stxxl::stats_data(*Stats) - stats4);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Third array (array2) loaded: "<< stats5 );
	std::cout << "===========================================\n";

	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		conv_array[i+length*length] = i;
	}
	stxxl::stats_data stats6(stxxl::stats_data(*Stats) - stats5);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Fourth array (input_2) loaded: "<< stats6 );
	std::cout << "===========================================\n";

  conv_RM_2_ZM_CM(conv_array.begin()+length*length,conv_array.begin(),length,length);
	stxxl::stats_data stats7(stxxl::stats_data(*Stats) - stats6);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Second matrix conversion: "<< stats7 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated for second conversion:  " << bm->get_maximum_allocation()/(1024));

	for (stxxl::uint64 i = 0; i < length*length; i++)
	{
		array.push_back(conv_array[i+length*length]);
	}

  for (stxxl::uint64 i = 0 ; i < length*length; i++){
    array.push_back(0);
  }


	stxxl::stats_data stats8(stxxl::stats_data(*Stats) - stats7);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Final array (result) loaded: "<< stats8 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated for setting up result matrix:  " << bm->get_maximum_allocation()/(1024));
	stxxl::timer start_p1;
	start_p1.start();

	mm(array.begin()+2*length*length,array.begin(),array.begin()+length*length,length);

	start_p1.stop();

	stxxl::stats_data stats9(stxxl::stats_data(*Stats) - stats8);
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Matrix multiplication: "<< stats9 );
	std::cout << "===========================================\n";
	STXXL_MSG("[LOG] Max KB allocated:  " << bm->get_maximum_allocation()/(1024));
	std::cout << "[LOG] Total multiplication time: " <<(start_p1.mseconds()/1000) << "\n";
	std::cout << "===========================================\n";
	std::cout << "Total system I/O statistics of process:\n";
	std::string result_log = std::string("cat /proc/") + std::to_string(getpid()) + std::string("/io");
	system(result_log.c_str());
	/*std::cout << "Result array\n";
  for (stxxl::uint64 i = 2*length*length ; i < 3*length*length; i++){
    std::cout << array[i] << " ";
  }
  std::cout << std::endl;
	*/
  return 0;
}
