/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 *
 * Configuation settings for the rk3xxx chip platform.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <power/rockchip_power.h>

#include "../config.h"
#include "key.h"

#define SARADC_BASE             RKIO_SARADC_PHYS

#define read_XDATA(address) 		(*((uint16 volatile*)(address)))
#define read_XDATA32(address)		(*((uint32 volatile*)(address)))
#define write_XDATA(address, value) 	(*((uint16 volatile*)(address)) = value)
#define write_XDATA32(address, value)	(*((uint32 volatile*)(address)) = value)

int gpio_reg[]={
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	RKIO_GPIO0_PHYS,
	RKIO_GPIO1_PHYS,
	RKIO_GPIO2_PHYS,
	RKIO_GPIO3_PHYS,
	RKIO_GPIO4_PHYS,
	RKIO_GPIO5_PHYS,
	RKIO_GPIO6_PHYS,
	RKIO_GPIO7_PHYS,
	RKIO_GPIO8_PHYS
#else
	#error "PLS check CONFIG_RKCHIPTYPE for key."
#endif
};

extern void DRVDelayUs(uint32 us);

gpio_conf       charge_state_gpio;
gpio_conf       power_hold_gpio;

key_config	key_rockusb;
key_config	key_recovery;
key_config	key_fastboot;
key_config      key_power;


/*
    �̶�GPIOA_0����Ϊ��д����,ϵͳ���ֲ���ʹ�øÿ�
*/
int GetPortState(key_config *key)
{
	uint32 tt;
	uint32 hCnt = 0;
	adc_conf* adc = &key->key.adc;
	int_conf* ioint = &key->key.ioint;

	if(key->type == KEY_AD)
	{
		// TODO: clkû������
		for(tt = 0; tt < 10; tt++)
		{
			// read special gpio port value.
			uint32 value;
			uint32 timeout = 0;

			write_XDATA32( adc->ctrl, 0);
			DRVDelayUs(1);
			write_XDATA32( adc->ctrl, 0x0028|(adc->index));
			DRVDelayUs(1);
			do {
				value = read_XDATA32(adc->ctrl);
				timeout++;
			} while((value&0x40) == 0);
			value = read_XDATA32(adc->data);
			//printf("adc key = %d\n",value);
			//DRVDelayUs(1000);
			if( value<=adc->keyValueHigh && value>=adc->keyValueLow)
				hCnt++;
		}
		write_XDATA32( adc->ctrl, 0);
		return (hCnt>8);
	} else if (key->type == KEY_INT) {
		int state;

		state = gpio_get_value(key->key.ioint.gpio);
		if (ioint->pressed_state == 0) { // active low
			return !state;
		} else {
			return state;
		}
	}

	return 0;
}


int checkKey(uint32* boot_rockusb, uint32* boot_recovery, uint32* boot_fastboot)
{
	*boot_rockusb = 0;
	*boot_recovery = 0;
	*boot_fastboot = 0;

	printf("checkKey\n");

	if(GetPortState(&key_rockusb))
	{
		*boot_rockusb = 1;
		//printf("rockusb key is pressed\n");
	}
	if(GetPortState(&key_recovery))
	{
		*boot_recovery = 1;
		//printf("recovery key is pressed\n");
	}
	if(GetPortState(&key_fastboot))
	{
		*boot_fastboot = 1;
		//printf("fastboot key is pressed\n");
	}

	return 0;
}

void RockusbKeyInit(key_config *key)
{
	key->type = KEY_AD;
	key->key.adc.index = 1;	
	key->key.adc.keyValueLow = 0;
	key->key.adc.keyValueHigh= 30;
	key->key.adc.data = SARADC_BASE;
	key->key.adc.stas = SARADC_BASE+4;
	key->key.adc.ctrl = SARADC_BASE+8;
}

void RecoveryKeyInit(key_config *key)
{
	key->type = KEY_AD;
	key->key.adc.index = 1;	
	key->key.adc.keyValueLow = 0;
	key->key.adc.keyValueHigh= 30;
	key->key.adc.data = SARADC_BASE;
	key->key.adc.stas = SARADC_BASE+4;
	key->key.adc.ctrl = SARADC_BASE+8;
}


void FastbootKeyInit(key_config *key)
{
	key->type = KEY_AD;
	key->key.adc.index = 1;	
	key->key.adc.keyValueLow = 170;
	key->key.adc.keyValueHigh= 180;
	key->key.adc.data = SARADC_BASE;
	key->key.adc.stas = SARADC_BASE+4;
	key->key.adc.ctrl = SARADC_BASE+8;
}


void PowerKeyInit(void)
{
	//power_hold_gpio.name
	key_power.type = KEY_INT;
	key_power.key.ioint.name = "power_key";
	key_power.key.ioint.gpio = (GPIO_BANK0 | GPIO_A5);
	key_power.key.ioint.flags = IRQ_TYPE_EDGE_FALLING;
	key_power.key.ioint.pressed_state = 0;
	key_power.key.ioint.press_time = 0;
}


int power_hold(void)
{
	if (get_rockchip_pmic_id() == PMIC_ID_RICOH619)
		return ricoh619_poll_pwr_key_sta();
	else
		return GetPortState(&key_power);
}


void key_init(void)
{
	charge_state_gpio.name = "charge_state";
	charge_state_gpio.flags = 0;
	charge_state_gpio.gpio = (GPIO_BANK0 | GPIO_B0);
	gpio_direction_input(charge_state_gpio.gpio);

	//power_hold_gpio.name

	RockusbKeyInit(&key_rockusb);
	FastbootKeyInit(&key_fastboot);
	RecoveryKeyInit(&key_recovery);
	PowerKeyInit();
}


void powerOn(void)
{
	if(power_hold_gpio.name != NULL)
		gpio_direction_output(power_hold_gpio.gpio, power_hold_gpio.flags);
}

void powerOff(void)
{
	if(power_hold_gpio.name != NULL)
		gpio_direction_output(power_hold_gpio.gpio, !power_hold_gpio.flags);
}

