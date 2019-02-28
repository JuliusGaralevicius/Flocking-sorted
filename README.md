# Flocking sorted
https://www.youtube.com/watch?v=3bm8I9wjthQ 
Agents sorted and new positions calculated using OpenGL compute shader.   
Currently configured for 16k particles in a space of 512x512 units.   
Firstly prefix sum of cell ids of boids is calulcated, then the boids are put in another sorted array in parallel.   
Then for each boid, create an offset table which stores an index of the first element in the sorted boids array for each neighbourhood cell. Runs at 300+ fps for 16k particles, 30 fps for 130k particles on gtx 1050.
