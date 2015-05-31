/* Visualise a fHOG detector. 
   This program takes a fHOG detector as input and displays it in a window.
*/

#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
  try {
    if (argc != 2)
    {
      cout << "Visualise a fHOG detector.";
      cout << "Usage: " << argv[0] << " detector.svm" << endl;
      return 1;
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
