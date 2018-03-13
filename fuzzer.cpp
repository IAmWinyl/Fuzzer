#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

/*
	JFIF HEADER
	----------------------------------------------------------------------------------------------------------------------

	BYTE SOI[2]          Start of Image Marker					FF D8				
	BYTE APP0[2]         Application Use Marker					FF E0
	BYTE Length[2]       Length of APP0 Field					00 10
	BYTE Identifier[5]   "JFIF" (zero terminated) Id string 	4A 46 49 46 00
	BYTE Version[2]      JFIF Format Revision					01 01
	BYTE Units[1]        Units used for Resolution 				01
	BYTE Xdensity[2]     Horizontal Resolution 					00 78
	BYTE Ydensity[2]     Vertical Resolution 					00 78
  	BYTE XThumbnail[1]   Horizontal Pixel Count 				00
  	BYTE YThumbnail[1]   Vertical Pixel Count					00
  	BYTE FileType[2]	 Normal vs Progressive 					FF C0 || FF C2
  	BYTE SegLength[2]	 Segment Length
  	BYTE Sample[1]		 Sample
  	BYTE Height[2]		 Height
  	BYTE Width[2] 		 Width	
  	BYTE ColorMode[1]    Color Mode: RBG vs CMYK				03 || 04
	BYTE Trailer[2]		 EOF Marker 							FF D9

	----------------------------------------------------------------------------------------------------------------------

*/

void initialize_header(vector <unsigned char> *arr);
void print_to_file(vector <unsigned char> arr, string jfName);
int run(string ffName, string jfName);

int main(int argc, char* argv[]) {

	vector <unsigned char> fileArray;
	int arrLength = 0, jpgFileName = 0, retVal = 0;
	string fuzzFileName;


	if(argc > 1) {
		fuzzFileName = (string) argv[1];
	}
	else {
		cout << "Usage: ./fuzz <filename>" << endl;
		exit(-1);
	}

	cout << "[1] Initializing header ..." << endl;
	initialize_header(&fileArray);

	cout << "[2] Printing to file ..." << endl;
	print_to_file(fileArray, to_string(jpgFileName) + ".jpg");

	cout << "[3] Running program with file ..." << endl;
	retVal = run(fuzzFileName, to_string(jpgFileName) + ".jpg");

	// SIGSEV return code is 11
	if(retVal == 11) {
		printf("[*] Segmentation fault detected: %s", to_string(jpgFileName) + ".jpg");
	}

	// Increment file name for next JPG file
	jpgFileName++;

	return 1;
}

void initialize_header(vector <unsigned char> *arr) {

	// Load header
	arr->push_back( (unsigned char) 0xFF ); //file.SOI[0]
	arr->push_back( (unsigned char) 0xD8 ); //file.SOI[1]
	arr->push_back( (unsigned char) 0xFF ); //file.APP0[0]
	arr->push_back( (unsigned char) 0xE0 ); //file.APP0[1]
	arr->push_back( (unsigned char) 0x00 ); //file.Length[0]
	arr->push_back( (unsigned char) 0x10 ); //file.Length[0]
	arr->push_back( (unsigned char) 0x4A ); //file.Identifier[0]
	arr->push_back( (unsigned char) 0x46 ); //file.Identifier[1]
	arr->push_back( (unsigned char) 0x49 ); //file.Identifier[2]
	arr->push_back( (unsigned char) 0x46 ); //file.Identifier[3]
	arr->push_back( (unsigned char) 0x00 ); //file.Identifier[4]
	arr->push_back( (unsigned char) 0x01 ); //file.Version[0]
	arr->push_back( (unsigned char) 0x01 ); //file.Version[1]
	arr->push_back( (unsigned char) 0x01 ); //file.Units
	arr->push_back( (unsigned char) 0x00 ); //file.Xdensity[0]
	arr->push_back( (unsigned char) 0x78 ); //file.Xdensity[1]
	arr->push_back( (unsigned char) 0x00 ); //file.Ydensity[0]
	arr->push_back( (unsigned char) 0x78 ); //file.Ydensity[1]
	arr->push_back( (unsigned char) 0x00 ); //file.XThumbnail
	arr->push_back( (unsigned char) 0x00 ); //file.YThumbnail
	arr->push_back( (unsigned char) 0xFF ); //file.FileType[0]
	arr->push_back( (unsigned char) 0xC0 ); //file.FileType[1]
	arr->push_back( (unsigned char) 0x00 ); //file.SegLength[0]
	arr->push_back( (unsigned char) 0x00 ); //file.SegLength[1]
	arr->push_back( (unsigned char) 0x00 ); //file.Sample
	arr->push_back( (unsigned char) 0x00 ); //file.Height[0]
	arr->push_back( (unsigned char) 0x01 ); //file.Height[1]
	arr->push_back( (unsigned char) 0x00 ); //file.Width[]
	arr->push_back( (unsigned char) 0x01 ); //file.Width[]
	arr->push_back( (unsigned char) 0x03 ); //file.ColorMode
	arr->push_back( (unsigned char) 0xFF ); //file.Trailer[0]
	arr->push_back( (unsigned char) 0xD9 ); //file.Trailer[1]

}

void print_to_file(vector <unsigned char> arr, string jfName) {

	ofstream file;
	int arrLength = arr.size();

	file.open(jfName);

	// Loop array elements
	for(int i = 0 ; i < arrLength; i++) {
		file << arr[i];
	}

	file.close();

}

int run(string ffName,string jfName) {
	string command = "./" + ffName + " " + jfName;
	return system(command.c_str());
}

