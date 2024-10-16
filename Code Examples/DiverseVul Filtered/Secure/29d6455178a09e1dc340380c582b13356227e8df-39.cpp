static __init void init_hrtick(void)
{
	hotcpu_notifier(hotplug_hrtick, 0);
}