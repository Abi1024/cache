#include <iostream>
#include <stxxl>
using namespace std;
const int B = 2;

#define TYPE int


void conv_RM_2_ZM_RM( TYPE *x, TYPE *xo, int n, int no )
{
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


void conv_ZM_RM_2_RM( TYPE *x, TYPE *xo, int n, int no )
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

void mm( TYPE *x, TYPE *u, TYPE *v, TYPE* y, int n0, int n)
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			TYPE *vv = v;
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

    for (int i = 0; i < n*n; i++){
      x[i] += y2[i];
			y2[i] = 0;
    }
	}
}

void mm_root(TYPE *x, TYPE *u, TYPE* v, int n){
  int extra_memory = 0;
  int n2 = n;
  while (n2 > B){
    extra_memory += n2*n2;
    n2 >>= 1;
  }
  //cout << "extra_memory " << extra_memory << endl;
  TYPE auxiliary_array[extra_memory];
  for (int i = 0; i < extra_memory; i++){
    auxiliary_array[i] = 0;
  }
  mm(x, u, v, auxiliary_array, n, n);
}


void conv_RM_2_ZM_CM( TYPE *x, TYPE *xo, int n, int no )
{
	if ( n <= B )
	{
		for ( int i = 0; i < n; i++ )
		{
			TYPE *xx = x + i;

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
  const int n = 16;
  TYPE array[n*n];
  //cout << "First input array\n";
  for (int i = 0 ; i < n*n; i++){
    array[i] = i;
    //cout << array[i] << " ";
  }
  //cout << endl;

  TYPE input_1[n*n];
  conv_RM_2_ZM_RM(input_1,array,n,n);

  //cout << "Second input array\n";
  for (int i = 0 ; i < n*n; i++){
    array[i] = n*n-i;
    //cout << array[i] << " ";
  }
  //cout << endl;

  TYPE input_2[n*n];
  conv_RM_2_ZM_CM(input_2,array,n,n);

  TYPE result[n*n];
  for (int i = 0 ; i < n*n; i++){
    result[i] = 0;
  }

  mm_root(result,input_1,input_2,n);
  cout << "Result array\n";
  for (int i = 0 ; i < n*n; i++){
    cout << result[i] << " ";
  }
  cout << endl;
  return 0;
}
