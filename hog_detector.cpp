#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
  try {
    if (argc != 3) {
      cout << "Give the path to the examples directory as the argument to this program" << endl;
      cout << endl;
      return 0;
    }

    const std::string dir = argv[1];
    const double c_val = atof(argv[2]);
    cout << (c_val);
    dlib::array<array2d<unsigned char> > images_train, images_test;
    std::vector<std::vector<rectangle> > sign_boxes_train, sign_boxes_test;

    load_image_dataset(images_train, sign_boxes_train, dir+"/training.xml");
    load_image_dataset(images_test, sign_boxes_test, dir+"/testing.xml");
    upsample_image_dataset<pyramid_down<2> >(images_train, sign_boxes_train);
    upsample_image_dataset<pyramid_down<2> >(images_test,  sign_boxes_test);
    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
    image_scanner_type scanner;
    scanner.set_detection_window_size(60, 60); 
    structural_object_detection_trainer<image_scanner_type> trainer(scanner);
    trainer.set_num_threads(4);  
    trainer.set_c(c_val);
    trainer.be_verbose();
    trainer.set_epsilon(0.01);
    object_detector<image_scanner_type> detector = trainer.train(images_train, sign_boxes_train);
    cout << "training results: " << test_object_detection_function(detector, images_train, sign_boxes_train) << endl;
    cout << "testing results:  " << test_object_detection_function(detector, images_test, sign_boxes_test) << endl;
            detector = threshold_filter_singular_values(detector,0.4);

    image_window hogwin(draw_fhog(detector), "Learned fHOG detector");
    cin.get();

    image_window win;
    for (unsigned long i = 0; i < images_test.size(); ++i)
    {
    // Run the detector and get the face detections.
    std::vector<rectangle> dets = detector(images_test[i]);
    win.clear_overlay();
    win.set_image(images_test[i]);
    win.add_overlay(dets, rgb_pixel(255,0,0));
    cout << "Hit enter to process the next image..." << endl;
    cin.get();
    }

  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
