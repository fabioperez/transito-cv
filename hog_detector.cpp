#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>

#include <iostream>
#include <fstream>

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
  try {
    if (argc != 2) {
      cout << "Give the path to the examples directory as the argument to this program" << endl;
      cout << endl;
      return 0;
    }
    const std::string training_directory = argv[1];

  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
