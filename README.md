# Mark signs on images
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

