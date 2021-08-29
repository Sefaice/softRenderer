softRenderer

* 2D line rasterization
  * Bresenham algorithm
  * Wu's line algorithm
* 2D triangle rasterization
  * line equation method
  * barycentric method(optimized)
* 3D triangle rasterization
  * model, view, projection transform
  * clip & tessellation
  * perspective division
  * viewport transform
  * back-face culling
  * z buffer
  * top-left rule
  * polygon mode
  * vertex z interpolation & other attributes interpolation
* lighting
  * programmable shader
  * texture mapping
  * (Blinn-)Phong lighting model
  * normal mapping
  * skybox
  * shadow mapping
* SSAA
* model loading
* Catmull-Clark subdivision with half-edge
* multi-thread parallel optimization

model loading and lighting model:

![model-backpack](./imgs/backpack.PNG)

![model-gun](./imgs/gun.PNG)

shadow mapping with PCF:

![shadow mapping](./imgs/shadow.PNG)

skybox and environment mapping:

![skybox and environment mapping](./imgs/skybox.PNG)

Catmull-Clark subdivision with half-edge:

![Catmull-Clark subdivision](./imgs/subdivision.gif)

