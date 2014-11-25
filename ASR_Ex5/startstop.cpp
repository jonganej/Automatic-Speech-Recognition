#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h>

const double infinity = 1e20;
int mCount = 0;

std::vector<std::vector<double> > memo, globalCost, backPointer;
std::vector<double> data;

void read(const std::string& fname, std::vector<double>& l) {
	std::ifstream fstream(fname);
	if (fstream) {
		std::string buffer;
		while (std::getline(fstream, buffer)) {
			if (buffer.size()>0)
				l.push_back(::atof(buffer.c_str()));
		}
	}
	else {
		std::cerr << fname << "could not be opened." << std::endl;
	}
}

void initialize(unsigned nMeasurements, unsigned k) {
	for (unsigned j=0; j<=k; j++) {
		std::vector<double> n(nMeasurements+1, infinity);
		if (j==0) 
			n[0] = 0;
		globalCost.push_back(n);
		backPointer.push_back(n);
	}

}

double mean(unsigned start, unsigned stop) {
	double sum = 0.0;
	for (unsigned i=start; i<=stop; i++) {
		sum += data[i];
	}
	return (sum*1.0 / (stop-start+1));
}

double sumSqErrs(unsigned start, unsigned stop) {
	double meanVal = mean(start, stop);
	double sum = 0;
	for (unsigned i=start; i<=stop; i++) {
		sum += (data[i] - meanVal)*(data[i] - meanVal);
	}
	return sum;
}

double localCostEx3(unsigned start, unsigned stop) {
	
	if (start == stop) return 0;
	if (start > stop) return 0;
	
	return sumSqErrs(start, stop);
}

double negativeLog(unsigned start, unsigned stop) {
	if (start == stop) return infinity;
	if (start > stop) return infinity;
	double res = (stop - start + 1) * 0.5 * (std::log(sumSqErrs(start, stop)/(stop - start + 1))+std::log(2*M_PI)+1);
	return res;
}

void globalCostFn(unsigned K, unsigned T) {
	
	for (unsigned kt=1; kt<=K; kt++) {
		for (unsigned t=1; t<=T; t++) {
			double minimum = infinity;
			unsigned minIdx = 0;
			for (unsigned m=0; m<=t; m++) {
				double score = globalCost[kt-1][m] + negativeLog(m+1,t);
				if (score <  minimum) {
					minimum = score;
					minIdx = m;
				}
			}
			globalCost[kt][t] = minimum;
			backPointer[kt][t] = minIdx;
		}
	}	
}


int main() {
	read("probe1.ascii", data);
	unsigned nSegments = 3, nMeasurements = data.size();

	initialize(nMeasurements, nSegments);

	globalCostFn(3, data.size());

	// for (int k=nSegments; k>=0; k--) {
	// 	for (int t=0; t<nMeasurements; t++) {
	// 		std::cout << globalCost[k][t] << ",";
	// 	}
	// 	std::cout << std::endl;
	// }
    
	int tk = nMeasurements;
	for (int k=nSegments; k>=1; k--) {
		tk = backPointer[k][tk];
		std::cout << tk << std::endl;
	}
}