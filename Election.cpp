// Kevin Pietrow
// Election.cpp

#include "Election(2).hpp" 
int main (int argc, char** argv) 
{      
	unsigned n = std::atoi(argv[1]); // or hardcode to whatever number of voters you want
	Election e {
		n, 
		{0.39, 0.33, 0.28},
		{0.12, 0.16, 0.98},           
		{0.57, 0.62, 0.52},           
		{ 
			{0.93, 0.05, 0.02}, 
			{0.05, 0.94, 0.01}, 
			{0.44,0.46,0.10} 
		}      
	};       
	e.simulate();       
	return 0; 
}

