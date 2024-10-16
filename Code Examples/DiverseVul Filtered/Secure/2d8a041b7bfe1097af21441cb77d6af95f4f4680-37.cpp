void ip_vs_control_cleanup(void)
{
	EnterFunction(2);
	unregister_netdevice_notifier(&ip_vs_dst_notifier);
	LeaveFunction(2);
}