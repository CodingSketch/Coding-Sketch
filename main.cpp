#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <numeric>
#include <string>

#include "include/Choose_Ske.h"


#define x (1.05)
double F = 0.15, S = 4.4;
using namespace std;
vector<string> items;
unordered_map<string, int>freq;

int memaccess_i = 0, memaccess_q = 0, memaccess_d = 0;

int thres_mid = 16, thres_large = 1024;

double item_num = 0, flow_num = 0;
vector<double> experiment_ARE, experiment_AAE,experiment_CON, experiment_through_insert, experiment_through_query/*, experiment_sum, exper_max*/;

int D,CHOOSE=8;

void readFile_CAIDA(string filename, int length, int MAX_ITEM){//for zipf：length must be 4
	ifstream inFile(filename, ios::binary);

	if (!inFile.is_open())
		cout << "File fail." << endl;

	int max_freq = 0;
	char key[length];
	for (int i = 0; i < MAX_ITEM; ++i)
	{
		inFile.read(key, length);
		if (inFile.gcount() < length)
			break;
		items.push_back(string(key, length));
		freq[string(key, length)]++;
	}
	inFile.close();

	for (auto pr : freq)
		max_freq = max(max_freq, pr.second);

	vector<int> fsd;
	for (auto pr : freq)
		fsd.push_back(pr.second);
//	nth_element(fsd.begin(), fsd.begin() + fsd.size() * .99, fsd.end());
	thres_large = fsd[fsd.size() * .99];

	/*
	//sort freq
	vector<pair<string, int>> arr;
	for(auto it : freq){
	  arr.emplace_back(it);
	}
	sort(arr.begin(), arr.end(), [](auto p1, auto p2){return p1.second == p2.second ? p1.first > p2.first : p1.second > p2.second; });
	freq.clear();
	int count = 0;
	for(auto [key, value]: arr)
	{
		if (value < 32) continue;
		//count++; if (count>10000) break;
		freq[key] = value;
	}
	*/

	item_num = items.size();
	flow_num = freq.size();
	cout << "dataset name: " << filename << endl;
	cout << freq.size() << "flows, " << items.size() << " items read" << endl;;
	cout << "max freq = " << max_freq << endl;
	// cout << "large flow thres = " << thres_large << ", middle flow thres = " << thres_mid << endl << endl;
}
void calcAcc(const vector<int>* ret_f)
{
	double _ARE = 0, _AAE = 0, _CON = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		cout<<"round:"<<i<<" ";
		int z = 0;
		double temp_ARE = 0, temp_AAE = 0;
		for (auto pr : freq)
		{
			int est_val = ret_f[i][z++];
			int real_val = pr.second;
			int dist = abs(est_val - real_val);
			int conflict = est_val != real_val ? 1 : 0;

			_ARE += (double)dist / real_val, _AAE += dist, _CON += conflict;
			temp_ARE += (double)dist / real_val, temp_AAE += dist;
			//cout<<est_val<<','<<real_val<<endl;
		}
		cout<<fixed<<temp_ARE/freq.size()<<" "<<temp_AAE/freq.size()<<endl;
	}
	//cout<<"size "<<freq.size()<<endl;
	_ARE /= freq.size(), _AAE /= freq.size(), _CON /= freq.size();
	_ARE /= testcycles, _AAE /= testcycles, _CON /= testcycles;
	experiment_ARE.push_back(_ARE);
	experiment_AAE.push_back(_AAE);
	experiment_CON.push_back(_CON);
	cout << fixed << "ARE = " << _ARE << ", AAE = " << _AAE << endl;/*<< ", insert = " << throughput_i << ", query = " << throughput_o << endl;*/
}
void test_BCM(int mem_in_byte, int ratio)
{
	int d = D;  //counts of hash function
	int w = mem_in_byte;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;

	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;
		Sketch *bcm;
		bcm=Choose_Sketch(w,d, i * 101,CHOOSE);

		//test bound
		/*
		int layer0_len=bcm->layer0_len();
		int to_insert = layer0_len / 10 * ratio;
		for (int j=0;j<to_insert;++j)
		{
			for (int temp=0;temp<16;++temp)
				bcm->temp_insert(j,1);
		}
		*/
		//test bound end
		

		//bcm->check();
// puts("alb");
                 clock_gettime(CLOCK_MONOTONIC, &time1);
        
		for (auto key : items){
			bcm->Insert(key.c_str());
			// bcm->check();
		}
		
		puts("insert over");
                 clock_gettime(CLOCK_MONOTONIC, &time2);

                 clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += bcm->Query(pr.first.c_str());
                 clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;
				
		for (auto pr : freq)
			ret_f[i].push_back(bcm->Query(pr.first.c_str()));		
	}

	// cout << "sum: " << estimate_sum/testcycles << "\tBCM" << endl;
	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);                                             
	calcAcc(ret_f);
}
int main(int argc, char** argv){
	
	char filename[100];
	sprintf(filename,"CAIDA");

	//string datasetname = "/share/zipf_2022/zipf_0.9.dat";
	string datasetname = argv[2];
	string::size_type idx = datasetname.find( "zipf" );
	int length = 0;
	if ( idx != string::npos ) length = 4;
	else length = 13;
	cout<<length<<endl;
	readFile_CAIDA(datasetname, length, 1000000);
	// "/share/CAIDA_2018/00.dat" "/share/zipf_2022/zipf_0.3.dat" /share/datasets/campus/campus.dat 13 4 13

	CHOOSE=atoi(argv[1]);
	mxtimes=atoi(argv[3]);
	
	printf("mem 1--5: %d, %d, %d, %d, %d\n",mem[1], mem[2], mem[3], mem[4], mem[5]);
	
	int mem_add = (1 << 20);
	int mem_in_byte_start = mem_add * 1;
	int mem_in_byte_end = mem_add * 16;
	int mem_in_byte;
	cout.precision(7);
	for(D=3;D<=3;D+=2)
	{
		for (mem_in_byte = mem_in_byte_start; mem_in_byte <= mem_in_byte_end; mem_in_byte += mem_add)
		{
			printf("memory used: (1<<%.0f)*%d\n", log2(mem_add), mem_in_byte/mem_add);
			//cout<<"memory used: "<<log2(mem_add)<<endl;
			test_BCM(mem_in_byte,mxtimes);		}
	}

	printf("\n");
	ofstream oFile;
	char oFilename[50];
	sprintf(oFilename,"sketch%d.csv",CHOOSE);
	oFile.open(oFilename, ios::app);
	if (!oFile) return 0;
	oFile << "Dataset:" << filename << endl;
	oFile <<"Memory," << "AAE," << "ARE," << "insert throughput," << "query throughput," << endl;

	mem_add=mem_in_byte_start; 
	for (int o = 0, j = 0, mem = mem_in_byte_start; o < experiment_AAE.size(); o++,mem += mem_add)
	{
		oFile << log2(mem) <<","<< experiment_AAE.at(o) << "," << experiment_ARE.at(o) << "," << experiment_through_insert.at(o) << ","
			<< experiment_through_query.at(o)
			/*<<","<< experiment_CON.at(o) << "," << fixed << experiment_sum.at(o) << "," << fixed << exper_max.at(o)*/ << endl;
		
	}
	oFile.close();

	cout<<"AAE\n";
	for (int o = 0, j = 0, mem = mem_in_byte_start; o < experiment_AAE.size(); o++,mem += mem_add)
		cout << experiment_AAE.at(o) <<endl;
	cout<<"ARE\n";
	for (int o = 0, j = 0, mem = mem_in_byte_start; o < experiment_AAE.size(); o++,mem += mem_add)
		cout << experiment_ARE.at(o) <<endl;
	cout << "dataset name: " << datasetname <<"\nmxtimes: "<<"\033[34m"<<mxtimes<<"\033[0m"<< endl;
	cout << "sketch name: " <<"\033[34m"<< sketch_name <<"\033[0m"<< endl;
	return 0;
}
