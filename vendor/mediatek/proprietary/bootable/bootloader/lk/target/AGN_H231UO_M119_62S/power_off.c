//#include <common.h>
//#include <asm/arch/mt65xx.h>
#include <debug.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_rtc.h>
#include <mtk_wdt.h>
#include <platform/timer.h>

extern kal_bool pmic_chrdet_status(void);

#ifndef NO_POWER_OFF
void mt6575_power_off(void)
{
	int count = 0;
	dprintf(CRITICAL, "mt_power_off\n");

	/* pull PWRBB low */
	rtc_bbpu_power_down();

	while (1) {
		mdelay(100);
		dprintf(CRITICAL, "mt_power_off : check charger\n");
		if (pmic_chrdet_status() == KAL_TRUE || count > 10) {
			mtk_arch_reset(0);
			break;
		}
		count++;
	}
}
#endif
