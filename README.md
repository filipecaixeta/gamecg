# Sungkyunkwan University

## Computer Graphics Final Project

### Racing Game


Filipe Alves Caixeta

Yuri Goncalves Rocha

Suwon - South Korea, June 7th 2015


### Introduction

This report is written for the final project in computer graphics course SWE3008. The project is about implementing a complete graphic system in 3D, and put together all the techniques learned in the previous assignments.

For this project, it was built a complete 3D racing game. In the report we will go through how we created the game, which features we chose to implement and how we did it. Also our thoughts will be explained through the project. Lastly we will briefly conclude the most important things about a computer game: How fun is it to play, and what we can do to improve the user experience.

### Model loading

For loading the model we decided to use Open Asset Import Library (Assimp). The reason for the choice is because Assimp is an open source library and it can import most of the popular 3d data formats. Besides importing the data also we used some processing functionalities of the library such as compute smooth normals and load material properties. For loading the texture data stb\_image library was used. It is a very limited library for loading images butit is portable and easy to use, for this reason it was chosen.

A file may contain multiple meshes and materials so, for every file, there is an object of type Model, which holds all the meshes, textures and materials. A mesh contains basically three data structures. Vertex with the positions, normals, texture coordinates and other information. Material with the information like material color, transparence, material name and other data. Texture that contains the id of the texture, type and name. These three data structures are inside a structure plus some other information like the ids of the buffers on GPU.

Besides the data structure Model there is also other structures for car and scenario. The class Car contains a model and information about what are the meshes for the wheel, glass, body of the car, information about location and rotation of the wheel are also there. For the scenario there is nothing extended from the Model but it could have data about lights, skybox, and other things related to the scene.

### Transparency

Transparency was a very important feature that allowed us to have trees with texture and glass material for the car. In order to make fast rendering trees the scenario contains trees that are actually two planes crossing each other with a texture. The texture is an image of a tree with alpha channel. The disadvantage of the technique is that we cannot use the same mesh that we use to render for collision.

With the default configuration of OpenGL even though we have alpha channel we would see the pixels getting rendered. In order to fix that we check the alpha value inside the fragment shader and discard the fragments with alpha below a certain threshold. It works fine but we have a problem with the glass material. In this case the glass would be discarded. In order to solve the problem a glass is a mesh without texture, so for all the materials without texture if they have an alpha value different from 0 they will not be discarded. To make the glass material the approach was to render first all the non-glass materials then we activate the OpenGL blend function to compute the glass material and control the transparency depending on the glass transparency.

