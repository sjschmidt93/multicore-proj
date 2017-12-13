#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <math.h>
#include <fstream>
#include <sstream>
#include <thread>
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

RGBApixel getAveragePixel(BMP img, int start_x, int start_y, int width, int height){

	int r_sum = 0;
	int g_sum = 0;
	int b_sum = 0;;
	int pixels = height * width;

	for(int x = start_x; x < start_x+width; x++){
		for(int y = start_y; y < start_y + height; y++){
			RGBApixel p = img.GetPixel(x,y);
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

RGBApixel getAveragePixel(BMP img){
	return getAveragePixel(img, 0, 0, img.TellWidth(), img.TellHeight());
}

/* may not be the best way to compare the color of two pixels but should
 be sufficient for now */
int getRGBDistance(RGBApixel p1, RGBApixel p2){
	return sqrt( pow(p1.Red - p2.Red, 2) + 
				 pow(p1.Green - p2.Green, 2) + 
				 pow(p1.Blue - p2.Blue,2));
}

string getClosestMatch(vector<pair < string, RGBApixel > > v, RGBApixel p){
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

int num_blocks = 0, block_size = 0, width = 0, height = 0, dim = 0;
vector<pair < string, RGBApixel > > v;
BMP input;
RGBApixel ** in_arr = NULL;
RGBApixel ** out_arr = NULL;

RGBApixel ** bmp2arr(BMP img){
	int d = img.TellWidth();
	RGBApixel ** arr = new RGBApixel*[d];
    for(int i = 0; i < d; i++)
    	arr[i] = new RGBApixel[d];
    for(int x = 0; x < d; x++)
    	for(int y = 0; y < d; y++)
    		arr[x][y] = img.GetPixel(x,y);
    return arr;
}

BMP arr2bmp(RGBApixel ** arr, int d){
	BMP img;
	img.SetSize(d, d);
	for(int x = 0; x < d; x++)
		for(int y = 0; y < d; y++)
			img.SetPixel(x,y,arr[x][y]);
	return img;
}

void freeArray(RGBApixel ** arr, int d){
	for(int i = 0; i < d; i++){
		if(arr[i] != NULL){
			delete[] arr[i];
			arr[i] = NULL;
		}
	}
	if(arr != NULL){
		delete[] arr;
		arr = NULL;
	}
}

void process(int min_y, int max_y){
	RGBApixel ** match_arr = NULL;
	for(int x = 0; x < width; x += block_size){
		for(int y = min_y; y < max_y; y += block_size){
			//cout << "(" << x << "," << y << ")" << endl;
			RGBApixel p = getAveragePixel(input, x, y, block_size, block_size);
			string s = getClosestMatch(v, p);
			BMP match;
			match.ReadFromFile(s.c_str());
			Rescale(match, 'H', block_size);
			RGBApixel ** match_arr = bmp2arr(match);
			for(int xx = 0; xx < block_size; xx++)
				for(int yy = 0; yy < block_size; yy++) 
					out_arr[x+xx][y+yy] = match_arr[xx][yy];
			//freeArray(match_arr, block_size);
		}
	}

}


int main(int argc, char * argv[]){

	int start = clock();
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

    dim = width;
    in_arr = bmp2arr(input);

	int r, g, b;
	string path;
	RGBApixel p;
	while(fp >> path >> r >> g  >> b){
		p.Red = r;
		p.Green = g;
		p.Blue = b;
		v.push_back(make_pair(path,p));
	}

	num_blocks = 16; // arbitrary for now
    block_size = dim / num_blocks;
    int blocks_per_thread = num_blocks / num_threads;

    out_arr = new RGBApixel*[dim];
    for(int i = 0; i < dim; i++)
    	out_arr[i] = new RGBApixel[dim];

    vector<thread> threads;
    for(int i =0; i < num_threads; i++)
    	threads.emplace_back(process, i * blocks_per_thread * block_size, blocks_per_thread * block_size * (i+1));
    for(auto & t: threads)
    	t.join();

    BMP output;
    output = arr2bmp(out_arr, dim);

    freeArray(in_arr, dim);
    freeArray(out_arr, dim);

    output.WriteToFile(argv[2]);
    cout << "Runtime for " << num_threads << " threads: " << (clock() - start) / double(CLOCKS_PER_SEC) << endl;
}