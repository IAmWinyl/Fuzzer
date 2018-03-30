#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

// Function prototype
void fuzz(vector <unsigned char> *arr);
void print_to_file(vector <unsigned char> arr, string jfName);
int run(string ffName, string jfName);

// Global variables
string fuzzFileName;

int main(int argc, char* argv[]) {

	vector <unsigned char> fileArray;
	int arrLength = 0, retVal = 0;

	// Check arguments
	if(argc > 1) {
		fuzzFileName = (string) argv[1];
	}
	else {
		cout << "Usage: ./fuzz <filename>" << endl;
		exit(-1);
	}

	// Start fuzzing
	fuzz(&fileArray);

	return 1;
}

void fuzz(vector <unsigned char> *arr) {
	unsigned char hex;
	int retVal = 0;
	string jfName;
	int jfNameCount = 0;


	// Loop through the 256 hex values
	for(int i = 0; i < 256; i++) {

		// Push value to array
		arr->push_back( (unsigned char) itoa(i,hex,16) );

		// Print value to file
		print_to_file(*arr, jfName);

		// Run file, check whether system code is SIGSEV
		if(run() == 11) {
			// Save file
			// Increment file name
			jfNameCount++;
		}
		else {
			// Delete file
		}


	}


	// SIGSEV return code is 11
	if(retVal == 11) {
		printf("[*] Segmentation fault detected: %s", to_string(jpgFileName) + ".jpg");
	}
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

int run(string jfName) {
	string command = "./" + fuzzFileName + " " + jfName;
	return system(command.c_str());
}

