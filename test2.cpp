#include <iostream>
#include <stxxl/vector>
const int B = 2;

#define TYPE int
const int CACHE_SIZE = 64;
typedef stxxl::VECTOR_GENERATOR<TYPE,4,CACHE_SIZE >::result vector_type;
typedef stxxl::vector<TYPE,4,stxxl::lru_pager<CACHE_SIZE>>::iterator itr;

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
  const stxxl::uint64 n = 1000;
  vector_type array;
  //std::cout << "First input array\n";
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array.push_back(i);
		//std::cout << array[i] << " ";
	}
  //std::cout << std::endl;

  vector_type input_1;
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		input_1.push_back(0);
	}
  conv_RM_2_ZM_RM(input_1.begin(),array.begin(),n,n);

	/*std::cout << "First input array in Z-MORTON\n";
	for (int i = 0; i < n*n; i++)
	{
		std::cout << input_1[i] << " ";
	}
	std::cout << std::endl;
	*/

	vector_type array2;
  //std::cout << "Second input array\n";
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		array2.push_back(n*n-i);
		//std::cout << array2[i] << " ";
	}
  //std::cout << std::endl;

  vector_type input_2;
	for (stxxl::uint64 i = 0; i < n*n; i++)
	{
		input_2.push_back(0);
	}
  conv_RM_2_ZM_CM(input_2.begin(),array2.begin(),n,n);

  vector_type result;
  for (stxxl::uint64 i = 0 ; i < n*n; i++){
    result.push_back(0);
  }

  mm_root(result.begin(),input_1.begin(),input_2.begin(),n);
  //std::cout << "Result array\n";
  for (stxxl::uint64 i = 0 ; i < n*n; i++){
    //std::cout << result[i] << " ";
  }
  //std::cout << std::endl;
  return 0;
}
