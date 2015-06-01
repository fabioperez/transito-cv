/* HOG DETECTOR TRAINER
 * This program trains a fHOG detector.
 * For help, run ./hog_detector -h
 *
 * Sample usage:
 *   ./hog_detector -u1 --filter 0.4 -v images/pare
 *
 * To better understand the code of this detector, read the following example codes:
 *   http://dlib.net/fhog_object_detector_ex.cpp.html
 *   http://dlib.net/train_object_detector.cpp.html
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
    parser.add_option("c","Set the SVM C parameter to <arg> (default: 1.0).",1);
    parser.add_option("u", "Upsample each input image <arg> times. Each upsampling quadruples the number of pixels in the image (default: 0).", 1);
    parser.add_option("v","Be verbose.");
    parser.add_option("filter","Remove filters with singular value less than <arg> (default: disabled).", 1);
    parser.add_option("detector-name","Save SVM detector to <arg> (default: 'detector.svm').", 1);
    parser.add_option("threads", "Use <arg> threads for training (default: 4).",1);
    parser.add_option("eps", "Set SVM training epsilon to <arg> (default: 0.01).", 1);
    parser.add_option("norm", "If set, the nuclear norm regularization strength will be <arg> (default: disabled).", 1);

    // TODO: Variable window size
#if 0
    parser.add_option("w","Set window size to <arg1> x <arg2> pixels (default: 80x80.", 2);
#endif

    parser.parse(argc, argv);

    // Can't give an option more than once
    const char* one_time_opts[] = {"h","c","u","v","detector-name", "threads", "eps","filter","norm"};
    parser.check_one_time_options(one_time_opts);

    // Check parameters values
    parser.check_option_arg_range("c", 1e-12, 1e12);
    parser.check_option_arg_range("u", 0, 8);
    parser.check_option_arg_range("threads", 1, 1000);
    parser.check_option_arg_range("eps", 1e-5, 1e4);
    parser.check_option_arg_range("filter", 0.0, 2.0);
    parser.check_option_arg_range("norm", 1e-12, 1e12);

    // Display help message
    if (parser.option("h")) {
      cout << "Usage: " << argv[0] << " [options] <path>" << endl;
      cout << "<path> must countain the files training.xml and testing.xml." << endl;
      parser.print_options(); 

      return EXIT_SUCCESS;
    }

    if (parser.number_of_arguments() == 0) {
      cout << "You must give an image or an image dataset metadata XML file produced by the imglab tool." << endl;
      cout << "\nTry the -h option for more information." << endl;
      return EXIT_FAILURE;
    }

    // Declarations and parameters
    const std::string dir = parser[0];
    dlib::array<array2d<unsigned char> > images_train, images_test;
    std::vector<std::vector<rectangle> > sign_boxes_train, sign_boxes_test;
    const double c_val = get_option(parser, "c", 1.0);
    const unsigned long upsample_amount = get_option(parser, "u", 0);
    const std::string detector_name = get_option(parser, "detector-name", "detector.svm");
    const int num_threads = get_option(parser, "threads", 4);
    const double eps = get_option(parser, "eps", 0.01);
    const double filter_val = get_option(parser, "filter", 0.0);
    const double norm = get_option(parser, "norm", 0.0);

    cout << "Training with the following parameters: " << endl;
    cout << "  threads:                   "<< num_threads << endl;
    cout << "  C:                         "<< c_val << endl;
    cout << "  epsilon:                   "<< eps << endl;
    cout << "  upsample this many times : "<< upsample_amount << endl;
    cout << "  filter threshold :         "<< filter_val << endl;
    cout << "  NNR strenght :             "<< norm << endl;

    // Load training and testing datasets
    load_image_dataset(images_train, sign_boxes_train, dir+"/training.xml");
    load_image_dataset(images_test, sign_boxes_test, dir+"/testing.xml");

    // Upsample images (set by -u parameters)
    for (unsigned long i = 0; i < upsample_amount; ++i) {
      upsample_image_dataset<pyramid_down<2> >(images_train, sign_boxes_train);
      upsample_image_dataset<pyramid_down<2> >(images_test,  sign_boxes_test);
    }

    // Create fHOG scanner
    typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type; 
    image_scanner_type scanner;
    if (norm > 0.0) scanner.set_nuclear_norm_regularization_strength(norm);
    scanner.set_detection_window_size(80, 80); 
    structural_object_detection_trainer<image_scanner_type> trainer(scanner);
    trainer.set_num_threads(num_threads); // Number of working threads
    trainer.set_c(c_val); // SVM C-value
    if (parser.option("v")) trainer.be_verbose();
    trainer.set_epsilon(eps);
    object_detector<image_scanner_type> detector = trainer.train(images_train, sign_boxes_train);

    if (filter_val > 0.0) {
      int num_filters_before = num_separable_filters(detector);
      detector = threshold_filter_singular_values(detector,filter_val);
      cout << num_filters_before-num_separable_filters(detector) << " filters were removed." << endl;
    }

    // Test results on training and testing dataset
    cout << "training results: " << test_object_detection_function(detector, images_train, sign_boxes_train) << endl;
    cout << "testing results:  " << test_object_detection_function(detector, images_test, sign_boxes_test) << endl;

    // Save detector to disk
    serialize(detector_name) << detector;
  }
  catch (exception& e) {
    cout << "\nexception thrown!" << endl;
    cout << e.what() << endl;
  }
}
