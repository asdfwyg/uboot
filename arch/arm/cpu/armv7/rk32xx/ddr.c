/*
 * (C) Copyright 2008-2014 Rockchip Electronics
 * hcy, Software Engineering.
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
#include <asm/arch/rkplat.h>
#include <asm/byteorder.h>

DECLARE_GLOBAL_DATA_PTR;

//��ȡ�����������ֽ���
uint32 ddr_get_cap(void)
{
#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	uint32 reg = *(volatile uint32*)(RKIO_PMU_PHYS + 0x9c);
	uint32 cap[2] = {0, 0};

	if((reg>>28)&1)
	{
		cap[0] = (1 << ((13+((reg>>6)&0x3))+(9+((reg>>9)&0x3))+(3-((reg>>8)&0x1))+(2>>((reg>>2)&0x3))));
		if((1+((reg>>11)&0x1)) > 1)
		{
			cap[0] += cap[0] >> (((reg>>6)&0x3)-((reg>>4)&0x3));
		}
		if(((reg>>30)&0x1))
		{
			cap[0] = cap[0]*3/4;
		}
	}
	if((reg>>29)&1)
	{
		cap[1] = (1 << ((13+((reg>>22)&0x3))+(9+((reg>>25)&0x3))+(3-((reg>>24)&0x1))+(2>>((reg>>18)&0x3))));
		if((1+((reg>>27)&0x1)) > 1)
		{
			cap[1] += cap[1] >> (((reg>>22)&0x3)-((reg>>20)&0x3));
		}
		if(((reg>>31)&0x1))
		{
			cap[1] = cap[1]*3/4;
		}
	}

	return (cap[0]+cap[1]);
#else
	#error "PLS config chiptype for ddr cap get!"
#endif
}


/**********************************************
 * Routine: dram_init
 * Description: sets uboots idea of sdram size
 **********************************************/
int dram_init(void)
{
	gd->ram_size = get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_SYS_SDRAM_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
#if defined CONFIG_RKDDR_PARAM_ADDR && !defined CONFIG_SYS_GENERIC_BOARD
	u64* buf = (u64*)CONFIG_RKDDR_PARAM_ADDR;
	u32 count = ((u32*)buf)[0];
	buf ++;
	printf("Found dram banks:%d\n", count);
	gd->bd->rk_dram[0].start = gd->bd->rk_dram[0].size = 0;
	if (count >= CONFIG_RK_MAX_DRAM_BANKS){
		printf("Wrong bank count: %d(%d)\n",
				count, CONFIG_RK_MAX_DRAM_BANKS);
		goto failed;
	}
	int i;
	for (i = 0; i < count; i++) {
		gd->bd->rk_dram[i].start = le64_to_cpu(buf[i]);
		gd->bd->rk_dram[i].size = le64_to_cpu(buf[count + i]);
		//TODO: add check, if start|size not valide, goto failed.
		/*
		if (check) {
			gd->bd->rk_dram[0].start = gd->bd->rk_dram[0].size = 0;
			goto failed;
		}*/
		printf("Adding bank:%016llx(%016llx)\n",
				gd->bd->rk_dram[i].start,
				gd->bd->rk_dram[i].size);
		gd->bd->rk_dram[i+1].start = gd->bd->rk_dram[i+1].size = 0;
	}
failed:
#endif

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

