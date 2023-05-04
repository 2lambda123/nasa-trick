# Planar Robotic Manipulator Simulation

SIM_robot is a 2-dimensional planar robotic simulation of a 2 degree of freedom manipulator.  It is a kinematic simulation, which means that it concerns itself solely with the position and velocity of the links but does not model or account for forces and torques.  This helps keep the simulation more simple and easier to comprehend, while also providing the necessary framework to serve as an adequate example of the general layout of a robotics simulation.  The controller described below and implemented in the simulation is both simple and extremely common.  It is used extensively in both kinematic and dynamic robotic modeling and control.

The simulation calculates both forward and inverse kinematics and implements some common control schemes to allow the user to control the manipulator in a variety of ways.  Only the tip (or end-effector) position of the manipulator is controlled, not the orientation of the last link.

Included is a graphics and user interface client which allows the user to view the motion of the robotic arm in the sim and to send moding and control inputs to the manipulator in real time.  A circle indicates the maximum extent of the workspace of the manipulator.


## Building the Simulation
In order to build a Trick simulation, the Trick-specific command trick-CP must be run to begin compilation process.  trick-CP is located in the Trick **```bin```** directory.  Simply run the trick-CP command in the SIM_robot directory.  Upon successful compilation, the following message will be displayed in the terminal:

```
=== Simulation make complete ===
```

The assocated graphics/user interface client should also be built automatically.

## Running the Simulation
In the SIM_robot directory:

```
% S_main_*.exe RUN_test/input.py
```
The Sim Control Panel, and a graphics client called "Robot Range" should appear.

The sim will come up in Freeze mode.  Click Start on the Trick Sim Control Panel and the simulation time should start advancing.

![graphics client display](images/GraphicsClient.png)


## Control Modes

There are three control modes available for this manipulator:  Single, Manual, and EEPos.  Each will be described below.  Furthermore, the gui provides a toggle to enable tracing the end-effector (or tip) position.

### Single Joint Mode
Single Joint Mode is the most straightfoward control mode in the simulation.  It simply commands a selected joint to move with the angular velocity commanded.  The user interface provides a button to select Single as your mode, a Joint select button to choose the desired joint, and a slider to command a desired joint velocity.

### Manual Mode
Manual Mode enables control of the motion of the end-effector with a desired velocity.  This mode commands the end-effector to move in the desired direction at the desired rate until either the velocity command is changed or the arm reaches a singularity (described below).  The gui has an "EE Velocity" interface for interacting in this mode.  The gray circle is like a dial, where the angle and location of the selected point in the circle determine the commanded direction and rate of the end-effector.  For example, clicking directly underneath the center of the circle on the edge will command the end-effector to move straight down as fast as the mode allows.  This input can be changed while moving, allowing the user to maneuver the end-effector however they see fit in real time.

### End-Effector Position Mode
This mode commands the end-effector to autonomously move to the selected point in the workspace.  The user clicks anywhere on the gui display and the end-effector will attempt to move to that location.  Singularities and reach limits may prevent it from reaching the point, however.  The manipulator makes no effort to avoid these as a lesson in owning the consequences of one's actions.

## Kinematics of the System
The kinematics of a robotic manipulator describe both the position and velocity of the manipulator at any point on the robot.  Kinematics do not include accelerations, forces, or moments in their description.

In this sim, we will discuss both forward and inverse kinematics.  Forward kinematics give the position/velocity of any location of interest on the manipulator given a set of joint angles/velocities.  Inverse kinematics go in the reverse:  Given a desired location/velocity for some point of interest on the manipulator, the equations supply the necessary joint angles/velocities.

The position of the end-effector is highly non-linear and heavily coupled with respect to the joint angles required to produce said position.  However, the velocity of the end-effector is linearly related to the joint velocities required to produce it, which forms the basis of the controller described below.  The non-linearity of the position equations, and the linearity of the velocity equations, will be shown in the next sections.

### How to Layout Points, Frames, and Joint Angles
The position of the end-effector relative to some fixed Base frame location can be calculated by knowing the joint angles of the manipulator and the lengths of the manipulator links.  First we will assign some points of interest along the manipulator that need to be kept track of during the forward kinematics calcualtions.  We will define the point around which the first link's rotation is centered as point $a$, the point of the second link's rotation $b$, and the tip of the manipulator (our end-effector) as $e$.  The distance between $a$ and $b$ will be defined as $L1$ and between $b$ and $e$ as $L2$.

