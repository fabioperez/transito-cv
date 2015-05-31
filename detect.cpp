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
    //if (argc != 2) {
    //cout << "Give the path to the examples directory as the argument to this program" << endl;
    //cout << endl;
    //return 0;
    //}
    command_line_parser parser;
    parser.parse(argc, argv);

    const std::string dir = argv[1];

    dlib::array<array2d<unsigned char> > images;

    images.resize(parser.number_of_arguments());
    for (unsigned long i = 0; i < images.size(); ++i) load_image(images[i], parser[i]);
    for (unsigned long j = 0; j < images.size(); ++j) pyramid_up(images[j]);
    //upsample_image_dataset<pyramid_down<2> >(images, sign_boxes_train);
    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 

    // Load SVM detectors
    object_detector<image_scanner_type> detector1, detector2;
    deserialize("svm_detectors/pare_detector.svm") >> detector1;
    deserialize("svm_detectors/lombada_detector.svm") >> detector2;
    std::vector<object_detector<image_scanner_type> > detectors;
    detectors.push_back(detector1);
    detectors.push_back(detector2);
    //std::vector<rectangle> dets = evaluate_detectors(detectors, images_train[0]); 
    //

    image_window win;
    std::vector<rect_detection> rects;
    for (unsigned long i = 0; i < images.size(); ++i) {
      evaluate_detectors(detectors, images[i], rects);
      printf("%lu\n",rects[0].weight_index);
      printf("%lu\n",rects[1].weight_index);
      cout << "Number of detections: "<< rects.size() << endl;

      // Put the image and detections into the window.
      win.clear_overlay();
      win.set_image(images[i]);
      //win.add_overlay(rects.rect, rgb_pixel(255,0,0));

      cout << "Hit enter to see the next image.";
      cin.get();
    }

    //image_window win;
    //for (unsigned long i = 0; i < images_test.size(); ++i) {
    //std::vector<rectangle> dets = evaluate_detectors(detectors, images_train[0]); 
    //win.clear_overlay();
    //win.set_image(images_test[i]);
    //win.add_overlay(dets, rgb_pixel(255,0,0));
    //cout << "Hit enter to process the next image..." << endl;
    //cin.get();
    //}

  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
