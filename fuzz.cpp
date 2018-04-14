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
int run(string jfName);


// Global variables
string fuzzFileName;

int main(int argc, char* argv[]) {

	vector <unsigned char> fileArray;

	// Check arguments
	if(argc > 1) {
		fuzzFileName = argv[1];
		// Check if file exists
		FILE *file = fopen(argv[1], "r");

		if(!file) {
			cout << "File does not exist." << endl;
			return 0;
		}
		else {
			fclose(file);
		}

	}
	else {
		cout << "Usage: ./fuzz <filename>" << endl;
		exit(-1);
	}

	// Initialize the first 256 spaces of vector with zeros
	fileArray.assign(256,0);

	// Start fuzzing
	fuzz(&fileArray);

	return 1;
}

void fuzz(vector <unsigned char> *arr) {
	char hexVal[16] = { };
	int retVal = 0;
	int jfNameCount = 0;
	int i, j, k;

	// Loop through the 256 hex values
	for(int i = 0; i < 256; i++) {
		// Push new value to array
		arr->back() = i;

		// Print value to file
		print_to_file(*arr, to_string(jfNameCount) + ".jpg");

		// Run file, check whether system code is SIGSEV
		if(run(to_string(jfNameCount) + ".jpg") == 11) {
			// Increment file name
			cout << "[*] Segmentation fault detected." << endl;
			jfNameCount++;

		}
	}


	// Loop through all of array
	for(k = distance( arr->begin(), arr->end() )-1; k > -1; k--) {

		// Reset values to zero
		fill(arr->begin(), arr->end(), 0);

		// Increment value of current position
		arr->at(k) += 1;

		// Assign j to second to last position
		j = distance( arr->begin(), arr->end() );

		// Loop starting from end towards current position
		while(j > k) {

			// Reset j to the end position
			j = distance( arr->begin(), arr->end() );

			// Loop through the 256 hex values
			for(i = 0; i < 256; i++) {

				// Push new value to array
				arr->back() = i;

				// Print value to file
				print_to_file(*arr, to_string(jfNameCount) + ".jpg");

				// Run file, check whether system code is SIGSEV
				if(run(to_string(jfNameCount) + ".jpg") == 11) {
					// Increment file name
					cout << "[*] Segmentation fault detected." << endl;
					jfNameCount++;

				}
			}

			j--;

			// If no overflow
			if(arr->at(j) != 255) {
				// Increment value of position j
				arr->at(j)++;
			}
			else {
				// While values are at overflow
				while(arr->at(j) == 255) {
					// Move position down
					j--;
				}
				// if current position isn't reached
				if(j > k) {
					// Increment value and zero the rest
					fill(arr->begin(), arr->end(), 0);
					arr->at(j)++;
				}
			}
		}
	}
}

void print_to_file(vector <unsigned char> arr, string jfName) {

	ofstream file;
	int arrLength = arr.size();

	file.open(jfName);

	// Loop array elements
	for(int i = 0 ; i < arrLength; i++) {
		file << static_cast<unsigned char>(arr[i]);
	}

	file.close();

}

int run(string jfName) {
	string command = "./" + fuzzFileName + " " + jfName + "> /dev/null 2>&1";
	return system(command.c_str());
}

