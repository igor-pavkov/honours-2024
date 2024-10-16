static int alarmtimer_rtc_add_device(struct device *dev,
				struct class_interface *class_intf)
{
	unsigned long flags;
	struct rtc_device *rtc = to_rtc_device(dev);
	struct wakeup_source *__ws;

	if (rtcdev)
		return -EBUSY;

	if (!rtc->ops->set_alarm)
		return -1;
	if (!device_may_wakeup(rtc->dev.parent))
		return -1;

	__ws = wakeup_source_register("alarmtimer");

	spin_lock_irqsave(&rtcdev_lock, flags);
	if (!rtcdev) {
		if (!try_module_get(rtc->owner)) {
			spin_unlock_irqrestore(&rtcdev_lock, flags);
			return -1;
		}

		rtcdev = rtc;
		/* hold a reference so it doesn't go away */
		get_device(dev);
		ws = __ws;
		__ws = NULL;
	}
	spin_unlock_irqrestore(&rtcdev_lock, flags);

	wakeup_source_unregister(__ws);

	return 0;
}