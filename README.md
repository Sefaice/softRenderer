softRenderer

* 2D line rasterization
  * Bresenham Algorithm
  * wu's line algorithm
* 2D triangle rasterization
  * line equation method
  * Barycentric method(optimized)
* 3D triangle rasterization
  * model, view, projection transform
  * clip & tessellation
  * perspective division
  * viewport transform
  * z buffer
  * top-left rule
  * polygon mode
  * vertex z interpolation & other attributes interpolation
  * naive texture mapping
  * (Blinn-)Phong lighting model
* SSAA
* catmull-clark subdivision

TODOS:
* back face culling(改顶点顺序为counter-clockwise?（会影响到2d raster）)
* MSAA
* obj loader implementation
* 优化方法：Rendering Blocks of Pixels；提升帧率
