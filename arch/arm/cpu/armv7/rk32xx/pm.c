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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/rkplat.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_PM_SUBSYSTEM

#define RKPM_VERSION		"1.0"

#if (CONFIG_RKCHIPTYPE == CONFIG_RK3288)
	#define RK_WAKEUP_KEY_PIN	(GPIO_BANK0 | GPIO_A5)
#else
	#error	"PLS config wake up key for chip!"
#endif


/*
 * rkpm wakeup gpio init
 */
void rk_pm_wakeup_gpio_init(void)
{
	int irq = gpio_to_irq(RK_WAKEUP_KEY_PIN);

	/* gpio pin just use to wakeup, no need isr handle */
	irq_install_handler(irq, -1, NULL);
	irq_set_irq_type(irq, IRQ_TYPE_LEVEL_LOW);
	irq_handler_enable(irq);
}


/*
 * rkpm wakeup gpio deinit
 */
void rk_pm_wakeup_gpio_deinit(void)
{
	int irq = gpio_to_irq(RK_WAKEUP_KEY_PIN);

	irq_handler_disable(irq);
	irq_uninstall_handler(irq);
}


/*
 * rkpm enter
 * module_pm_conf: callback function that control such as backlight/lcd on or off
 */
void rk_pm_enter(v_pm_cb_f module_pm_conf)
{
	/* disable exceptions */
	disable_interrupts();

	if (module_pm_conf != NULL) {
		module_pm_conf(0);
	}

	/* pll enter slow mode */
	rkclk_pll_mode(CPLL_ID, RKCLK_PLL_MODE_SLOW);
	rkclk_pll_mode(GPLL_ID, RKCLK_PLL_MODE_SLOW);
	rkclk_pll_mode(APLL_ID, RKCLK_PLL_MODE_SLOW);

	/* cpu enter wfi mode */
	wfi();

	/* pll enter nornal mode */
	rkclk_pll_mode(APLL_ID, RKCLK_PLL_MODE_NORMAL);
	rkclk_pll_mode(GPLL_ID, RKCLK_PLL_MODE_NORMAL);
	rkclk_pll_mode(CPLL_ID, RKCLK_PLL_MODE_NORMAL);


	if (module_pm_conf != NULL) {
		module_pm_conf(1);
	}

	/* enable exceptions */
	enable_interrupts();
}

#else

void rk_pm_wakeup_gpio_init(void) {}
void rk_pm_enter(v_pm_cb_f module_pm_conf) {}

#endif /* CONFIG_PM_SUBSYSTEM */

