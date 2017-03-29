It is a game controller that tracks object based on the colour. This game controller simulates the 
functioning of the key strokes and controls the game without touching the keyboard. It is an attempt
to find a different method of human computer interaction and find new ways to interact with the computer.
This game controller is expected to work in every racing based game. This aims to provide a new experience in gaming.

It is basically a 9 step process that consist of:
1. Getting the video input from the webcam
2. Converting to HSV color model
3. Marker detection (Filtering)
4. Marker tracking
5. Observe user’s action from the markers
6. Interpret user’s actions
7. Take action based on user’s actions
8. Generate Key Events to control the game
9. Repeat these steps until ESC key is pressed.

These unnecessary small white patches can be eliminated by applying morphological operations.Morphological
operations can be achieved by erosion, followed by the dilation with the same structuring element.
