#include "ee_24lc256.h"

void eepromInit (void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	/*!< I2C Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	/*!< GPIOB Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Reset I2C peripheral */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);

	/* Release reset signal of I2C IP */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);

	/*!< GPIO configuration */
	/*!< Configure I2C pins: SCL */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure I2C2 pins: SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

	/*!< I2C configuration */
	/* I2C peripheral configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = I2C_ADDRESS7_MAIN;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;

	/* I2C configuration after enabling it */
	I2C_Init(I2C2, &I2C_InitStructure);

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C2, ENABLE);

	/* Enable the sEE_I2C peripheral DMA requests */
	//I2C_DMACmd(I2C2, ENABLE);
}

void eepromWriteByte (uint16_t address, uint8_t data)
{
	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	/*!< Send START condition */
	I2C_GenerateSTART(I2C2, ENABLE);

	/*!< Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	/*!< Send EEPROM address for write */
	I2C_Send7bitAddress(I2C2, I2C_ADDRESS7_EEPROM, I2C_Direction_Transmitter);

	/*!< Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));


	/*!< Send the EEPROM's internal address to write to : MSB of the address first */
	I2C_SendData(I2C2, (uint8_t)((address & 0xFF00) >> 8));

	/*!< Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/*!< Send the EEPROM's internal address to write to : LSB of the address */
	I2C_SendData(I2C2, (uint8_t)(address & 0x00FF));

	/*!< Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/*!< Send the EEPROM's data to write */
	I2C_SendData(I2C2, data);

	/*!< Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/*!< Send STOP condition */
	I2C_GenerateSTOP(I2C2, ENABLE);

	eepromWaitReady();
}

uint8_t eepromReadByte (uint16_t address)
{
	uint8_t dataIn;

	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	/*!< Send START condition */
	I2C_GenerateSTART(I2C2, ENABLE);

	/*!< Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	/*!< Send EEPROM address for write */
	I2C_Send7bitAddress(I2C2, I2C_ADDRESS7_EEPROM, I2C_Direction_Transmitter);

	/*!< Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*!< Send the EEPROM's internal address to write to : MSB of the address first */
	I2C_SendData(I2C2, (uint8_t)((address & 0xFF00) >> 8));

	/*!< Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/*!< Send the EEPROM's internal address to write to : LSB of the address */
	I2C_SendData(I2C2, (uint8_t)(address & 0x00FF));

	/*!< Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/*!< Send START condition */
	I2C_GenerateSTART(I2C2, ENABLE);

	/*!< Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	/*!< Send EEPROM address for write */
	I2C_Send7bitAddress(I2C2, I2C_ADDRESS7_EEPROM, I2C_Direction_Receiver);

	/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR) == RESET);

	/*!< Disable Acknowledgement */
	I2C_AcknowledgeConfig(I2C2, DISABLE);

	/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
	(void)I2C2->SR2;

	/*!< Send STOP Condition */
	I2C_GenerateSTOP(I2C2, ENABLE);

	/* Wait for the byte to be received */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET);

	dataIn = I2C_ReceiveData(I2C2);

	/* Wait to make sure that STOP control bit has been cleared */
	while(I2C2->CR1 & I2C_CR1_STOP);

	/*!< Re-Enable Acknowledgement to be ready for another reception */
	I2C_AcknowledgeConfig(I2C2, ENABLE);

	return dataIn;
}

void eepromWriteData (uint16_t address, uint8_t* data, uint16_t length)
{
	uint32_t i;
	for(i=0; i<length; i++)
		 eepromWriteByte (address+i, data[i]);
}

void eepromReadData (uint16_t address, uint8_t* data, uint16_t length)
{
	uint32_t i;
	for(i=0; i<length; i++)
		 data[i] = eepromReadByte (address+i);
}

void eepromWaitReady (void)
{
	uint32_t tmpSR1;
	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	while (1){
		/*!< Send START condition */
		I2C_GenerateSTART(I2C2, ENABLE);

		/*!< Test on EV5 and clear it */
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

		/*!< Send EEPROM address for write */
		I2C_Send7bitAddress(I2C2, I2C_ADDRESS7_EEPROM, I2C_Direction_Transmitter);

		do{
			/* Get the current value of the SR1 register */
			tmpSR1 = I2C2->SR1;
		}
		/* Keep looping till the Address is acknowledged or the AF flag is
		set (address not acknowledged at time) */
		while((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);


		/* Check if the ADDR flag has been set */
		if (tmpSR1 & I2C_SR1_ADDR){
			/* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already
			 been read) */
			(void)I2C2->SR2;

			/*!< STOP condition */
			I2C_GenerateSTOP(I2C2, ENABLE);

			/* Exit the function */
			return;
		}
		else{
			/*!< Clear AF flag */
			I2C_ClearFlag(I2C2, I2C_FLAG_AF);
		}
	}
}

/* example usage */
/*
eepromWriteByte(RELAY_TICK_ADD, 0xA5);
ticks = eepromReadByte(RELAY_TICK_ADD);
*/
