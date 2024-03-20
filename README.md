## Project title
MangoKart

## Team members
Asanshay Gupta

Ethan Boneh

## Project description
We built a 3d graphics engine capable of supporting a simple driving game, in which players use a real-world steering wheel to drive around a map.

## Member contribution
A short description of the work performed by each member of the team.

**Asanshay:**
I started by designing the math behind the projections (nearly a complete vector math library) and developing a simple initial projection mechanism for individual points. I then built out a set of structs representing objects, faces, edges, and points, as well as adding to Ethan's initial logic to render them. I then built out object-scale and world-scale occlusion and a simple flat shader with a global light source. Finally, I worked on culling faces outside of the camera's view as well as higher performance versions of the `draw_triangle` and `draw_line` functions that appropriately clipped to the screen. I also built the testing framework for the engine, which included tests for camera movement, object animation, and light movement as well as drawing various objects.

**Ethan:**
I built out movement and rotation mechanics for the steering wheel, and helped build the world for the game. I also did bounds clipping, collision detection, and sorted objects for occlusion culling. I helped develop the initial generic 3d object-drawer, which Asanshay built out.


## References
Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work. 

- The graphics engine as a whole was inspired by [https://github.com/SuperAce100/graphicsengine](SuperAce100/graphicsengine), a homemade python graphics engine Asanshay built with his friend Elliott Faa in the summer of 2023. Our original intention was to base the maths on this engine's calculations but we found that the lookAt matrix method would be more efficient.
- Here is the Medium article that we based the lookAt matrix transformations on: [https://medium.com/@carmencincotti/lets-look-at-magic-lookat-matrices-c77e53ebdf78]
- Used Aditri's gyroscope driver. Translation of gyroscope measurements into movement/rotation/orientation was written by Ethan
- Used Calvin's `math_float.h` library provided to us

## Self-evaluation
How well was your team able to execute on the plan in your proposal?  
Any trying or heroic moments you would like to share? Of what are you particularly proud:
the effort you put into it? the end product? the process you followed?
what you learned along the way? Tell us about it!


**Ethan:** We were able to build out a minimalist version of our final product--a working graphics engine supporting a first-person game--which we are both excited to continue building on in the future. It was cool to explore the linear algebra behind 3d graphics--for instance, we originally planned to encode world coordinates using a homogenous coordinate system for orthographic projection, but found later that we could get pretty much the same results using just basic similar triangles that, when combined with a lookAt matrix, let us move the camera around without having to shift the world's coordinates.

Deciding that we wanted to take the game into the real world was also pretty tough--it took a while to confirm which of the gyroscope's measurements corresponded to the correct orientation, how to eliminate the noise, what the correct benchmarks were, and how to derive angular displacement properly without throwing the screen off every time we turned one way and then turned back.

Asanshay's work was genuinely instrumental in developing the graphics library, scaling performance, debugging, and developing features that made objects look realistic. He also made sure the code was clean and naming conventions were followed, which was greatly appreciated. 

**Asanshay:** One of our main goals in choosing such an ambitious project was to challenge ourselves in areas we weren't familiar with, like working with hardware for Ethan and working with linear algebra for myself. I had never taken a linear algebra class before, so it was really interesting to learn about things like vector cross and dot products through this project. In terms of our final execution, we still believe this is very much a work in progress--just seeing the things Daniel and Ege were able to achieve is hugely inspiring--but we're committed to taking it forward. 

In terms of the math itself, as Ethan mentioned we were able to simplify it significantly by using two matrix transforms through the lookAt model for projections, which also made those computations blazing fast - even at the very end when we had shaders and culling, we were still limited in speed by the drawing itself. One thing that I'm particularly proud of is the way we handled object-scale occlusion, which involves rendering each edge and face in the correct order to make sure everything is realistically depicted. To do this, we had to sort an array of edges and faces by z-coordinate, which took a custom data type that we called a `drawable`, which contained a `void *` to store a pointer to the face to be sorted and a type enum flag which told us whether we should render it as an edge or as a face. In addition, we're really focused on trying to make well-organized and clean code overall for our own sanity, which has been fun to work on. 

Big thanks to Aditri for her drivers/hardware support, Aditi for her help with 3D printing, and to Ethan of course for being a great partner on a really fun project!

## Photos
You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.
