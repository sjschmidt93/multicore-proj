#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <fstream>
using namespace std;
using namespace boost::filesystem;

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

int main(int argc, char * argv[]){
	if(argc != 2){
		cout << "Usage: ./avg_lib <image library path>" << endl;
		return 1;
	}
	BMP img;
	path p(argv[1]);
	std::ofstream fp;
	fp.open("library-averages.txt");

    for (auto i = directory_iterator(p); i != directory_iterator(); i++){
    	BMP img;
    	string path_str = i->path().string();
    	img.ReadFromFile(path_str.c_str());
    	RGBApixel p = getAveragePixel(img);
    	fp << path_str << " " << (int) p.Red << " " << (int) p.Green << " " << (int) p.Blue << endl;
    }
	fp.close();
}