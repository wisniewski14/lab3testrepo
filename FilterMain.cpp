#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rtdsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{
	
	
  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  }
}



double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

	long long cycStart, cycStop;

	cycStart = rdtscll();
	int d = filter -> getDivisor();
	int w = input -> width;
	int h = input -> height;
	output -> width = w;
	output -> height = h;
	w--;
	h--;
	
	int filt[3][3];
	for(int i =0;i<3;i++){
		for(int j =0;j<3;j++){
			filt[i][j]= filter -> get(i, j);
		}
	}
	
	for(int plane = 0; plane < 3; plane++) {
		for(int row = 1; row < h ; row++) {
			for(int col = 1; col < w ; col++) {
				
				int v1=input -> color[plane][row-1][col-1]*filt[0][0];
				   v1+=input -> color[plane][row-1][col  ]*filt[0][1];
				   v1+=input -> color[plane][row-1][col+1]*filt[0][2];
				
				int v2=input -> color[plane][row  ][col-1]*filt[1][0];
				   v2+=input -> color[plane][row  ][col  ]*filt[1][1];
				   v2+=input -> color[plane][row  ][col+1]*filt[1][2];
				
				int v3=input -> color[plane][row+1][col-1]*filt[2][0];
				   v3+=input -> color[plane][row+1][col  ]*filt[2][1];
				   v3+=input -> color[plane][row+1][col+1]*filt[2][2];
				
				v1+=v2+v3;
				v1=(d==1) ? v1 : v1/d;
				output -> color[plane][row][col] = (v1 > 255) ? 255 : (v1 < 0) ? 0 : v1;
				//output -> color[plane][row][col] = (~(v1>>31)) & ((!(v1>>8)-1) | (v1&255));
				
			}
		}
	}
	
	cycStop = rdtscll();
	double diff = cycStop - cycStart;
	double diffPerPixel = diff / (output -> width * output -> height);
	fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
	return diffPerPixel;
}




