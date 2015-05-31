This is a traffic sign detector, that uses [dlib](http://dlib.net/) and its implementation of the Felzenszwalb's version of the Histogram of Oriented (HoG) detector.

The training examples used in this repository are from Brazilian road signs, but the detector should work with any traffic signs, as long as you train it properly. Google Street View images can be used to train the detector. 25~40 images are sufficient to train a good detector.

## Build

```
mkdir build
cd build
cmake .. && cmake --build .
```

## Mark signs on images
1. Compile `imglab`:

```
cd imglab
mkdir build
cd build
cmake ..
cmake --build .
```

2. Create XML from sample images:

```
imglab/build/imglab -c images/pare/training.xml images/pare/train/*.(jpg|jpeg|png)
imglab/build/imglab images/pare/training.xml
imglab/build/imglab -c images/pare/testing.xml images/pare/test/*.(jpg|jpeg|png)
imglab/build/imglab images/pare/testing.xml
```

3. Use `shift+click` to draw a box around signs.

## Train the fHOG detector

To train a fHOG detector, run `build/hog_detector`. For example, to run the detector on the `image/pare/` folder in the verbose mode, upsampling each image once, with a threshold filter with value `0.4`, execute the following command: 

```
build/hog_detector -v -u1 --filter 0.4 images/pare/
```

Run `build/hog_detector -h` for more details.