| ![Figure1.1](https://github.com/filipecaixeta/gamecg/raw/master/report/1.1.jpg)  | ![Figure1.2](https://github.com/filipecaixeta/gamecg/raw/master/report/1.2.jpg)|
|:---:|:---:|
| Figure 1.1: No alpha channel | Figure 1.2: rendered with alpha channel |

Figure 1: Alpha channel. Figure 1.1 shows the planes that are used to create the tree and the glass material without transparency. On Figure 1.2 we can see that the green plane disappeared and the glass has some transparency. 

| ![Figure2.1](https://github.com/filipecaixeta/gamecg/raw/master/report/2.1.jpg)  | ![Figure2.2](https://github.com/filipecaixeta/gamecg/raw/master/report/2.2.jpg)|
|:---:|:---:|
| Figure 2.1: With blending function | Figure 2.2: With alpha threshold |
Figure 2: Blending function. We can see the difference when using alpha threshold that will discard the fragment and blending function that will mix the colors depending of the alpha.

### Fog

Fog is a very easy feature to implement that we can get very good results as it can be seen on Figure 3. Inside the vertex shader there are calculations to get the absolute distance from the camera to a fragment. Depending on the fragment's distance the fog is applied with more or less intensity. We can also change the color and intensity of the fog to make it looks like a dust storm, haze or just to give the appearance that the things are very far from the camera. 

| ![Figure3.1](https://github.com/filipecaixeta/gamecg/raw/master/report/3.1.jpg)  | ![Figure3.2](https://github.com/filipecaixeta/gamecg/raw/master/report/3.2.jpg)| ![Figure3.3](https://github.com/filipecaixeta/gamecg/raw/master/report/3.3.jpg)|
|:---:|:---:|:---:|

Figure 3: Fog. From the left side we can see an image without fog. The other two images we can see different levels of fog.

### Wheel

In order to have the wheel spinning we first need to find them in the 3D model. For that we renamed the meshes for the wheel in a 3D editor and they are selected in the game by using the mesh name. Because the car is aligned with x, y and z axis, and the wheel is symmetric in two dimensions we can take the middle point between the maximum and minimum in each coordinate to know the pivot for rotation. This point is used to make the wheels rotate when the car has speed. Also we implemented steering for the front wheels with the same idea but the pivot for rotation is dislocated to be outside the wheel like in real cars. To make it look more real the steering wheel also rotates but faster than the wheels and as in real life when the directional keys are released the steering wheel goes back faster to rotation 0º.

| ![Figure4.1](https://github.com/filipecaixeta/gamecg/raw/master/report/4.1.jpg)  | ![Figure4.2](https://github.com/filipecaixeta/gamecg/raw/master/report/4.2.jpg)|
|:---:|:---:|
Figure 4: Steering. Steering applied when the car is on a curve.

### Sound

Sound is an integral part to any game. When playing a game, the human senses are starving for feedbacks. There are some tactile feedback on the controllers and the vision is used, however only for a limited part of its viewing area. So to add more to the game we need sounds to help the player feel more the action and change the mood of the scenes.

The SDL library was used to add sound effects for the car plus musics to the radio. The SDL_mixer library is a sample multi-channel audio mixer library. It supports any number of simultaneously playing channels of 16 bit stereo audio, plus a single channel of music. It was chosen due to its portability and ease of use. Even though it is a simple sound library, it was enough for the application needs regarding sounds.

The musics are played in an infinite loop. However, it can be paused anytime by pressing the key ‘p’ and also the radio station can be changed by pressing the key ‘m’. Another feature is the volume control, which rise the music volume when using the cockpit camera, and lower it when using the outside camera. This gives a more realistic sound environment. 

The sound effects are played when certain effects happen. For example, when the car brakes and the deacceleration is bigger than a certain threshold, the braking sound is played. If the car is parked and an acceleration is played, the startup acceleration sound is played.
    
### Physics 
    
After implementing the graphics and the sound, there is still one important point missing, the game physics. The physics engine is responsible to apply internal and external forces on objects, iterate through them and return the world transformation to all objects. In this application, a real time rigid body physics engine was needed to simulate collisions and rigid body dynamics. The chosen library was the Bullet Physics Library, which is an open source library used in a vast amount of games.

The first thing added to the simulation was the rigid fixed bodies, in other words, the world objects like tracks, walls, buildings, etc. To add this object, one used the 3D mesh vertex array, which was already loaded, and generated the rigid body using a built in function.

After adding the fixed objects, it was necessary to implement a moving vehicle, which would receive external forces, like engine force and braking force, and collide with the fixed objects. For this purpose a ray cast vehicle was implemented. The ray cast vehicle consists on casting a ray for each wheel. Using the ray’s intersection point, one can calculate the suspension length, and hence the suspension force. This force is then applied to the chassis, which is represented as a moving rigid rectangular body. Applying the force to the chassis will keep it from hitting the ground. The suspension is simulated using a spring force plus a damping force. The damping force is responsible to stop the car from bouncing forever. Then, the friction force is calculated for each wheel where the ray contacts the ground. In Bullet, the friction model is implemented as separate impulses applied to each wheel and then add a constraint to balance the friction on each wheel, which prevents oscillation caused by multiple wheels adding additional velocity.

Now that all the rigid bodies are set, one just need to apply engine and braking forces and steering to the front wheels. All of these constraints are already implemented on the Bullet built in class btRayCastVehicle. 

### Camera

We implemented several camera viewpoints around the car, so the user can chose what angle there suit him most to control the car. Which means the user have more options to interact with the game. Camera is very essential for the user to control the car! To change viewpoint simply press V. These are the viewpoints we chose:

##### Camera following car from behind

Here the camera follows the car from behind, showing the full car at all time. This makes it easier to get a bigger view on the surroundings.

Basically we have the camera angle to follow a coordinate on the car, so when the coordinate of the car is moving, the camera does the same. We adjusted the distance to the cars coordinate point to get the view we found suiting.

To give a more immersive environment, the car positions are added on a queue, so the camera can have a delayed position. This will create an effect of the camera “getting behind” when the car is accelerating, and “getting closer” when the car brakes.

#####  	First person camera inside car

This viewpoint is from inside the car. Here you get more realism because it is like driving a car in real life. While driving the car, the steering wheel moves along, and the user experience is more intense. The camera is set on a coordinate point inside of the car, with very low distance. Inside the car you also have the possibility to look around with the mouse. Click the mouse button and move the mouse to change the camera angle. On release the camera moves back to standard view. To perfect this view, a rear mirror could be implemented, so you can watch the surroundings behind you while you driving. 

#####   Rotating

The user also has the possibility to rotate the camera around the car. With a center point in the car, the camera moves in a circle around that point. When rotating, the user can choose to stop the camera at any point and lock the camera to that specific position. With that the user can chose whatever angle suits him best, and can change to the best view for any given situation. This is an interactive feature that gives the user the freedom to personalize his favorite way to drive the car.

| ![Figure5.1](https://github.com/filipecaixeta/gamecg/raw/master/report/5.1.jpg)  | ![Figure5.2](https://github.com/filipecaixeta/gamecg/raw/master/report/5.2.jpg)| ![Figure5.3](https://github.com/filipecaixeta/gamecg/raw/master/report/5.3.jpg)|
|:---:|:---:|:---:|
| Figure 5.1: Camera mode 1 following the car.  | Figure 5.2: Camera mode 3 rotating around the car. | Figure 5.3: Camera mode 3 rotating around the car. |

| ![Figure5.4](https://github.com/filipecaixeta/gamecg/raw/master/report/5.4.jpg)  | ![Figure5.5](https://github.com/filipecaixeta/gamecg/raw/master/report/5.5.jpg)| ![Figure5.6](https://github.com/filipecaixeta/gamecg/raw/master/report/5.6.jpg)|
|:---:|:---:|:---:|
| Figure 5.4: Camera mode 2 inside the car on default position.  | Figure 5.5: amera mode 2 inside the car on looking to the side defined by mouse position. | Figure 5.6: Camera mode 2 inside the car on looking back defined by mouse position. |
Figure 5: Different camera configurations, chosen by the user.

### HUD

In general, HUD stands for ‘Heads-Up Display’. It is usually applied on airplane cockpit and provides informations right on the window so it makes pilot not need to see below the window. In game programming, taking this basic idea, it means a sprite rendered as 2D on 3D scene to provide player with informations of current game situation.

The basic idea of rendering HUD is ‘just look like a 2D’, rendering on 3D scene but it moves as the camera moves. It generates an illusion that the sprite is in fixed location on screen.

In this project, the speedometer of car is rendered as HUD. It has two component, speedometer plate and needle. Needle spins by the amount of speed of the car. To rendering HUD, depth test is temporarily disabled to make sure the sprite not be interrupted other meshes, and after the rendering HUD, depth test is enabled again.

![Figure6](https://github.com/filipecaixeta/gamecg/raw/master/report/6.png) 

Figure 6: The idea of implementing HUD. Whenever the camera moves, HUD mesh follows the camera.


![Figure7](https://github.com/filipecaixeta/gamecg/raw/master/report/7.png) 

Figure 7: Final rendering with the HUD.


### Light

The Light model used was the Bling-Phong model. This model computes a normal vector for each vertex, then compute a normal for each point on the surface using linear interpolation. Normal points on the lines are computing with linear interpolation between vertex normals. Normal points between the lines are computed using linear interpolation between the normal lines. Then, one can compute an intensity for each pixel. This model is splitted into three coefficients: Ambient, Diffuse and Specular. The ambient coefficient gives an uniform sort of color to the object with a constant intensity. The diffuse coefficient approximates how photons will bounce off of the object's surface in several different directions. The specular coefficient takes into account the direct reflection of photons off the surface, affecting a very localized part of the object, giving a shiny finish.


### Game Controls

 The complete game controls are the following:

- W or Up directional key:        Accelerate forward;
- A or Left directional key:        Steer left;
- D or Right directional key:        Steer right;
- S or Down directional key:        Accelerate backward;
- SPACE key:                Brake;
- C:                        Change camera mode;
- V:                        Pause/Start camera rotation on camera mode 3;
- O:                         Change car color;
- P:                        Pause/Start radio;
- M:                        Change radio station;
- Q or ESC:                Quit Game.

### Conclusion


The racing game has a lot of the features we learned through the computer graphic course. It has been possible to create a really interactive game, were a lot of aspects has been implemented. Throughout the whole process we tried to remain true to the main purposes – It have to be creative and fun, while still intuitive. We felt we accomplished this very well for the tools and time we had!

We had several thoughts on how we could improve the user experience, to make the game even more fun. Mainly the game could use more objectives. The thought here is to bring more variation to the game, so the user doesn&#39;t feel bored when playing for a while. Ideas for the objectives are:

- Implement AI to play against;
- Make challenges (Achieve certain speed on a checkpoint on map, Fly this distance on ramp etc.);
- Play against friends;
- Implement high score;
- Implement terrains (paved/unpaved road, gravel road etc.).

If the user has more objectives, he will feel more entertained, which is the main purpose of making a game.

Also another feature would be to make it more interactive. Here are some thoughts on that:

- Personalize car (Styling);
- Tune car (Make it go faster etc.);
- Implement Turbo mode;
- See youtube videos in car / Make playlist;
- Destroy environment;
- Making differences to each cars.

A history line for the user to follow would also be a possibility. So basically there is a lot of ideas to improve the user experience, and if we had time to expand the project, the above ideas would be considered for future implementation.

## Build Instructions

Before compiling this code, one should install the following libraries:

- ASSIMP library
- Bullet library with Dynamic Libraries enabled.
- SDL 2 library
- SDL 2 mixer library
- OpenGL libraries
- Glut
- Glew
- Stb image loader library

To compile and run, type the following commands at the main folder:

```
$ mkdir build && cd build
$ cmake ..
$ make -j4
$ ./main
```
