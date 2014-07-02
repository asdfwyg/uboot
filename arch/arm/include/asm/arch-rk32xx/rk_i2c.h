/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 * Peter, Software Engineering, <superpeter.cai@gmail.com>.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _RK_I2C_H_
#define _RK_I2C_H_


typedef enum rk_i2c_bus_ch {
	I2C_BUS_CH0,
	I2C_BUS_CH1,
	I2C_BUS_CH2,
	I2C_BUS_CH3,
	I2C_BUS_CH4,
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	I2C_BUS_CH5,
#endif
	I2C_BUS_MAX
} rk_i2c_bus_ch_t;

#endif /* _RK_I2C_H_ */













