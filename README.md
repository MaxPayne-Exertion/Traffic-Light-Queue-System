# Traffic-Light-Queue-System
Implementation of traffic light problem using Queue

baseline window generator using sdl2:
currently displays a rectangle (to test if sdl2 works)

addition of templating:
making the base queue to take in templates instead of integer,
additional addition of vehicle and lane classes and their respective queues

traffic generation:
the inital traffic generator was a basic random number generator that put in the vehicle in any one of the 4 lane, and the id was just a for loop with increments
current one has an id generator which produces an 8lenght alpha numeric vehicle code, and places them to one of the 4 lane files


road representation:
reperesentation of the road using SDL, the 4 roads each with 3 lanes

current status of render:
<img width="828" height="866" alt="image" src="https://github.com/user-attachments/assets/3c9a0eab-7e31-4fad-b76f-dc6df9c4ea46" />
