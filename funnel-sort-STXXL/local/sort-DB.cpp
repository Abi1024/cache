#define USE_STXXL
#include <stxxl.h>
#include "funnelSort.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <ctime>

using namespace std;

class record
{
	public:
		char key[11], data1[33], data2[53];
};

class record_comparator
{
	public:
		bool operator () (const record& a, const record& b) const
		{
			return strcmp(a.key, b.key) < 0 || (strcmp(a.key, b.key) == 0 && strcmp (a.data1, b.data1) < 0);
		}
};


void print_usage(const char* program)
{
        cout<<"USAGE: "<<program<<" file-to-be-sorted destination-file"<<endl;
        cout<<"By default storing the sorted database is disabled, to enable it you need to uncomment the write back in my code"<<endl;
        cout<<"So just provide a dummy filename"<<endl;
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		print_usage(argv[0]);
		return 1;
	}
	std::cout << "At start of funnel sort\n";
	ifstream in;
	in.open(argv[1]);

	stxxl::timer start_t, sort_start_t;

	ofstream out;
	out.open(argv[2], ios::trunc);
	start_t.start();

	int record_counter = 0;
	record_comparator comp;
	while(!in.eof())
	{
		record_counter++;
		string temp_string;
		getline(in, temp_string);
	}

	record *records;
	records = new record[record_counter];
	in.close();
	in.open(argv[1]);
	record_counter = 0;
	while(!in.eof())
	{
		in>>records[record_counter].key>>records[record_counter].data1>>records[record_counter].data2;
		record_counter++;
	}
	start_t.stop();
	std::cout << "Starting to sort\n";
	stxxl::stats* Stats = stxxl::stats::get_instance();
        stxxl::stats_data stats_begin(*Stats);
	stxxl::block_manager * bm = stxxl::block_manager::get_instance();
	STXXL_MSG("[LOG] FunnelSort. DB Used: "<<argv[1]);
	STXXL_MSG("[LOG] Total initializing time: "<<(start_t.mseconds()/1000));
	sort_start_t.start();
	FunnelSort::sort<class record, class record_comparator>(&records[0], &records[record_counter], comp);
	STXXL_MSG("[LOG] IO Statistics for sorting: "<<(stxxl::stats_data(*Stats) - stats_begin));
	STXXL_MSG("[LOG] Max MB allocated: " << bm->get_maximum_allocation()/(1024*1024));  // max. number of bytes allocated until now

	sort_start_t.stop();
        STXXL_MSG("[LOG] Total sorting time: "<<(sort_start_t.mseconds()/1000));
	STXXL_MSG("[LOG] Total process time: "<<(start_t.mseconds()/1000 + sort_start_t.mseconds()/1000));

	std::cout << "Done with sort\n";
	/*
	for(int i = 0 ; i < record_counter ; i ++)
		out<<"Post-sort Record: "<<i<<" | KEY: "<<records[i].key<<endl;
	*/
	in.close();
	out.close();
	return 0;
}
