/* Visualise a fHOG detector. 
*  This program takes a fHOG detector as input and displays it in a window.
*
*  Usage:
*    ./view_hog detector.svm
*/

#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/cmd_line_parser.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
  try {
    command_line_parser parser;
    parser.add_option("h","Display this help message.");

    parser.parse(argc, argv);
    const char* one_time_opts[] = {"h"};
    parser.check_one_time_options(one_time_opts);

    // Display help message
    if (parser.option("h")) {
      cout << "Usage: " << argv[0] << " [options] <fHOG SVM file>" << endl;
      parser.print_options(); 

      return EXIT_SUCCESS;
    }

    if (parser.number_of_arguments() == 0) {
      cout << "You must give a fHOG SVM file as input." << endl;
      cout << "\nTry the -h option for more information." << endl;
      return EXIT_FAILURE;
    }

    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
    object_detector<image_scanner_type> detector;
    deserialize(argv[1]) >> detector;
    image_window hogwin(draw_fhog(detector), "Learned fHOG detector");
    cout << "Press any key to exit!" << endl;
    cin.get(); // Wait input to exit
  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
