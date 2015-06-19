/* HOG DETECTOR
 *
 */

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

    parser.add_option("h","Display this help message.");
    parser.add_option("u", "Upsample each input image <arg> times. Each upsampling quadruples the number of pixels in the image (default: 0).", 1);
    parser.add_option("wait","Wait user input to show next image.");
    parser.add_option("t","Uses a correlation tracker to improve performance at cost of precision. Signs locations are reevaluated every <arg> frames.", 1);

    parser.parse(argc, argv);
    parser.check_option_arg_range("u", 0, 8);
    parser.check_option_arg_range("t", 0, 999);

    const char* one_time_opts[] = {"h","u","wait", "t"};
    parser.check_one_time_options(one_time_opts);

    // Display help message
    if (parser.option("h")) {
      cout << "Usage: " << argv[0] << " [options] <list of images>" << endl;
      parser.print_options(); 

      return EXIT_SUCCESS;
    }

    if (parser.number_of_arguments() == 0) {
      cout << "You must give a list of input files." << endl;
      cout << "\nTry the -h option for more information." << endl;
      return EXIT_FAILURE;
    }

    const unsigned long upsample_amount = get_option(parser, "u", 0);

    dlib::array<array2d<unsigned char> > images;

    images.resize(parser.number_of_arguments());
    for (unsigned long i = 0; i < images.size(); ++i) load_image(images[i], parser[i]);
    for (unsigned long i = 0; i < upsample_amount; ++i) {
      for (unsigned long j = 0; j < images.size(); ++j) {
        pyramid_up(images[j]);
      }
    }
    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 

    // Load SVM detectors
    object_detector<image_scanner_type> detector1, detector2, detector3;
    deserialize("svm_detectors/pare_detector.svm") >> detector1;
    deserialize("svm_detectors/lombada_detector.svm") >> detector2;
    deserialize("svm_detectors/pedestre_detector.svm") >> detector3;
    std::vector<object_detector<image_scanner_type> > detectors;
    detectors.push_back(detector1);
    detectors.push_back(detector2);
    detectors.push_back(detector3);


    image_window win;
    std::vector<rect_detection> rects;
    const int MAX_ITERATIONS = get_option(parser, "t", 0); 
    if (MAX_ITERATIONS == 0) {
      for (unsigned long i = 0; i < images.size(); ++i) {
        evaluate_detectors(detectors, images[i], rects);

        // Put the image and detections into the window.
        win.clear_overlay();
        win.set_image(images[i]);
        std::string text_desc;

        for (unsigned long j = 0; j < rects.size(); ++j) {
          switch (rects[j].weight_index) {
            case 0: 
              text_desc = "PARE";
              break;
            case 1: 
              text_desc = "LOMBADA";
              break;
            case 2: 
              text_desc = "PEDESTRE";
              break;
            default: 
              break;
          }
          win.add_overlay(rects[j].rect, rgb_pixel(255,rects[j].weight_index*122,0),text_desc);
        }

        if (parser.option("wait")) {
          cout << "Press any key to continue...";
          cin.get();
        }
      }
    }
    else {
      const int MAX_TRACKERS = 100;
      correlation_tracker tracker[MAX_TRACKERS];
      int tracker_label[MAX_TRACKERS];
      std::string tracker_text[MAX_TRACKERS];


      for (unsigned long i = 0; i < images.size();) { 
        evaluate_detectors(detectors, images[i], rects);      
        for (unsigned long j = 0; j < rects.size() && j < MAX_TRACKERS; ++j) {
          tracker[j].start_track(images[i], rects[j].rect);
          tracker_label[j] = rects[j].weight_index;
        }
        
        for (int k = 0; k < MAX_ITERATIONS && i < images.size(); ++k) {
          for (int j = 0; j < rects.size() && j < MAX_TRACKERS; ++j) {      
            // Update tracker
            tracker[j].update(images[i]);

            // Get sign name
            switch (tracker_label[j]) {
              case 0: 
                tracker_text[j] = "PARE";
                break;
              case 1: 
                tracker_text[j] = "LOMBADA";
                break;
              case 2: 
                tracker_text[j] = "PEDESTRE";
                break;
              default: 
                break;
            }
          }
          
          win.clear_overlay();
          win.set_image(images[i]);

          for (int j = 0; j < rects.size() && j < MAX_TRACKERS; ++j) {
            win.add_overlay(tracker[j].get_position(), rgb_pixel(255,rects[j].weight_index*122,0), tracker_text[j]);
          }

          // Wait for user input
          if (parser.option("wait")) {
            cout << "Press any key to continue...";
            cin.get();
          }

          i++;
        }
      }
    }
  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
