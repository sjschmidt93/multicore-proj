#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost::filesystem;

void printPixel(RGBApixel p){
	cout << "(" << (int) p.Red 
		 << "," << (int) p.Green 
		 << "," << (int) p.Blue << ")"
		 << endl;
}

RGBApixel getAveragePixel(BMP img){

	int r_sum = 0;
	int g_sum = 0;
	int b_sum = 0;;
	int height = img.TellHeight();
	int width = img.TellWidth();
	int pixels = height * width;

	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
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

int main(int argc, char * argv[]){

	// if(argc != 4){
	// 	cout << " Usage: ./proj <img lib path> <input_img> <output_img>" << endl;
	// 	return 1;
	// }

	BMP img;
	path p(argv[1]);
    for (auto i = directory_iterator(p); i != directory_iterator(); i++)
    {
    	BMP img;
    	img.ReadFromFile(i->path().string().c_str());
    	RGBApixel p = getAveragePixel(img);
    	printPixel(p);
    }

	return 0;

}