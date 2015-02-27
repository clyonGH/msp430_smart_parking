End-of-study project by Mathieu GERIER and Céline LY

These source code files are written for the CC430F5137 that are included in the electronic cards designed by Mr. BOÉ and Mr. VANTROYS. For our project, they represent the wireless sensor network.

How to compile
Depending on whether you want to compile a sensor or a hub:
- sensor: make tx SRC=num_sensor DEST=num_hub
- hub: make rx ID=num_hub NEXT=num_next_hub PARK=num_parking
with num_sensor being the id of the sensor and num_hub being the id of the hub to which the sensor "belongs", if a sensor is compiled OR with num_hub being the id of the hub, num_next_hub being the id of the next hub where the data should be sent and num_parking being the id of the parking to which the hub "belongs", if a hub is compiled.

How to flash
The CC430F5137 have to be powered and linked to the Test and Reset of a programmer (eg MSP430 Launchpad). Then the code can be deployed with: sudo make deploy.
