## Project Description
In this project, a fitness band is going to be implemented using the Nucleo STM32L432KC micro-controller and a group of sensors. The fitness band is going to collect the following information: 
* Heart Rate
* Temperature
* Number of steps

The collected information is going to be displayed on the LED monitor. 
The steps and heart rate will be displayed on a web server.
A LED is blinked if the daily goal is reached.

## System Diagram

<img src="https://i.ibb.co/WfHNXXK/192120545-559483108372919-4248482154184749217-n.png"/>

## Components Needed
### Hardware Components
* LCD Monitor: 0.96 OLED Display
* Nucleo STM32
* ESP32
* LED
* DS3231:
* ## Nucleo STM32L432KC:
<img src="https://cdn-reichelt.de/bilder/web/xxl_ws/A300/NUCLEO_L432KC_01.png" width="400" height="400" />

### Configuration:
<img src="https://i.ibb.co/SJQvjpV/192257017-2994295410799858-7231297659618842066-n.png" />

* ## DS3231:
![alt text](https://cdn.shopify.com/s/files/1/0672/9409/products/ds3231-rtc.png?v=1578177227)

•The DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature-compensated crystal oscillator (TCXO) and crystal. 
It incorporates a battery input and maintains accurate timekeeping when main power to the device is interrupted. 
The clock operates in either the 24-hour or 12-hour format with an active-low AM/PM indicator. The DS3231 RTC has a built-in alarm functions as well as a temperature sensor. 
Address and data are transferred serially through an I2C bidirectional bus.
* ## Temperature Sensor: DS3231(I2C)
### Description:
It is a Digital Temperature Sensor with ±3°C accuracy.
### Operation:
•	The DS3231 has TCXO which is formed of temperature sensor, oscillator, and control logic. 

•	The on-chip temperature sensor senses the temperature.

•	The controller reads the output of that sensor.

•	Temperature conversion occurs on initial application of VCC and once every 64 seconds afterwards.

### Configuration:
1.	Write the register address values of the temperature sensing into a variable.

2.	Those addresses are 0x11 and 0x12 for the MSB and LSB of the temperature.

3.	Then, use HAL_I2C_Transmit to transmit the register address so they can be read from.

4.	Use HAL_I2C_Receive to read values from the temperature registers. 

### Output:
•	The output is received in the two variables tempMSB and tempLSB.

•	Then, the tempLSB is shifted by 6 to get the most significant bits that have the data since the rest of the register is empty.

•	Then, the final value of temperature is calculated by adding the value of the MSB of temperature to the shifted by 6 LSB value and multiplying it by 0.25

* ## RTC : DS3231(I2C)
### Operation:
The DS3231 has a Real-Time clock that counts seconds, minutes, hours, date of the month, month, day of the week, and year, with leap-year compensation valid up to 2100.
### Configuration:
1.	Write the value of seconds to register 0x00. 

2.	Use HAL_I2C_Master_Transmit to transmit the values to register 0xD0

3.	Write the value of minutes to register 0x01

4.	Use HAL_I2C_Master_Transmit to transmit the values to register 0xD0

5.	Write the value of hours to register 0x02 

6.	Use HAL_I2C_Master_Transmit to transmit the values to register 0xD0

7.	Use HAL_I2C_Master_Transmit to send the seconds, minutes and hours register addresses to be read from

8.	Use HAL_I2C_Receive to read the seconds, minutes, and hours values.

### Output:
•	The output is the values of seconds, minutes, and hours in hexadecimal.

•	Then, it is passed to hexaToAscii function to convert the value into Ascii so that it could be displayed.

* ## Heart Rate Sensor (MAX30102)
<img src="https://electropeak.com/pub/media/catalog/product/cache/fa232c603e0403143aafcf902b42df2f/_/s/_s_e_sen-22-005-1.jpg" width="400" height="400" />

* MAX30102 is a pulse oximeter that we use to measure the heart rate of the user.
* The sensor shines a light through the skin, then reads back the reflected light to measure the blood flow.
* The measured blood flow indicates how many beats per minute there are.
* To get the number of beats per minute we first have to get the number of peaks or spikes which correspond to a beat each as blood flows the most then.
* For that, the sensor takes 25 samples per second. But since a person’s heartbeat is between 60-100 on average, that means we will get slightly more than one peak per second.
* That is why we take 100 samples over 4 seconds. This means that we now will have an average of about 5 peaks to get our average from.
* Now to get the average heart rate, we first have to get the peaks and to do that we get the maximum value of our sample which is greater than the surrounding values.
* We then remove peaks that are too close to each other as they are most likely fluctuations in the reading.
* After that what we have is a clean array of peaks and their index number (sample number).
* Since we now have a sorted array of peaks, we can now get the interarrival time (sample number) by subtracting the index of each one from the one after it.
* Then we average these values to get the average inter-arrival time.
* After we get the average inter-arrival time, we can now get the heart rate by using the equation: bpm = (60*25)/avg inter-arrival.
* We use this equation because the average interarrival time is just the average number of sample we have to wait between every peak (beat) and the next, and we have 25 samples per second so we have 25*60 samples per minute.
* So we multiply (samples/minute) *  (samples/beat) to get beats/minute (bpm).
* For example, if we take the diagram below, we have 3 peaks with inter-arrival times of 18 and 22.
* The average inter-arrival time is now 20, so the heart rate is (25 * 60) / 20 = 75 bpm.
<img src="https://i.ibb.co/PmBQfpy/192035025-1190713361379689-2014508003744052149-n.png"/>

* ## ESP32:
<img src="https://images-na.ssl-images-amazon.com/images/I/61RixSStGBL._AC_SL1000_.jpg" width="400" height="400" />

* We are using the ESP32 as a webserver to display our sensor readings that we get from the stm32 MCU.
* First we get the values we want to send from the stm32 and send them over UART each with an id to identify which value is being sent.
* In our case we sent 1 for the heart rate and 2 for the number of steps.
* Then on the esp32 we received those values and put them in their correct variables.
* We have our webserver hosted on the ESP32 which has our readings which we can then access over wifi on our phones.
* The page auto refreshes every 2 seconds to make sure the current displayed values are always up to date.

* ## Accelerometer: ADXL 345 (I2C) 
<img src="https://components101.com/asset/sites/default/files/components/ADXL345-Accelerometer-Module.jpg" width="500" height="333" />

### Description:
• The ADXL345 is a low-power, complete 3-axis Micro Electro-Mechanical System. 

• It is an accelerometer module that can be used to measure both dynamic and static acceleration.
 
• The module has both I2C and SPI interfaces. It has a dynamic range of +/-16g.

### Structure:
• It consists of a polysilicon surface which is a micromachined structure that is built on top of a silicon wafer. 

• There exist polysilicon springs that suspend the structure over the surface of the wafer.

• It also has differential capacitors that consist of independent fixed plates and other plates attached to the moving mass. 

### Operation:
• The polysilicon springs provide a resistance against forces that happen to the applied acceleration. 

• Deflection of the structure is measured using differential capacitors.

• Any acceleration occurring along any of the three axes deflects the mass and unbalances the differential capacitor.
 
• This leads to a sensor response in which the change in capacitance is converted to an output voltage that is proportional to the acceleration on each axis. This response is directly proportional to the acceleration. 

• To determine the magnitude and acceleration polarity, Phase-sensitive demodulation is used.


### Configuration:

1. Write 0x1 to the Register- 0x31 that is meant for DATA_FORMAT. This makes the range +/-4g

2. Write 0x00 to Register-0x2D which is the POWER_CTL register to reset all bits.

3. Write 0x08 to Register-0x2D to make the frequency of readings = 8 Hz

4. Read values from Register 0x32 to Register 0x37 to get the acceleration values on the X, Y, and Z- axes.

### Algorithm:

•	The output values are periodically measured acceleration values on the three axes. 

•	The magnitude of the acceleration is measured by getting the total acceleration from the three axes.

• The average total acceleration value is measured by getting the average of two consecutive samples. 

• A threshold value is set to compare the total acceleration value to. This is to know whether a step happened or not so it can take real steps into consideration and ignore movements that do not constitute actual steps. 

• A step is counted when the average total acceleration is more than the preset threshold value.

### Algorithm Limitations:
•	This algorithm is not accurate enough to detect the correct number of steps.

•	This is because it detects all types of movement without giving consideration to the movement pattern that dictates if a step was taken.

•	For example, it can detect the hand movement as a step since it is only measuring acceleration.

•	To get accurate results, pattern detection machine learning algorithms will be more suitable.


### Software Components
* CubeMX
* Keil uVision
## Initial Presentation Video
Link to Video: https://drive.google.com/file/d/1NLI5KUsdD9AfIiT6ZDZ2axOaACTywNjs/view?usp=sharing
## Prototype Circuit
![alt text](https://i.ibb.co/4dTQwJk/current.png)
## Prototype Demo Video:
https://drive.google.com/file/d/1Rvp0v-eiQUCg7r-bVMzAHm0exDvMAFfP/view?usp=sharing
## Final Circuit:
![alt text](https://i.ibb.co/Zz3Zzbt/192102916-497275594932144-2805342142864678019-n.png)
## Final Demo:
https://drive.google.com/file/d/1EmY8mwXyZJKOsBeBLNb0nM642s4Nr5P8/view?usp=sharing

## Final Presentation:
https://drive.google.com/drive/folders/1krpi-uHM0iI3UUIrVHf4TGSxWjNYV5SS?usp=sharing

## Conclusion
* We implemented a prototype for a fit band that measures ambient temperature, heart rate and number of steps. 
* The values of the ambient temperature, heart rate, number of steps and the Real-Time clock are displayed on the 0.96 OLED Display. 
* The values of the heart rate and the number of steps are displayed on a web server using Esp32.
* The project had certain limitations. These limitations are:
 1. The SPO2 levels are not measured.
 2.  The calculation of the steps using the ADXL345 is not accurate since it needs pattern detection to consider the correct movement and calculate steps accurately.
