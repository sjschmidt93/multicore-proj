#include "EasyBMP_1.06/EasyBMP.h"
using namespace std;

int main(int argc, char * argv[]){
	BMP img;
	img.ReadFromFile(argv[1]);
	for(int x = 0; x < img.TellHeight(); x++){
		for(int y = 0; y < img.TellWidth(); y++){
			cout << "(" << x << "," << y << ") = (" 
				<< (int) img(x,y)->Red <<  ","
				<< (int) img(x,y)->Green <<  ","
				<< (int) img(x,y)->Blue << ")" 
			<< endl;
		}
	}
	return 0;
}