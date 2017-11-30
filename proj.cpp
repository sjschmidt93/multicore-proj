#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <math.h>
using namespace std;
using namespace boost::filesystem;

const int block_size = 16;

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

void createTestBMPs(){
	BMP white, black;
	white.SetSize(block_size, block_size);
	black.SetSize(block_size, block_size);
	RGBApixel black_p = getPixel(0,0,0);
	RGBApixel white_p = getPixel(255,255,255);
	RGBApixel other_p = getPixel(255,0,0);
	int area = block_size * block_size;
	for(int x = 0; x < block_size; x++){
		for(int y = 0; y < block_size; y++){
			if((x + y) % (area / 12) == 0){
				white.SetPixel(x,y,other_p);
				black.SetPixel(x,y,other_p);
			}
			else{
				white.SetPixel(x,y,white_p);
				black.SetPixel(x,y,black_p);
			}
		}
	}
	white.WriteToFile("white-test.bmp");
	black.WriteToFile("black-test.bmp");
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

int main(int argc, char * argv[]){

	// if(argc != 4){
	// 	cout << " Usage: ./proj <img lib path> <input_img> <output_img>" << endl;
	// 	return 1;
	// }

	//createTestBMPs();

	BMP img;
	path p(argv[1]);
	vector< pair< string, RGBApixel > > v;
    for (auto i = directory_iterator(p); i != directory_iterator(); i++){
    	BMP img;
    	string path_str = i->path().string();
    	img.ReadFromFile(path_str.c_str());
    	RGBApixel p = getAveragePixel(img);
    	v.push_back( make_pair(path_str, p) );
    }

    BMP input, output;
    input.ReadFromFile(argv[2]);
    int width = input.TellWidth();
    int height = input.TellHeight();
    output.SetSize(height,width);
    int count = 0;
    for(int x = 0; x < width; x += block_size){
    	for(int y = 0; y < height; y += block_size){
    		RGBApixel p = getAveragePixel(input, x, y, block_size, block_size);
    		count++;
    		string match = getClosestMatch(v, p);
    		BMP img_match;
    		img_match.ReadFromFile(match.c_str());
    		for(int xx = 0; xx < block_size; xx++)
    			for(int yy = 0; yy < block_size; yy++)
    				output.SetPixel(x+xx,y+yy,img_match.GetPixel(xx, yy));
    	}
    }
    output.WriteToFile("test-output.bmp");
    //cout << "Count:" << count << endl;
    //cout << "blocks:" << (height * width) / (block_size * block_size) << endl;
	return 0;

}