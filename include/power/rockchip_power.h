#ifndef __ROCKCHIP_PMIC_H_
#define __ROCKCHIP_PMIC_H_
#include <power/pmic.h>

enum pmic_id {
	PMIC_ID_UNKNOW,
	PMIC_ID_RICOH619,
	PMIC_ID_ACT8846,
	PMIC_ID_RK808,
};
unsigned char get_rockchip_pmic_id(void);
int dwc_otg_check_dpdm(void);
int is_charging(void);
int pmic_act8846_init(unsigned char bus);
int pmic_ricoh619_init(unsigned char bus);
int pmic_rk808_init(unsigned char bus);
int pmic_ricoh619_charger_setting(int current);
int pmic_act8846_charger_setting(int current);
int pmic_rk808_charger_setting(int current);
void pmic_ricoh619_shut_down(void);
void pmic_act8846_shut_down(void);
void pmic_rk808_shut_down(void);
int fg_cw201x_init(unsigned char bus);
int fg_ricoh619_init(unsigned char bus,uchar addr);
int ricoh619_poll_pwr_key_sta(void);


#endif
