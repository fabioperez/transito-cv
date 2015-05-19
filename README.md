# Mark signs on images
1. Compile `imglab`:

```
cd imglab
mkdir build
cd build
cmake ..
cmake --build
```

2. Create XML from sample images:

```
imglab/build/imglab -c training.xml images/pare/train/*.jpg
imglab/build/imglab train.xml
imglab/build/imglab -c testing.xml images/pare/test/*.jpg
imglab/build/imglab test.xml
```

3. Use `shift+click` to draw a box around signs.

