/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
//#include <matplotlib.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;
//namespace plt = matplotlibcpp;


static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -m FLOAT, --umaxnorm=FLOAT  umbral de la autocorrelación a largo plazo [default: 0.5]
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  float umaxnorm=stof(args["--umaxnorm"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate,umaxnorm, PitchAnalyzer::HAMMING, 50, 500);

  ///Normalitzar el senyal
  float max=0;
  for(unsigned int k=0; k<x.size();k++){
    if(abs(x[k])>max){
      max=x[k];
    }
  }
  for(unsigned int k=0; k<x.size();k++){
    x[k]=x[k]/max;
  }

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  float llindarPos=0.01;//BUSCAR VALOR OPTIM
  float llindarNeg=-0.01;//BUSCAR VALOR OPTIM
  for(unsigned int k=0; k<x.size();k++){
    if(x[k]>0){
      x[k]=x[k]-llindarPos;
      if(x[k]<0){
        x[k]=0;
      }
    } else {
      x[k]=x[k]-llindarNeg;
      if(x[k]>0){
        x[k]=0;
      }
    }
  }
  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  vector<float> fMediana;
  unsigned int fMedianaLen=3;
  /*for (unsigned int l=0; l<f0.size()-(fMedianaLen-1); l++){
    for(int r=0; r<fMedianaLen; r++){
      fMediana[r]=f0[l+r];
    }
    sort(fMediana.begin(),fMediana.end());
    f0[l]=fMediana[fMediana.size()/2];
  }*/
    for(unsigned int l = 0; l<f0.size();l++){
      if(l<(f0.size()-fMedianaLen)){
        for(unsigned int r = 0; r<fMedianaLen; r++){
          fMediana[r]  = f0[l+r];
        }
      }
      else{
        fMediana[1] = f0[l];
      }
      sort(fMediana.begin(), fMediana.end());
      f0[l] = fMediana[1];

    }
  /*unsigned int l = 0;
  while(l<f0.size()){
    if(l<=(f0.size()-fMedianaLen)){
      for(unsigned int r= 0; r<fMedianaLen; r++){
          fMediana[r] = f0[l+r];
      }
    }
    else{
      for(unsigned int r = 0; r<fMedianaLen; r++){
        fMediana[r] = f0[l];
      }
    }
    sort(fMediana.begin(), fMediana.end());
    f0[l] = fMediana[1];
    l++;





  }*/

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  /*int n = 500;
	std::vector<double> x(n), y(n), z(n);
	for(int i=0; i<n; ++i) {
		x.at(i) = i;
		y.at(i) = sin(2*M_PI*i/360.0);
		z.at(i) = 100.0 / i;
	}

  plt::suptitle("My plot");
    plt::subplot(1, 2, 1);
	plt::plot(x, y, "r-");
    plt::subplot(1, 2, 2);
    plt::plot(x, z, "k-");
    // Add some text to the plot
    plt::text(100, 90, "Hello!");


	// Show plots
	plt::show();*/

  return 0;
}
