#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <math.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
using namespace std;
using namespace boost::filesystem;

void printPixel(RGBApixel p){
	cout << "(" << (int) p.Red 
		 << "," << (int) p.Green 
		 << "," << (int) p.Blue << ")"
		 << endl;
}

RGBApixel getPixel(int r, int g, int b){
	RGBApixel p;
	p.Red = r;
	p.Green = g;
	p.Blue = b;
	return p;
}

int getRGBDistance(RGBApixel p1, RGBApixel p2){
	return sqrt( pow(p1.Red - p2.Red, 2) + 
				 pow(p1.Green - p2.Green, 2) + 
				 pow(p1.Blue - p2.Blue,2));
}

int num_blocks = 0, block_size = 0, width = 0, height = 0;
vector<pair < string, RGBApixel > > v;
BMP input;
RGBApixel ** output_arr = NULL;
RGBApixel ** input_arr = NULL;

RGBApixel getAveragePixel(int start_x, int start_y, int width, int height){

	int r_sum = 0;
	int g_sum = 0;
	int b_sum = 0;;
	int pixels = height * width;

	for(int x = start_x; x < start_x+width; x++){
		for(int y = start_y; y < start_y + height; y++){
			RGBApixel p = input_arr[x][y];
			r_sum += p.Red;
			g_sum += p.Green;
			b_sum += p.Blue;
		}
	}

	int r_avg = r_sum / pixels;
	int g_avg = g_sum / pixels;
	int b_avg = b_sum / pixels;

	RGBApixel p;
	p.Red = r_avg;
	p.Green = g_avg;
	p.Blue = b_avg;
	return p;
}

string getClosestMatch(RGBApixel p){
	string ret = "";
	double min = numeric_limits<double>::max();
	for(auto & it : v){
		RGBApixel candidate = it.second;
		double dist = getRGBDistance(candidate,p);
		if(dist < min){
			min = dist;
			ret = it.first;
		}
	}
	return ret;
}

void process(int id, int min_y, int max_y){
	int rows = max_y - min_y;	
	RGBApixel ** my_arr = new RGBApixel*[rows];
	for(int i = 0; i < rows; i++)
		my_arr[i] = new RGBApixel[width];
	int start = clock();

	for(int y = min_y; y < max_y; y += block_size){
		for(int x = 0; x < width; x += block_size){
			RGBApixel p = getAveragePixel(y, x, block_size, block_size);
			string s = getClosestMatch(p);
			BMP match;
			match.ReadFromFile(s.c_str());
			Rescale(match, 'H', block_size);
			RGBApixel match_arr[block_size][block_size];
			for(int xx = 0; xx < block_size; xx++)
				for(int yy = 0; yy < block_size; yy++) 
					my_arr[yy+y-min_y][x+xx] = match.GetPixel(yy,xx);
		}
	}
	for(int y = 0; y < max_y - min_y; y++)
		for(int x = 0; x < width; x++)
			output_arr[y+min_y][x] = my_arr[y][x];
	for(int i = 0; i < rows; i++)
		delete[] my_arr[i];
	delete[] my_arr;
}


int main(int argc, char * argv[]){

	auto start = std::chrono::high_resolution_clock::now();
	if(argc != 4){
		cout << " Usage: ./proj <input_img> <output_img> <num threads>" << endl;
		return 1;
	}

	std::ifstream fp("library-averages.txt");

	if(!fp.good()){
		cout << "ERROR: No library file. Run ./avg_lib <image library path>" << endl;
		return 1;
	}

	istringstream ss(argv[3]);
	int num_threads;
	if(!(ss >> num_threads)){
		cout << "ERROR: Conversion of argument for num_threads unsuccessful" << endl;
		return 1;
	}
	if(num_threads <= 0 || (num_threads & (num_threads -1))){
		cout << "ERROR: Number of threads must be >= 1 and a power of 2" << endl;
		return 1;
	}

    input.ReadFromFile(argv[1]);
    width = input.TellWidth();
    height = input.TellHeight();
    if(width != height){
    	cout << "ERROR: Input image must be a square." << endl;
    	return 1;
    }
    if((width == 0) || (width & (width-1))){
    	cout << "ERROR: Input image dimension must be a power of 2" << endl;
    	return 1;
    }
    int dim = width;
    output_arr = new RGBApixel*[dim];
    input_arr = new RGBApixel*[dim];
    for(int i = 0; i < dim; i++){
    	output_arr[i] = new RGBApixel[dim];
    	input_arr[i] = new RGBApixel[dim];
    }

    for(int y = 0; y < dim; y++)
    	for(int x = 0; x < dim; x++)
    		input_arr[y][x] = input.GetPixel(y,x);

	int r, g, b;
	string path;
	RGBApixel p;
	while(fp >> path >> r >> g  >> b){
		p.Red = r;
		p.Green = g;
		p.Blue = b;
		v.push_back(make_pair(path,p));
	}

	num_blocks = 256; // arbitrary for now
    block_size = height / num_blocks;
    int blocks_per_thread = num_blocks / num_threads;


    vector<thread> threads;
    for(int i =0; i < num_threads; i++)
    	threads.emplace_back(process, i, i * blocks_per_thread * block_size, blocks_per_thread * block_size * (i+1));
    for(auto & t: threads)
    	t.join();

    BMP output;
    output.SetSize(dim, dim);
    for(int x = 0; x < dim; x++)
    	for(int y = 0; y < dim; y++)
    		output.SetPixel(x,y,output_arr[x][y]);

    for(int i = 0; i < dim; i++){
    	delete[] output_arr[i];
    	delete[] input_arr[i];
    }
    delete[] output_arr;
    delete[] input_arr;

    output.WriteToFile(argv[2]);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
}