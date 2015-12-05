# asteroids
This has been an on going project for a course of mine, its a cute little 3D-ish version of classic asteroids.


BUILDING:

To build and run you will need freeglut for MSVC 
(available at: http://files.transmissionzero.co.uk/software/development/GLUT/freeglut-MSVC.zip)
If you like suffering you can also set it up in eclipse with freeglut for MinGW. I haven't tried bringing it over to linux quite yet, but I know there is a version  of freeglut out there for linux, however I couldn't tell you how to set it up.

To build this in Visual Studio (easiest way), simply make a Win32 Console Application visual studio project, add freeglut for MSVC's includes to the projects additional include directiories, and its 32bit lib file to the additional linker libraries.


PLAYING:

The controls are pretty simple for this game.

Controls:

Left Arrow   --  rotate your spaceship counter-clockwise

Right Arrow  --  rotate your spaceship clockwise

X            --  accelerate your spaceship

Z            --  fire missles

Escape       --  quit game

Right Click  --  view menu

Scoring/Lives:

Your score will be displayed in the upper left corner of the screen, as well as the number of times you can die before the game restarts.

Default Lives                         ---     5

Score required for each extra life    ---     7000

Big asteroid worth                    ---     20

Medium asteroid worth                 ---     50

Small asteroid worth                  ---     100

Big alien worth                       ---     200

Small alien worth                     ---     1000

Whenever all the asteroids on a screen/level of the game are destroyed, a new screeen/level will start with one more than the previous number of asteroids up until 6 asteroids.

