#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <string>

#define FILE_OUTPUT
// #define CONSOLE_OUTPUT

// definition of data types
typedef std::string Word;
typedef std::vector<Word> WordList;
typedef std::vector<WordList> SentenceList;

enum EditOperation {NO, SUB, INS, DEL};
struct AlignmentElement {
    Word spoken;
    Word recog;
    EditOperation operation;

    AlignmentElement(const Word & spoken, const Word & recog, EditOperation operation) :
	spoken(spoken), recog(recog), operation(operation) {}
};
typedef std::vector<AlignmentElement> Alignment;



// i/o-functionality
void readSentences(std::ifstream & is, SentenceList & s);
void writeAlignment(const Alignment & a, std::ostream & o);


// main
void usage();
int main(int argc, char* argv[]);



// -----------------------------------------------------------------------------
// i/o-functionality

void readSentences(std::ifstream & is, SentenceList & sentences) {
    std::string buffer;
    Word word;
    while (std::getline(is, buffer)) {
	if (buffer.size() == 0) 
	    continue;
	std::istringstream line(buffer.c_str());
	WordList sentence;
	while (line >> word) 
	    sentence.push_back(word);
	sentences.push_back(sentence);
    }
}

/*
  How to use writeAlignment:

  1: Alignment alignment;
  2: alignment.push_back(AlignmentElement("HELLO", "HOW", SUB));
  3: alignment.push_back(AlignmentElement("",      "LOW", INS));
  4: writeAlignment(alignment, std::cout);
*/
void writeAlignment(const Alignment & alignment, std::ostream & os) {
    std::ostringstream ossSpoken, ossRecog, ossOperation;
    int length;
    for (Alignment::const_iterator itAlignment = alignment.begin();
	 itAlignment != alignment.end(); ++itAlignment)
	switch (itAlignment->operation) {
	case NO:
	    length = std::max(itAlignment->spoken.size(), itAlignment->recog.size()) + 1;
	    ossSpoken    << std::left << std::setw(length) << itAlignment->spoken;
	    ossRecog     << std::left << std::setw(length) << itAlignment->recog;
	    ossOperation << std::left << std::setw(length) << " ";
	    break;
	case SUB:
	    length = std::max(itAlignment->spoken.size(), itAlignment->recog.size()) + 1;
	    ossSpoken    << std::left << std::setw(length) << itAlignment->spoken;
	    ossRecog     << std::left << std::setw(length) << itAlignment->recog;
	    ossOperation << std::left << std::setw(length) << "S";
	    break;
	case INS:
	    length = itAlignment->recog.size() + 1;
	    ossSpoken    << std::left << std::setw(length) << " ";
	    ossRecog     << std::left << std::setw(length) << itAlignment->recog;
	    ossOperation << std::left << std::setw(length) << "I";
	    break;
	case DEL:
	    length = itAlignment->spoken.size() + 1;
	    ossSpoken    << std::left << std::setw(length) << itAlignment->spoken;
	    ossRecog     << std::left << std::setw(length) << " ";
	    ossOperation << std::left << std::setw(length) << "D";
	    break;
	}
    os << std::endl;
    os << ossSpoken.str()    << std::endl;
    os << ossRecog.str()     << std::endl;
    os << ossOperation.str() << std::endl;
}

// String compare
bool isEqual(const std::string& s1, const std::string& s2) { return (0 == s1.compare(s2));}

void outputGrid(const std::vector<std::vector<std::tuple<int,int> > >& vecOfVec, std::ostream & os) {
	for (unsigned i=0; i<vecOfVec.size(); i++) {
		for (unsigned j=0; j<vecOfVec[i].size(); j++) {
			os << std::get<0>(vecOfVec[i][j]) << "," << std::get<1>(vecOfVec[i][j]) << " | " ;
		}
		os << std::endl;
	}
}

template<typename T>
void outputGrid(const std::vector<std::vector<T> >& vecOfVec, std::ostream & os) {
	for (unsigned i=0; i<vecOfVec.size(); i++) {
		for (unsigned j=0; j<vecOfVec[i].size(); j++) {
			os << vecOfVec[i][j] << "  " ;
		}
		os << std::endl;
	}
}

int wordCount(const SentenceList& sl) {
	int wordcount = 0;
	for (const WordList& wl : sl) {
		for (const Word& w: wl) {
			wordcount++;
		}
 	}
 	return wordcount;
}


// -----------------------------------------------------------------------------
// main

void usage(const char * basename) {
    std::cerr << " usage: " << basename 
	      << " <spoken-sentences-file> <recognized-sentences-file>" 
	      << std::endl;
    std::exit(1);
}

