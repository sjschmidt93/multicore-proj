#include "EasyBMP_1.06/EasyBMP.h"
#include <iostream>
#include <iomanip>
using namespace std;

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

bool cmpPixels(RGBApixel * p1, RGBApixel * p2){
	return p1->Red == p2->Red && p1->Green == p2->Green && p1->Blue == p2->Blue;
}

int main(int argc, char * argv[]){

	if(argc != 2){
		cout << " Usage: ./proj <input_bmp>" << endl;
		return 1;
	}

	BMP img;
	img.ReadFromFile(argv[1]);
	RGBApixel avg = getAveragePixel(img);
	printPixel(avg);
	// int height = img.TellHeight();
	// int width = img.TellWidth();
	// int pixels = height * width;

	// // BMP output;
	// // output.SetSize(height, width);

	// int count = pixels;
	// for(int x = 0; x < height - 1; x++){
	// 	for(int y = 0; y < width - 1; y++){
	// 		output.SetPixel(x,y,r);
	// 	}
	// }

	// output.WriteToFile("test_output.bmp");
	return 0;

}