static int __init alarmtimer_init(void)
{
	struct platform_device *pdev;
	int error = 0;
	int i;

	alarmtimer_rtc_timer_init();

	/* Initialize alarm bases */
	alarm_bases[ALARM_REALTIME].base_clockid = CLOCK_REALTIME;
	alarm_bases[ALARM_REALTIME].gettime = &ktime_get_real;
	alarm_bases[ALARM_BOOTTIME].base_clockid = CLOCK_BOOTTIME;
	alarm_bases[ALARM_BOOTTIME].gettime = &ktime_get_boottime;
	for (i = 0; i < ALARM_NUMTYPE; i++) {
		timerqueue_init_head(&alarm_bases[i].timerqueue);
		spin_lock_init(&alarm_bases[i].lock);
	}

	error = alarmtimer_rtc_interface_setup();
	if (error)
		return error;

	error = platform_driver_register(&alarmtimer_driver);
	if (error)
		goto out_if;

	pdev = platform_device_register_simple("alarmtimer", -1, NULL, 0);
	if (IS_ERR(pdev)) {
		error = PTR_ERR(pdev);
		goto out_drv;
	}
	return 0;

out_drv:
	platform_driver_unregister(&alarmtimer_driver);
out_if:
	alarmtimer_rtc_interface_remove();
	return error;
}