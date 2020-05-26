It is highly recommended to compile and run this project on Windows operating system. And the specified compilation platform is Visual Studio 2015.

This project's cmake structure is from [libigl/libigl-example-project](https://github.com/libigl/libigl-example-project), which is a blank project example showing how to use libigl and cmake. 

## See the tutorial first

Then build, run and understand the [libigl
tutorial](http://libigl.github.io/libigl/tutorial/).

## libigl download
```
git clone https://github.com/libigl/libigl
cd libigl
git checkout db6c89aa0acb6c9bd772951f2275c19bc2267054
```

## Build libigl

    mkdir build
    cd build
    cmake ..
    make
    
Compile this project using the standard cmake routine:
On Windows, use Cmake-gui to generate a solution for VS2015, and compile it.

## Compile
After downloading and installing libigl, you need to add the Path of libigl to this project by modifying `FindLIBIGL.cmake`
```
find_path(LIBIGL_INCLUDE_DIR igl/readOBJ.h
    HINTS
        ENV LIBIGL
        ENV LIBIGLROOT
        ENV LIBIGL_ROOT
        ENV LIBIGL_DIR
    PATHS
        ${CMAKE_SOURCE_DIR}/../..
        ${CMAKE_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/libigl
        ${CMAKE_SOURCE_DIR}/../libigl
        ${CMAKE_SOURCE_DIR}/../../libigl
        D:/Program\ Files/libigl # you can add the path like this
        /usr
        /usr/local
        /usr/local/igl/libigl
    PATH_SUFFIXES include
)

```
Compile this project using the standard cmake routine.
On Windows, use Cmake-gui to generate a solution for VS2015. It will be appeared a file (Tspline.sln) in the build directory. Compile and run it with VS2015.


## examples and results
1. Cylindrical surface
![cylinder.PNG](https://github.com/aijm/NURBS/blob/master/examples/cylinder.PNG)
2. Torus surface
![torus.PNG](https://github.com/aijm/NURBS/blob/master/examples/torus.PNG)
3. Skinned surface and solid
![representative_image.png](https://github.com/Hubury/TSpline/blob/master/out/representative_image.png)


