static inline void alarmtimer_rtc_timer_init(void)
{
	rtc_timer_init(&rtctimer, NULL, NULL);
}