int main(int argc, char* argv[]){
    // check for correct number of command line arguments
    if (argc != 3)
	usage(argv[0]);
	

    // read spoken and recognized sentences from files
    SentenceList spoken, recog;
    {
		std::ifstream spokenStream(argv[1]);
		if (!spokenStream) {
		    std::cerr << std::endl << "ERROR: " << argv[1] << " could not be opened" 
			      << std::endl;
		    return 2;
		}
		std::ifstream recogStream(argv[2]);
		if (!recogStream) {
		    std::cerr << std::endl << "ERROR: " << argv[2] << " could not be opened" 
			      << std::endl;
		    return 2;
		}

		readSentences(spokenStream, spoken);
		readSentences(recogStream, recog);
		if (spoken.size() != recog.size()){
		    std::cerr << std::endl << "ERROR: Number of sentences do not match!" 
			      << " (" << spoken.size() << "/" << recog.size() << ")" 
			      << std::endl;
		    return 3;
		}
    }


    // ... <your code>

    int insertionPenalty, deletionPenalty, substPenalty;
    insertionPenalty = 1;
    deletionPenalty = 1;
    substPenalty = 1;
    
    std::ofstream toPrint;
    toPrint.open("text.txt", std::ofstream::out);
    toPrint.close();

    int wordCountSpoken = wordCount(spoken);

    int errorCount = 0;

    for (unsigned n=0; n<spoken.size(); n++) {
    	Alignment alignmentArray;
	    unsigned sizeRecog, sizeSpoken;
	    sizeRecog = recog[n].size();
	    sizeSpoken = spoken[n].size();

	    #ifdef CONSOLE_OUTPUT
	    for (Word& w : spoken[n]) {
	    	std::cout << w << " " ;
	    }
	    std::cout << std::endl;
	    for (Word& w : recog[n]) {
	    	std::cout << w << " " ;
	    }

	    std::cout << std::endl;
	    #endif
	    // Initializing 2-D vectors y-axis: spoken, x-axis: recognized, vector[recog][spoken]

	    std::vector<std::vector<int> > ldGrid(sizeRecog+1, std::vector<int>(sizeSpoken+1, -1));
	    std::vector<std::vector<std::tuple<int, int> > > bpGrid(sizeRecog+1, std::vector<std::tuple<int,int> >(sizeSpoken+1, std::make_tuple(-1,-1)) );
	    std::vector<std::vector<EditOperation> > opGrid(sizeRecog+1, std::vector<EditOperation>(sizeSpoken+1, NO));

	    // Filling the basic values
	    ldGrid[0][0] = 0;
	    for (int j=1; j<sizeSpoken+1; j++) {
	    	ldGrid[0][j] = j;
	    	opGrid[0][j] = DEL;
	    	bpGrid[0][j] = std::make_tuple<int,int>(0, j-1);
	    }

	    for (int i=1; i<sizeRecog+1; i++) {
	    	ldGrid[i][0] = i;
	    	opGrid[i][0] = INS;
	    	bpGrid[i][0] = std::make_tuple<int,int>(i-1, 0);
	    }

	    // print out initialized grid:

	    // Actual calculation of Levenshtein distance
	    int count = 0;
	    for (int s=1; s<sizeSpoken+1; s++) {
	    	for (int r=1; r<sizeRecog+1; r++) {
	    		int d = (int) isEqual(recog[n][r-1], spoken[n][s-1]);
	    		int subs = ldGrid[r-1][s-1];
	    		int insert = ldGrid[r-1][s];
	    		int del = ldGrid[r][s-1];

	    		if ((subs - d < insert) && (subs - d < del)) {
	    			if (d) opGrid[r][s] = NO;
	    			else opGrid[r][s] = SUB;
	    			bpGrid[r][s] = std::make_tuple(r-1, s-1);
	    			ldGrid[r][s] = substPenalty + subs - (int) d;
	    		}
	    		else if (insert < del) {
	    			opGrid[r][s] = INS;
	    			ldGrid[r][s] = insertionPenalty + insert;
	    			bpGrid[r][s] = std::make_tuple(r-1, s);
	    		}
	    		else {
	    			opGrid[r][s] = DEL;
	    			ldGrid[r][s] = deletionPenalty + del;
	    			bpGrid[r][s] = std::make_tuple(r, s-1);
	    		}
	    	}
	    }
	    

	    int x = bpGrid.size() - 1;
	    int y = bpGrid[0].size() - 1;

	    // Getting alignment through backpointer
	    while ((x > 0) || (y > 0)) {
	    	switch (opGrid[x][y]) {
	    		case NO:
	    		{
	    			AlignmentElement align(spoken[n][y-1], recog[n][x-1], NO);
	    			alignmentArray.insert(alignmentArray.begin(), align);
	    			#ifdef CONSOLE_OUTPUT
	    			std::cout << "  ";
	    			#endif
	    			break;
    			}
    			case SUB:
    			{
    				AlignmentElement align(spoken[n][y-1], recog[n][x-1], SUB);
    				alignmentArray.insert(alignmentArray.begin(), align);
    				#ifdef CONSOLE_OUTPUT
    				std::cout << "SUB ";
    				#endif
    				errorCount++;
	    			break;
	    		}
    			case DEL:
    			{
    				AlignmentElement align(spoken[n][y-1], "", DEL);
    				alignmentArray.insert(alignmentArray.begin(), align);
    				#ifdef CONSOLE_OUTPUT
    				std::cout << "DEL ";
    				#endif
    				errorCount++;
	    			break;
	    		}
    			case INS:
    			{
    				AlignmentElement align("", recog[n][x-1], INS);
					alignmentArray.insert(alignmentArray.begin(), align);
					#ifdef CONSOLE_OUTPUT	    
					std::cout << "INS ";
					#endif
					errorCount++;			
					break;
	    		}
	    		default:
	    			std::cerr << "Something went wrong here..." << std::endl;
	    	}

	    	#ifdef CONSOLE_OUTPUT
	    	std::cout << "---" << x << ", " << y  << ": " << alignmentArray[0].spoken << std::endl;
	    	#endif
	    	int x_temp = x;
	    	x = std::get<0>(bpGrid[x_temp][y]);
	    	y = std::get<1>(bpGrid[x_temp][y]);
	    }

	    #ifdef FILE_OUTPUT
	    if (n<10) {
		    toPrint.open("text.txt", std::ofstream::app);
		    writeAlignment(alignmentArray, toPrint);
		    toPrint.close();
		}	
	    #endif
	}
	std::cout << "With I/D/S penalities: " << insertionPenalty << "/" << deletionPenalty << "/" << substPenalty << std::endl;
	std::cout << "Word count: " << wordCountSpoken << std::endl;
	std::cout << "WER: " << (errorCount*1.0)/(wordCountSpoken*1.0) << std::endl;
	
	
	

    return 0;
}
