static void ppp_stop(struct net_device *dev)
{
	ppp_cp_event(dev, PID_LCP, STOP, 0, 0, 0, NULL);
}