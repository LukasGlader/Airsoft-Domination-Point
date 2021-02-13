# Airsoft Domination Point
Arduino based score tracker for airsoft domination games.

# Features:
 - Keeps track of the scores for up to three teams (named Blue, Yellow and Orange).
 - Using a 128x64px display the maximum time that can be displayed is 99h59m59s per team.
 - Retains the current owner team of the captured point as well as the current score (+/- 1 minute) in case the power supply is disconnected.

# Usage:
 #### 1. Connect power source
  - The device will now display the accumulated scores as they were at last power-down.
  - The device is not captured by anyone and thus not adding points to any team.

 #### 2. Capture the point by pressing one of the three team buttons
  - The device will now indicate that the point is captured by the given team.
  - The device will start adding time to the capturing team.
  
 #### 3. Re-capture the point by another team
 - The device will now indicate that the new team has captured the point.
 - The score for the previous owning team will stop increasing
 - The score for the new owner will start increasing.
   


#### The point can be completely "un-captured" by either disconnecting and reconnecting the power source or by pressing all three buttons simultaneously. 
  - The device will not indicate any team as the owner
  - The current scores will still be displayed but none of the scores will be increasing.
  - Note that the device will ignore any key presses for two seconds after this is done.

#### The point can be uncaptured as well as resetting the score to 0 for all teams by holding down all three buttons simultaneously for at least 5 seconds.
  - Note that the device will ignore any key presses for two seconds after this is done.

# Required Hardware:
- Arduino compatible device with EEPROM (tested on an Arduino Nano Every).
- 3x momentary switches
- I2C 128x64 pixel display

Note:
The EEPROM will be written to once every minute when captured, as well as each time the owner changes. According to my reasearch on the wear on EEPROM when writing to it this feature should be reliable for several months of running time.


# Required Software:
U8x8 library for OLED display, look up tutorial for the display you are using (i.e SSD1306 based display).




# Examples of the display when device is in use:

## After first start
<pre>
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                       |
| Yellow:                     |
| Orange:                     |
|_____________________________|
</pre>

## After capture by blue team
<pre>
 _____________________________
| Current owner: Team Blue    |
|                             |
| Score:                      |
| Blue:                  1:05 |
| Yellow:                     |
| Orange:                     |
|_____________________________|
</pre>

## After re-capture by orange team
<pre>
 _____________________________
| Current owner: Team Orange  |
|                             |
| Score:                      |
| Blue:                  1:05 |
| Yellow:                     |
| Orange:             1:06:46 |
|_____________________________|
</pre>

## After power outage or "un-capturing" by game admin
<pre>
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                 57:31 |
| Yellow:                     |
| Orange:             1:06:46 |
|_____________________________|
</pre>

## After score reset by game admin
<pre>
 _____________________________
| Current owner:              |
|                             |
| Score:                      |
| Blue:                       |
| Yellow:                     |
| Orange:                     |
|_____________________________|
</pre>