![drawing of manipulator with two joint rotation points and end-effector tip annotated as a,b,e respectively](images/POIs.png)

But points of interest are of no use without frames of reference, so we need to define some frames in which we can define the relative locations of these points.  Let's define a fixed, static Base (or Origin) frame O centered on point $a$.  This frame will give us a reference frame to keep track of the location of points $a$, $b$, and $e$.  Point $a$ is fixed, point $b$ moves when the first joint angle changes, and point $e$ moves when either joint angle changes.  We need to keep track of how the links change their orientation with respect to both the base frame O and to each other.  To do this we define reference coordinate frames to each link that move along with it.  The convention for this sim is to point the X-axis of each frame along the length of these straight links, so that the X-axis points from each point of interest to the next ($a$ to $b$, $b$ to $e$).  Frame O is fixed, allowing a constant reference frame to refer back to at any time.

![The manipulator with a base frame O centered on point "a" which doesn't move, a frame "A" attached to the first link, and a frame "B" attached to the second link](images/Frames.png)

Note:  Frames are not fixed to one point, and can be moved around translationally wherever is convenient to help you visualize frame rotations.  However, their orientations with respect to other frames is *not* arbitrary, and in this example are defined by the joint angles $q$.  Sometimes it is convenient to place two frames on top of each other and co-located their origins (as seen in the figures above described generic frame rotations).  Sometimes it is convenient to place the frames for each link on the tip of the link, to help visual the relative rotation between the previous link and the next one.  You get to decide!  Put them anywhere and everywhere, they're free of charge.


We also need to keep track of the joint angles, since they can and will change during the run (otherwise it's less a manipulator and more a sculpture).  Let's define the joint angles as $q1$ and $q2$, making 0 rotation align all three frames so that they are all oriented the same way.  This way any rotation of a link can be easily described with joint angles $q1$ and $q2$.

![Figure showing the manipulator with joint angles q1 and q2, q1 being the rotation between the first link and the fixed base from O, and q2 being the rotation between the second link and the first link.](images/Angles.png)


Now everything of importance to the kinematics of the system has been named and labeled.  The base and joint frames allow us to keep track of the relative location of the points on the manipulator with respect to each other and the base, fixed inertial reference frame.  The joint angles allow us to describe every possible location of each of the points in any frame.

### Frame Rotations
Given that a robot will have *at least* as many frames as it has moveable links, we need to be able to transform the information about the manipulator from one frame to another.  This is called frame rotation.  One can construct a *rotation matrix* if the relative orientation of any two frames is known with respect to each other.  In this example, we will be rotating between frames O, A, and B (specifically, from A and B back to O to express everything in O).

In general, if you have two frames, A and B, rotated with respect to each other by an angle $q$, then you can determine how the unit vectors which make up frame B relate to frame A:

![Two coordinate axes that are co-located at their origins but rotated with respect to each other by angle q](images/RotMat.png)

Unit vectors (such as $\hat{X}_A$ and $\hat{Y}_A$) are by definition length 1.  The cosine of the angle $q$ is defined as the length of the adjacent side divided by the hypotenuse.  Since the length of the hypotenuse has to be 1, $cos(q)$ is equal to the length of the adjacent side.  For $sin(q)$, you use the opposite side instead of the adjacent.

![Two coordinate axes with the unit vectors of frame B split into x- and y-components and defined in terms of frame A's axes](images/UnitVecComps.png)

Given the information above, $X_B$ and $Y_B$ can be described with respect to frame A as:

![X axis of frame B equals cosine q in X direction plus sine q in Y direction of frame A](images/XhatBEq.png)


![Y axis of frame B equals negative sin q in X direction plus cosine q in Y direction of frame A](images/YhatBEq.png)

These equations together can be written in matrix form as

![Column vector of X and Y in frame B equals matrix cosine q sine q negative sine q cosine q time column vector of X and Y in frame A](images/MatrixEq_RotMat.png)

This matrix is called a rotation matrix, from Frame A to Frame B, commonly denoted as

![R subscript A superscript B equals rotation matrix from previous image](images/R_A_B.png)


This is the rotation matrix from Frame A to Frame B.  The subscript frame (here, A) is the "from" frame and the superscript frame (here, B) is the "to" frame.  If you take any vector described in frame A and pre-multiply it by this rotation matrix, you will get the same vector but expressed in frame B.  More often than not, you will actually be more concerned with rotation from Frame B back to Frame A.  To do this, you would need the inverse of the matrix (which reverses the frame transformation order).  Luckily, rotations matrices are *orthonormal*, and one characteristic of matrices like this is that the inverse of this matrix is equal to the transpose (which is much easier to find in general).

![Inverse rotation from A to B equals rotation from B to A equals transpose of rotation from A to B](images/orthonormality.png)

This is the bread and butter of kinematics, so you will definitely see this again.


### Position of the End-Effector
The position of the tip of manipulator can be described by calculating the vectors from points $a$ to $b$ and from $b$ to $e$ and adding them together.  Vectors can be added together if they are described in the same frame.  Vectors are simply lengths in a particular direction.

The vector from points $a$ to $b$ is a function of the joint angle $q1$ and the length of the link $L1$ (described in the base frame O).  The vector from points $b$ to $e$ is a function of the joint angle $q2$ and the length of the link $L2$ (described in the first link's frame A).

![Full manipulator layout with point "a", "b", and "e", frames "O", "A", and "B", and joint angles q1 and q2 showing relative rotations of each link with respect to the previous link](images/VectorComponents.png)

As can be seen in the figure above, the $x$ and $y$ components of the vector which goes from point $a$ to point $b$ is shown to be

![L1 times cosine q1](images/L1cos1.png)

and

![L1 time sine q1](images/L1sin1.png)

expressed in the base frame O.

The $x$ and $y$ components of the vector from $b$ to $e$ is shown to be

![L2 times cosine q2](images/L2cos2.png)

and

![L2 times sine q2](images/L2sin2.png)

expressed in the first link's frame, A.  

In frame A, the vector from $a$ to $b$ is simply

![Vector P sub "a" "b" in frame A equals column vector L1 0](images/P_ab_A.png)

because the axes of frame A rotate with the first link, so the vector always points along the $x$-axis of frame A.  Similarly, the vector from $b$ to $e$ in frame B is

![Vector P sub "b" "e" in frame B equals column vector L2 0](images/P_be_B.png)

If we add the vectors from $a$ to $b$ to $e$ together, we'll get a vector from $a$ to $e$.  This will tell us the location of the end-effector, which is what we're really after:

![Manipulator with vector from "a" to "b", vector from "b" to "e", and combined diagonal vector directly from "a" to "e"](images/Pae_Pab_Pbe.png)

However they can't be added until they are all expressed in the same frame.  To do that, we will rotate $P_{ab}$ and $P_{be}$ into frame O, the static base frame, and then add them together.  In order to do that, we will use rotation matrices as discussed in the previous section.

First, rotate the vector $P_{OB}$ into frame O by use of the rotation matrix from frame A into frame O (remembering that $R_A^B = R_B^A^T$)

![Full expanded vector form of vector P sub "a" "b" in frame O](images/Pab_full_xform_O.png)

This matches what was shown earlier, that the vector $P_{ab}$ in frame O had components that match what was just derived using rotation matrices.

For the vector $P_{BE}$, two transformations are needed.  First, from frame B back to A, then from A back to O.  Rotation matrices can be prepended as long as the origin and destination frames line up logically.  In this case, a vector in frame B is being rotated to frame A, and that resultant vector is then rotated to frame O:

![Vector from b to e in frame O equals rotation from frame A to frame O times rotation from frame B to frame A times column vector L2 0](images/Pbe_double_rotation.png)

which leads to 

![Vector from b to e in frame O with rotation matrices expanded](images/Pbe_single_rotation.png)

which finally gives

![Fully expanded vector from b to e in frame O](images/Pbe_O.png).

Now the vectors $P_{ab}$ and $P_{be}$ can be added together, since they are both expressed in frame O

![Vector from "a" to "b" plus vector from b to e all expressed in frame O](images/Pae_addition.png)

which, after some trigonometric wizardry, boils down to

![Vector from "a" to "e" in reduced form equals column vector with x component L1 cosine q1 plus L2 cosine q1+q2 and y component L1 sine q1 plus L2 sine q1 plus q2](images/Pae_O.png)


### Velocity of the End-Effector

### The Jacobian

### Singularities


## Control Schemes

### Single Joint Mode

### Manual Mode

### End-Effector Position Mode

### Singularity Management









