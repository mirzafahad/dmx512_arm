# DMX Communication protocol #
##### Embedded System Course Project

This code was written as a part of course project for Embedded System (EE5314), UTA.


### Update ###
All the steps are working. POLL command is tested with three hardware and worked. Having some issues with POLL command.
 

#### Bugs ####
POLL command has some bugs:
1) If maximum data transfer is less then available device address's on the bus (Max < 512) then sometimes POLL doesn't works properly. Need to check.

2) If there is more then one device on the bus I had to call findDevice more then once. dmx512.c line 320. 

### Who do I talk to? ###
fahadmirza80@yahoo.com