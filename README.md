## Description
A personal implementation of the famous Breakout arcade game with C++ and an  
OpenGL-based API from my University. The source code I wrote is found in  
[this directory](./src/lab_m1/Tema1/).

## How to Run the Game  
- Check the [API's Readme](API_README.md) for a complete tutorial or try the commands below
in a Linux terminal:  

    - `mkdir build && cd build`
    - `cmake ..`  
    - `make`  
    - `./bin/Debug/GFXFramework`: a new window should open.  
&nbsp;  
    <img src=image.png width="40%" />

## Gameplay Tutorial
- Build your Breakout platform by dragging items from the left side.  
- You are allowed to use a maximum of 10 blocks, following two building  
constraints: all blocks must have at least one directly connected neighbour  
(this is verified by a bfs traversal) and the top row of your platform should  
be continuous and the longest one. Example:


    <img src=image-1.png width="40%" />

- Start the game by pressing the green button in the top-right corner.  
- Press space to release the ball. Use the left and right arrow keys to  
move.


    <img src=image-2.png width="40%" />

- Enjoy!  
