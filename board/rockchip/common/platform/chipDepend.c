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
#include <asm/io.h>
#include "../config.h"


/***************************************************************************
��������:��ʱ
��ڲ���:us��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayUs(uint32 count)
{
	__udelay(count);
}



/***************************************************************************
��������:��ʱ
��ڲ���:ms��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayMs(uint32 count)
{
	DRVDelayUs(1000*count);
}


/***************************************************************************
��������:��ʱ
��ڲ���:s��
���ڲ���:
���ú���:
***************************************************************************/
void DRVDelayS(uint32 count)
{
	while (count--)
		DRVDelayMs(1000);
}


//����Loader�����쳣����
//ϵͳ������ָ����sdramֵΪ�ñ�־���������ɽ���rockusb
//ϵͳ����ʧ�ܱ�־
uint32 IReadLoaderFlag(void)
{
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	return readl(RKIO_PMU_PHYS + PMU_SYS_REG0);
#else
	#error "PLS check CONFIG_RKCHIPTYPE for loader flag."
#endif
}

void ISetLoaderFlag(uint32 flag)
{
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	writel(flag, RKIO_PMU_PHYS + PMU_SYS_REG0);
#else
	#error "PLS check CONFIG_RKCHIPTYPE for loader flag."
#endif
}

void FW_NandDeInit(void)
{
#ifdef CONFIG_NAND//RK_FLASH_BOOT_EN
	if(gpMemFun->flag == BOOT_FROM_FLASH) {
		FtlDeInit();
		FlashDeInit();
	}
#endif
}


#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
static void rk3288_uart2usb(uint32 en)
{
	if (en) {
		grf_writel((0x0000 | (0x00C0 << 16)), GRF_UOC0_CON3); // usbphy0 bypass disable and otg enable.

		/* if define force enable usb to uart, maybe usb function will be affected */
#ifdef CONFIG_RKUART2USB_FORCE
		grf_writel((0x0004 | (0x0004 << 16)), GRF_UOC0_CON2); // software control usb phy enable
		grf_writel((0x002A | (0x003F << 16)), GRF_UOC0_CON3); // usb phy enter suspend
		grf_writel((0x00C0 | (0x00C0 << 16)), GRF_UOC0_CON3); // usb uart enable.
#else
		con = grf_readl(GRF_SOC_STATUS2);
		if (!(con & (1<<14)) && (con & (1<<17))) { // check IO domain voltage select.
			grf_writel((0x0004 | (0x0004 << 16)), GRF_UOC0_CON2); // software control usb phy enable
			grf_writel((0x002A | (0x003F << 16)), GRF_UOC0_CON3); // usb phy enter suspend
			grf_writel((0x00C0 | (0x00C0 << 16)), GRF_UOC0_CON3); // uart enable
		}
#endif /* CONFIG_RKUART2USB_FORCE */
	} else {
		grf_writel((0x0000 | (0x00C0 << 16)), GRF_UOC0_CON3); // usb uart disable
		grf_writel((0x0000 | (0x0004 << 16)), GRF_UOC0_CON2); // software control usb phy disable
	}
}
#endif


void rkplat_uart2UsbEn(uint32 en)
{
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	rk3288_uart2usb(en);
#else
	#error "PLS check CONFIG_RKCHIPTYPE if support uart2usb."
#endif /* CONFIG_RKPLATFORM */
}

