/*This library is designed with the help of the following github repo
*https://github.com/vrano714/max30102-tutorial-raspberrypi/blob/master/max30102.py?fbclid=IwAR3IUTtQ3xgon1Q3it_weUbdpVsIukNfI4z598LRBmG6shSCopwAqqP8zuA
*Author: Ismael Elsharkawi 
*Date: 21/5/2021
*Version: 1.0
*/
#include "max30102.h"
#include "ssd1306.h"
int hrarr[4];
int prevhr = 0;
int hr = 0;
int ir_valley_locs[15] = {0};
uint8_t channel=1, address=0xAE, gpio_pin=7;
uint8_t reg_data;
uint8_t data_written;
uint8_t led_mode=0x03;
uint32_t read_fifo_red_led, read_fifo_ir_led;//instead of multiple return statements, need to be defined as externs in the main program
uint8_t d[6] = {0,0,0,0,0,0};
uint32_t read_seq_red_buf[100];
int read_seq_ir_buf[100];
uint8_t amount=100;

void initialize(){
        /*We have to take care that there is an interrupt pin used*/
	      if(HAL_I2C_IsDeviceReady(&hi2c1, address, 1, 100)!=HAL_OK){HAL_Delay(1000);}
				
        reset();

        HAL_Delay(10);//sleep(1);  # wait 1 sec
				address = 0xAE;
        HAL_I2C_Mem_Read(&hi2c1, 0xAF, REG_INTR_STATUS_1, I2C_MEMADD_SIZE_8BIT, &reg_data, 1, 100);
        setup();
}
void shutdown(){
        data_written = 0x80;
        HAL_I2C_Mem_Write(&hi2c1, address, REG_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
}
void reset(){
        //data_written = 0x40;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);	
				address=0xAE;
				ssd1306_I2C_Write(address, REG_MODE_CONFIG, 0x40);
}

void setup(){
				address=0xAE;
				ssd1306_I2C_Write(address, REG_INTR_ENABLE_1, 0xc0);
        //data_written = 0xc0;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_INTR_ENABLE_1, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_INTR_ENABLE_2, 0x00);
        //data_written = 0x00;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_INTR_ENABLE_2, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_FIFO_WR_PTR, 0x00);
        //data_written = 0x00;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_FIFO_WR_PTR, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_OVF_COUNTER, 0x00);
        //data_written = 0x00;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_OVF_COUNTER, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_FIFO_RD_PTR, 0x00);
        //data_written = 0x00;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_FIFO_RD_PTR, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_FIFO_CONFIG, 0x4f);
        //data_written = 0x4f;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_FIFO_CONFIG, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_MODE_CONFIG, led_mode);
        //data_written = led_mode;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_MODE_CONFIG, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_SPO2_CONFIG, 0x27);
        //data_written = 0x27;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_SPO2_CONFIG, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_LED1_PA, 0x24);
        //data_written = 0x24;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_LED1_PA, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_LED2_PA, 0x24);
        //data_written = 0x24;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_LED2_PA, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
				ssd1306_I2C_Write(address, REG_PILOT_PA, 0x7f);
        //data_written = 0x7f;
        //HAL_I2C_Mem_Write(&hi2c1, address, REG_PILOT_PA, I2C_MEMADD_SIZE_8BIT, &data_written , 1, 10);
}
void set_config(uint8_t reg,uint8_t value){
		address=0xAE;
    HAL_I2C_Mem_Write(&hi2c1, address, reg, I2C_MEMADD_SIZE_8BIT, &value , 1, 10);
}

void read_fifo(){
    uint8_t reg_INTR1, reg_INTR2;
		address = 0xAE;
    HAL_I2C_Mem_Read(&hi2c1, 0xAE, REG_INTR_STATUS_1, I2C_MEMADD_SIZE_8BIT, &reg_INTR1, 1, 10);
    HAL_I2C_Mem_Read(&hi2c1, 0xAE, REG_INTR_STATUS_2, I2C_MEMADD_SIZE_8BIT, &reg_INTR2, 1, 10);
    HAL_I2C_Mem_Read(&hi2c1, 0xAE, REG_FIFO_DATA,
                I2C_MEMADD_SIZE_8BIT, d,
                6 , 25);
    read_fifo_red_led = (d[0] << 16 | d[1] << 8 | d[2]) & 0x03FFFF;
    read_fifo_ir_led = (d[3] << 16 | d[4] << 8 | d[5]) & 0x03FFFF;
}
/*The code here is initialized with a 200 element buffer because the python code had append function which is not an option in C*/

int read_sequential(){
    int i;
		int c999 = 0;
		for(int ii = 0; ii<4; ii++){
    for(i=0;i<amount;i++){
        while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1){} /*There is an implicit assumption that GPIOA pin 5 is the interrupt line*/
        read_fifo();
        read_seq_red_buf[i] = read_fifo_red_led;
        read_seq_ir_buf[i] = read_fifo_ir_led;
    }
		hrarr[ii] = calc(read_seq_ir_buf);
		if(hrarr[ii] == -999)
			c999++;
	}
		return (hrarr[0]+hrarr[1]+hrarr[2]+hrarr[3]+(999*c999))/(4-c999);
}

