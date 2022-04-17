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

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -m FLOAT, --umaxnorm=FLOAT  umbral de la autocorrelación a largo plazo [default: 0.5]
    -p FLOAT, --llindarPos=FLOAT  umbral positivo central clipping [default: 0.01]
    -n FLOAT, --llindarNeg=FLOAT  umbral negativo central clipping [default: -0.01]
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
  float llindarPos=stof(args["--llindarPos"].asString());
  float llindarNeg=stof(args["--llindarNeg"].asString());
  // Read input sound file
  unsigned int rate;
  /*unsigned t0, t1;
 
  t0=clock();*/
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
  vector<float> f0_;
  int fMedianaLen=3;
  f0_.push_back(f0[0]);
  for (unsigned int l=1; l<f0.size()-1; l++){

    for(int r=-1; r<2; r++){
      fMediana.push_back(f0[l+r]);
    }
    sort(fMediana.begin(),fMediana.end());
    f0_.push_back(fMediana[1]);//generalitzar l'1
    fMediana.clear();
  }
   f0_.push_back(f0[f0.size()-1]);

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0_.begin(); iX != f0_.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur
  /*t1 = clock();

  double time = (double(t1-t0)/CLOCKS_PER_SEC);

  os<<"Execution Time: " << time << endl;*/

  return 0;
}
