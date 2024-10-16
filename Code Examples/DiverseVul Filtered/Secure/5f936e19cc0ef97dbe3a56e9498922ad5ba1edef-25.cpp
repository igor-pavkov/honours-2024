static int alarmtimer_rtc_interface_setup(void)
{
	alarmtimer_rtc_interface.class = rtc_class;
	return class_interface_register(&alarmtimer_rtc_interface);
}