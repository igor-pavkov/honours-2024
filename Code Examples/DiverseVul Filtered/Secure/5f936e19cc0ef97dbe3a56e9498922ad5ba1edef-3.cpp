static int alarmtimer_resume(struct device *dev)
{
	struct rtc_device *rtc;

	rtc = alarmtimer_get_rtcdev();
	if (rtc)
		rtc_timer_cancel(rtc, &rtctimer);
	return 0;
}