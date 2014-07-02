/*pmic auto compatible driver on rockchip platform
 * Copyright (C) 2014 RockChip inc
 * Andy <yxj@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <power/rockchip_power.h>
#include <power/battery.h>

static unsigned char rockchip_pmic_id;

static const char * const fg_names[] = {
	"CW201X_FG",
	"RICOH619_FG",
};


static void set_rockchip_pmic_id(unsigned char id)
{
	rockchip_pmic_id = id;
}

unsigned char get_rockchip_pmic_id(void)
{
	return rockchip_pmic_id;
}


int get_power_bat_status(struct battery *battery)
{
	int i;
	struct pmic *p_fg = NULL;
	for (i = 0; i < ARRAY_SIZE(fg_names); i++) {
		
		p_fg = pmic_get(fg_names[i]);
		if (p_fg)
			break;
	}

	if (p_fg) {
		p_fg->pbat->bat = battery;
		if (p_fg->fg->fg_battery_update)
			p_fg->fg->fg_battery_update(p_fg, p_fg);
	} else {
		return -ENODEV;
	}

	return 0;
}


/*
return 0: no charger
return 1: charging
*/
int is_charging(void)
{
	int ret;
	struct battery battery;
	memset(&battery,0, sizeof(battery));
	ret = get_power_bat_status(&battery);
	if (ret < 0)
		return 0;
	return battery.state_of_chrg;
}

int pmic_charger_setting(int current)
{
	enum pmic_id  id = get_rockchip_pmic_id();
	switch (id) {
		case PMIC_ID_ACT8846:
			pmic_act8846_charger_setting(current);
			break;
		case PMIC_ID_RK808:
			pmic_rk808_charger_setting(current);
			break;
		default:
			break;
	}
	return 0;
}


/*system on thresd*/
int is_power_low(void)
{
	int ret;
	struct battery battery;
	memset(&battery,0, sizeof(battery));
	ret = get_power_bat_status(&battery);
	if (ret < 0)
		return 0;
	return (battery.voltage_uV < CONFIG_SYSTEM_ON_VOL_THRESD) ? 1:0;	
}


/*screen on thresd*/
int is_power_extreme_low(void)
{
	int ret;
	struct battery battery;
	memset(&battery,0, sizeof(battery));
	ret = get_power_bat_status(&battery);
	if (ret < 0)
		return 0;
	return (battery.voltage_uV < CONFIG_SCREEN_ON_VOL_THRESD) ? 1:0;
}


int pmic_init(unsigned char  bus)
{
	int ret;
#if defined(CONFIG_POWER_RICOH619)
	ret = pmic_ricoh619_init(bus);
	if (ret >= 0) {
		set_rockchip_pmic_id(PMIC_ID_RICOH619);
		printf("pmic:ricoh619\n");
		return 0;
	}
#endif

#if defined(CONFIG_POWER_ACT8846)
	ret = pmic_act8846_init (bus);
	if (ret >= 0) {
		set_rockchip_pmic_id(PMIC_ID_ACT8846);
		printf("pmic:act8846\n");
		return 0;
	}
#endif

#if defined(CONFIG_POWER_RK808)
	ret = pmic_rk808_init (bus);
	if (ret >= 0) {
		set_rockchip_pmic_id(PMIC_ID_RK808);
		printf("pmic:rk808\n");
		return 0;
	}
#endif
	return ret;
}


int fg_init(unsigned char bus)
{
	int ret;
#if defined(CONFIG_POWER_FG_CW201X)
	ret = fg_cw201x_init(bus);
	if(ret >= 0) {
		printf("fg:cw201x\n");
		return 0;
	}
#endif
	return 0;
}

void shut_down(void)
{
	enum pmic_id  id = get_rockchip_pmic_id();
	switch (id) {
		case PMIC_ID_ACT8846:
			pmic_act8846_shut_down();
			break;
		case PMIC_ID_RICOH619:
			pmic_ricoh619_shut_down();
			break;
		case PMIC_ID_RK808:
			pmic_rk808_shut_down();
			break;
		default:
			break;
	}
}

