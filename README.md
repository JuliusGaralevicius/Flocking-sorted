# Flocking sorted
https://www.youtube.com/watch?v=yF7cT7l4HTA   
Agents sorted and new positions calculated using OpenGL compute shader.   
Currently configured for 16k particles in a space of 512x512 units.   
Firstly the prefix sum of cell ids of boids is calulcated, then the boids are put in another sorted array in parallel.   
Then when doing a nearest neighbour search, no need to check all other boids, only those in neighbourhood cells. Currently a range of 1000-2000 boids is used, need to be improved to check for specific cells based on current cell.
