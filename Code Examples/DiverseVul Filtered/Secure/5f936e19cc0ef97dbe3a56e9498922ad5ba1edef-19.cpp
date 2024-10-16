static void alarmtimer_rtc_interface_remove(void)
{
	class_interface_unregister(&alarmtimer_rtc_interface);
}