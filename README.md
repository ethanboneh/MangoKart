## Project title
MangoKart

## Team members
Asanshay Gupta
Ethan Boneh

## Project description
We built a 3d graphics engine capable of supporting a simple driving game, in which players use a real-world steering wheel to drive around a map.

## Member contribution
A short description of the work performed by each member of the team.

Asanshay:

Ethan:
I built out movement and rotation mechanics for the steering wheel, and helped build the world for the game. I also did bounds clipping, collision detection, and sorted objects for occlusion culling. I helped develop the initial generic 3d object-drawer, which Asanshay built out.


## References
Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

- Used Aditri's gyroscope driver. Translation of gyroscope measurements into movement/rotation/orientation was written by Ethan
- used math_float.h library provided to us

## Self-evaluation
How well was your team able to execute on the plan in your proposal?  
Any trying or heroic moments you would like to share? Of what are you particularly proud:
the effort you put into it? the end product? the process you followed?
what you learned along the way? Tell us about it!


Ethan: We were able to build out a minimalist version of our final product--a working graphics engine supporting a first-person game--which we are both excited to continue building on in the future. It was cool to explore the linear algebra behind 3d graphics--for instance, we originally planned to encode world coordinates using a homogenous coordinate system for orthographic projection, but found later that we could get pretty much the same results using just basic similar triangles that, when combined with a lookAt matrix, let us move the camera around without having to shift the world's coordinates.

Deciding that we wanted to take the game into the real world was also pretty tough--it took a while to confirm which of the gyroscope's measurements corresponded to the correct orientation, how to eliminate the noise, what the correct benchmarks were, and how to derive angular displacement properly without throwing the screen off every time we turned one way and then turned back.

Asanshay's work was genuinely instrumental in developing the graphics library, scaling performance, debugging, and developing features that made objects look realistic. He also made sure the code was clean and naming conventions were followed, which was greatly appreciated. 

## Photos
You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.
