#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <iostream>
#include <math.h>

#ifndef TYPE
#define TYPE long long int
#endif

#ifndef maxN
#define maxN 128
#endif

#ifndef BASECPU
#define BASECPU 16
#endif

#ifndef MAX_NUM
#define MAX_NUM 500
#endif

int N, B;
TYPE *X, *U, *V;
TYPE *XO, *UO, *VO;

#ifdef debug

TYPE *A, *BB, *C;
void MatrixMultiplication(int N){
	//A[:]=0;

	for(int i=0;i<N; i++)
	{
		for(int j=0;j<N;j++)
		{   A[i*N+j]=0;
			for(int k=0;k<N;k++)
			{
				A[i*N+j]=A[i*N+j]+(BB[i*N+k]*C[k*N+j]);

			}

		}
	}

}
#endif

int getMilliCount( void )
{
	timeb tb;

	ftime( &tb );

	int nCount = tb.millitm + ( tb.time & 0xfffff ) * 1000;

	return nCount;
}

int getMilliSpan( int nTimeStart )
{
	int nSpan = getMilliCount( ) - nTimeStart;

	if ( nSpan < 0 ) nSpan += 0x100000 * 1000;

	return nSpan;
}

// X is output array
// XO is input array
// n is length of this recursive call's array
// no is length of root array from input


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


/*
ZM_RM : hybrid Z-Morton row-major
RM: row-major<F12>
 */

// X is input array
// XO is output array
// n is length of this recursive call's array
// no is length of root array from input
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


/*
RM: row-major
ZM_CM : hybrid Z-Morton column-major
 */

// X is output array
// XO is input array
// n is length of this recursive call's array
// no is length of root array from input
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

// x = u * v
// n * n matrices

void mm( TYPE *x, TYPE *u, TYPE *v, int n )
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


int main ( int argc , char *argv[ ] )
{
	N = 0;
	if ( argc > 1 ){
		N = atoi( argv[ 1 ] );
		// maxN = N;
	}
	if ( N <= 0 ) N = maxN;

	int k = 0;

	while ( N )
	{
		N >>= 1;
		k++;
	}

	if ( k > 1 ) N = ( 1 << ( k - 1 ) );
	else N = 1;

	B = 0;
	if ( argc > 2 ) B = atoi( argv[ 2 ] );
	if ( B <= 0 ) B = BASECPU;

	k = 0;

	while ( B )
	{
		B >>= 1;
		k++;
	}

	if ( k > 1 ) B = ( 1 << ( k - 1 ) );
	else B = 1;

	if ( B > N ) B = N;

	printf( "N = %d, B = %d\n", N, B);

		
	//RM input arrays
	// X = UO * VO
	X  = new TYPE[N * N];
	UO  = new TYPE[N * N];
	VO  = new TYPE[N * N];

#ifdef debug
	//used in naive multiplication
	// A = BB * C
	A = new TYPE[N*N];
	BB = new TYPE[N*N];
	C = new TYPE[N*N];
	
#endif


	srand( time( NULL ) );
	//generating random data into input matrices
	for ( int i = 0; i < N * N; i++ )
	{
		X[ i ] = 0;
		UO[ i ] = ( rand( ) % MAX_NUM );
		VO[ i ] = ( rand( ) % MAX_NUM );
		//UO[ i ] = ( i % N == i / N ); //build identity matrix
		//VO[ i ] = ( i % N == i / N);
#ifdef debug
		//storing for correctness validation
		BB[i]=UO[ i ];
		C[i]=VO[ i ];
#endif
	}
	// U is ZM_RM input array
	U  = new TYPE[N * N];
	conv_RM_2_ZM_RM( U, UO,  N, N );
	delete[] UO;

	// V is ZM_CM input array
	V = new TYPE[N * N];
	conv_RM_2_ZM_CM( V, VO,  N, N );
	delete[] VO;

	float tstart = getMilliCount( );

	//Matrix Multiply X = U * V 
	mm ( X, U, V, N );

	float tend = getMilliSpan( tstart );
	
	//XO is answer converted back to RM
	XO = new TYPE[N * N];

	for(int i = 0 ; i < N*N; i++)
		XO[i]=0;
	// XO[0:N*N]=0;
	conv_ZM_RM_2_RM( X, XO,  N, N );
#ifdef debug
	std::cout << "\nFirst Matrix: " << std::endl;
	for(int i=0; i< N; i++)
	{
		for(int j=0; j< N; j++)
			std::cout<< U[i + N*j] << " ";
		std::cout << "\n";
	}
	std::cout << "\nSecond Matrix: " << std::endl;
	for(int i=0; i< N; i++)
	{
		for(int j=0; j< N; j++)
			std::cout<< V[i + N*j] << " ";
		std::cout << "\n";
	}

	std::cout << "\nRaw Result: " << std::endl;
	for(int i=0; i< N; i++)
	{
		for(int j=0; j< N; j++)
			std::cout<< X[i + N*j] << " ";
		std::cout << "\n";
	}
#endif
	std::cout << "\nRow Major Result: " << std::endl;
	for(int i=0; i< N; i++)
	{
		for(int j=0; j< N; j++)
			std::cout<< XO[i + N*j] << " ";
		std::cout << "\n";
	}


#ifdef debug
	std::cout << "\nChecking Correctness..." << std::endl;
	MatrixMultiplication(N);

	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			if(XO[i*N+j] - A[i*N+j] != 0)
			{
				std::cout << "Wrong at:" << i << "," << j << std::endl;
				std::cout << "A = " << A[i+N*j] << ", " << "XO = " << XO[i+N*j] << std::endl;
				std::cout << "Difference: " << XO[i+N*j] - A[i+N*j] << std::endl;
				exit(1);
			}
		}
	}
	std::cout << "\nCorrect!" << std::endl;
#endif

	//  printf( "p = %d, T_p = %lf ( ms )\n\n", __cilkrts_get_nworkers( ), tend );

	delete[] X;
	delete[] V;
	delete[] U;
	delete[] XO;

	return 0;
}
