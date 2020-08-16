# airsoft_domination_point
Arduino based score tracker for airsoft domination games.

The finished device will consist of an arduino connected to an i2c display and three buttons, one for each team.

Usage:
1. Connect power source
  - The device will now display the accumulated scores as they were at last power-down.
  - The device is not captured by anyone and thus not adding points to any team.

2. Capture the point by pressing one of the three team buttons
  - The device will now indicate that the point is captured by the given team.
  - The device will start adding time to the capturing team.
  
3. Re-capture the point by another team
   - The device will now indicate that the new team has captured the point.
   - The score for the previous owning team will stop increasing
   - The score for the new owner will start increasing.
   
The point can be completely "un-captured" by either disconnecting and reconnecting the power source or by pressing all three buttons simultaneously. 
  - The device will not indicate any team as the owner, the current scores will still be displayed but none of the scores will be increasing.
  - Note that the device will ignore any key presses for two seconds after this is done.

The point can be uncaptured as well as resetting the score to 0 for all teams by holding down all three buttons simultaneously for at least 5 seconds.
  - Note that the device will ignore any key presses for two seconds after this is done.

Features:
- Keeps track of the scores for up to three teams (named Blue, Yellow and Orange).
- Using a 128x64px display the maximum time that can be displayed is 99h59m59s per team.
- Retains the current owner team of the captured point as well as the current score (+/- 1 minute) in case the power supply is disconnected.


Required Hardware:

- Arduino compatible device with EEPROM (tested on an Arduino Nano Every)
- 3x buttons
- I2C 128x64 pixel display

Required Software:
U8x8 library for OLED display, look up tutorial for the display you are using (i.e SSD1306 based display).




Examples of the display when device is in use:

After first start
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                       |
| Yellow:                     |
| Orange:                     |
|_____________________________|

After capture by blue team
 _____________________________
| Current owner: Team Blue    |
|                             |
| Score:                      |
| Blue:                  1:05 |
| Yellow:                     |
| Orange:                     |
|_____________________________|

After re-capture by orange team
 _____________________________
| Current owner: Team Orange  |
|                             |
| Score:                      |
| Blue:                  1:05 |
| Yellow:                     |
| Orange:             1:06:46 |
|_____________________________|

After power outage or "un-capturing" by game admin
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                 57:31 |
| Yellow:                     |
| Orange:             1:06:46 |
|_____________________________|

After score reset by game admin
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                       |
| Yellow:                     |
| Orange:                     |
|_____________________________|
