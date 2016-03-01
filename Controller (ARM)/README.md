# DMX Communication protocol #
#### Embedded System Course Project

This code was written as a part of course project for Embedded System (EE5314), UTA.


#### Update ####
Latest: Dr. Losh suggested, in step 8, when the controller supposed to receive break in response of POLL, its better to wait 44us(timer or wait function) and then check if we have a break or not. I will try that in future, this might resolve the intermittent issues im having. One of the possible reasons is when ever we switch from tx to rx signal line might have glitch (need to check with oscilloscope).

*All the steps are working till 9. POLL command is tested with three hardware and worked. Having some issues with POLL command. LED activity (step 10) hasn't been implemented. 

 

#### Bugs ####
POLL command has some bugs:

1) If maximum data transfer is less then available device address's on the bus (Max < 512) then sometimes POLL doesn't works properly. Need to check.

2) If there is more then one device on the bus I had to call findDevice more then once. dmx512.c line 320. 

### Who do I talk to? ###
fahadmirza80@yahoo.com