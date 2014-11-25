/*
 * TO BE COMPILED WITH C++11
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>
#include <map>
#include <tuple>
#include <iterator>
#include <fstream>

std::array<double, 81> tArray;
std::array<double, 87> sArray; 
std::vector<std::vector<double> > tsArray;
std::vector<std::vector<double> > memoizationStore;

#define MEMO 1
#define VERBOSE 0

unsigned count = 0;
unsigned distCalcCount = 0;


// calculate L1 distance
double calculateDistance(int t, int s) {
	distCalcCount++;
	return std::abs(tArray[t]-sArray[s]);
}

// recursive call for time alignment algorithm
double timeAlignment012(int t, int s) {
	std::vector<double> previousCost;
	double result;
	if (t==0) {
		if (s == 0) return calculateDistance(t,s) ;
		// else if (s== 1 || s==2) return tsArray[t][s] + 2;
		else return 1e8;
	}
	else if (s < 0) { // s < 0 is an invalid state anyway, so return infinity.
		return 1e8;
	}
	else {	
		if (memoizationStore[t][s] != -1) {
			result = memoizationStore[t][s];
			count++;
		}
		else {
			double val1 = 0, val2 = 0, val3 = 0;
			val1 = timeAlignment012(t-1, s) + 2;
			previousCost.push_back(val1);

			val2 = timeAlignment012(t-1, s-1);
			previousCost.push_back(val2);

			val3 = timeAlignment012(t-1, s-2) + 2;
			previousCost.push_back(val3);
			
			result = calculateDistance(t,s) + *(std::min_element(previousCost.begin(), previousCost.end()));
			if (MEMO)
				memoizationStore[t][s] = result;
		}
	}
	return result;
}

int main() {


	std::ifstream reffile, testfile;
	reffile.open("ref.dat", std::ifstream::in);
	testfile.open("test.dat", std::ifstream::in);

	std::string in,test;
	double refInput;
	double testInput;

	count = 0;
	while (std::getline(reffile, in)) {
		
		refInput = std::stod(in);
		tArray[count] = (refInput);
		count++;
	}
	count = 0;
	while(std::getline(testfile, test)) {
		testInput = std::stod(test);
		sArray[count] = (testInput);
		count ++;
	}


	std::cout << "tarray size: " << tArray.size() << std::endl;
	std::cout << "sarray size: " << sArray.size() << std::endl;

	std::vector<std::vector<double> > dProg(tArray.size(), std::vector<double>(sArray.size(), 1e8) );
	std::vector<std::vector<double> > backPointer(tArray.size(), std::vector<double>(sArray.size(), 1e8) );

	// construct distance matrix, initialize memoization matrix
	std::cout << "Constructing distance matrix, initializing memoization matrix..." << std::endl;
	for (unsigned t=0; t < tArray.size(); t++) {
		std::vector<double> svals;
		std::vector<double> memo;
		for (unsigned s=0; s < sArray.size(); s++) {
			// svals.push_back(std::abs(tArray[t]-sArray[s]));
			if (VERBOSE)
				std::cout << std::abs(tArray[t]-sArray[s]) << ", ";
			memo.push_back(-1);	
		}
		if (VERBOSE) 
			std::cout << std::endl;
		// tsArray.push_back(svals);
		memoizationStore.push_back(memo);
	}

	

	std::cout << "---" << std::endl;


	// Recursive variant 
	double result = timeAlignment012(tArray.size()-1, sArray.size()-1);

	std::cout << "Cost (Recursion): " << result << std::endl;

	std::cout << "Recalculations saved with memoization: " << count << std::endl;
	std::cout << "Distance calculations: " << distCalcCount << std::endl;

	

	// Dynamic programming variant
	distCalcCount = 0;
	
	for (unsigned s=0; s<sArray.size(); s++) {
		for (unsigned t=ceil(s/2.0); t<tArray.size(); t++) {
			if (s==0)
				dProg[t][s] = calculateDistance(t,s) + ((t == 0) ? 0 : 2);
			else if (s==1) {
				dProg[t][s] = calculateDistance(t,s) + std::min(dProg[t-1][s-1], dProg[t-1][s]+2);
			}
			else {
				std::vector<double> pcosts{dProg[t-1][s]+2, dProg[t-1][s-1], dProg[t-1][s-2]+2};
				dProg[t][s] = calculateDistance(t,s) + *std::min_element(pcosts.begin(), pcosts.end());
				backPointer[t][s] = std::distance(pcosts.begin(), std::min_element(pcosts.begin(), pcosts.end()));
			}
		}	
	}

	if (VERBOSE) {
		std::cout << "Dynamic programming results..." << std::endl;
		for (unsigned t=0; t<tArray.size(); t++) {
			for (unsigned s=0; s<sArray.size(); s++) {
				std::cout << dProg[t][s] << " " ;
			}
			std::cout << std::endl;
		}
	}

	std::cout << "---" << std::endl;
	std::cout << "Cost (Dynamic programming): " << dProg[tArray.size()-1][sArray.size()-1] << std::endl;
	std::cout << "Distance calculations: " << distCalcCount << std::endl;

	int x = backPointer.size()-1;
	int y = backPointer[0].size()-1;

	std::vector<std::tuple<double,double> > path;
	path.push_back(std::make_tuple(x,y));

	// Trace backpointer
	while (x > 0 && y > 0) {
		// std::cout << "x:" << x << " y:" << y << std::endl;
		int cell = backPointer[x][y];
		switch (cell) {
			case 0:
				path.insert(path.begin(), std::make_tuple(x-1, y));
				break;
			case 1:
				path.insert(path.begin(), std::make_tuple(x-1, y-1));
				break;
			case 2:
				path.insert(path.begin(), std::make_tuple(x-1, y-2));
				break;
		}
		x = x - 1;
		y = y - cell;
	}
	for (std::tuple<double, double>& t : path) {
		std::cout << std::get<1>(t) << " -> ";
	}
	std::cout << std::endl;

	return 0;
}