struct rtc_device *alarmtimer_get_rtcdev(void)
{
	unsigned long flags;
	struct rtc_device *ret;

	spin_lock_irqsave(&rtcdev_lock, flags);
	ret = rtcdev;
	spin_unlock_irqrestore(&rtcdev_lock, flags);

	return ret;
}