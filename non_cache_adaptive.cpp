#include <iostream>
#include <stxxl/vector>
const int B = 64;

#define TYPE int
const int CACHE = 1;
const stxxl::uint64 n = 750;
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
  vector_type array;
	stxxl::stats* Stats = stxxl::stats::get_instance();
	stxxl::stats_data stats_begin(*Stats);
		stxxl::block_manager * bm = stxxl::block_manager::get_instance();
	std::cout << "running NON cache_adaptive matrix multiply with matrices of size: " << (int)n << "x" << (int)n << "\n";
  //std::cout << "First input array\n";
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array.push_back(i);
		//std::cout << array[i] << " ";
	}
  //std::cout << std::endl;
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array.push_back(0);
	}
  conv_RM_2_ZM_RM(array.begin()+n*n,array.begin(),n,n);
	std::cout << "done converting first matrix\n";
	/*std::cout << "First input array in Z-MORTON\n";
	for (int i = 0; i < n*n; i++)
	{
		std::cout << input_1[i] << " ";
	}
	std::cout << std::endl;
	*/

  //std::cout << "Second input array\n";
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array[i] = n*n-i;
		//std::cout << array2[i] << " ";
	}
  //std::cout << std::endl;
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array.push_back(0);
	}
  conv_RM_2_ZM_CM(array.begin()+2*n*n,array.begin(),n,n);
	std::cout << "done converting second matrix\n";
  for (stxxl::uint64 i = 0 ; i < n*n; i++){
    array[i] = 0;
  }

	stxxl::timer start_p1;
	start_p1.start();

	STXXL_MSG("[LOG] Max MB allocated:  " << bm->get_maximum_allocation()/(1024*1024));
  mm_root(array.begin(),array.begin()+n*n,array.begin()+n*n*2,n);

	start_p1.stop();


	std::cout << "done multiplying matrix\n";
	//STXXL_MSG("[LOG] IO Statistics for sorting: "<<(stxxl::stats_data(*Stats) - stats_begin));
	STXXL_MSG("[LOG] Max MB allocated:  " << bm->get_maximum_allocation()/(1024*1024));
	std::cout << "[LOG] Total multiplication time: " <<(start_p1.mseconds()/1000) << "\n";
  //std::cout << "Result array\n";
  /*for (stxxl::uint64 i = 0 ; i < n*n; i++){
    std::cout << result[i] << " ";
  }*/
  //std::cout << std::endl;
  return 0;
}
