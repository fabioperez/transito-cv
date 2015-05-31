#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/cmd_line_parser.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
  try {
    command_line_parser parser;
    parser.parse(argc, argv);

    const std::string dir = argv[1];

    dlib::array<array2d<unsigned char> > images;

    images.resize(parser.number_of_arguments());
    for (unsigned long i = 0; i < images.size(); ++i) load_image(images[i], parser[i]);
    for (unsigned long j = 0; j < images.size(); ++j) pyramid_up(images[j]);
    //for (unsigned long j = 0; j < images.size(); ++j) pyramid_up(images[j]);
    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 

    // Load SVM detectors
    object_detector<image_scanner_type> detector1, detector2;
    deserialize("svm_detectors/pare_detector.svm") >> detector1;
    deserialize("svm_detectors/lombada_detector.svm") >> detector2;
    std::vector<object_detector<image_scanner_type> > detectors;
    detectors.push_back(detector1);
    detectors.push_back(detector2);

    image_window win;
    std::vector<rect_detection> rects;
    for (unsigned long i = 0; i < images.size(); ++i) {
      evaluate_detectors(detectors, images[i], rects);

      // Put the image and detections into the window.
      win.clear_overlay();
      win.set_image(images[i]);
      for (unsigned long j = 0; j < rects.size(); ++j) {
        win.add_overlay(rects[j].rect, rgb_pixel(255,rects[j].weight_index*255,0));
      }

    }

  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
