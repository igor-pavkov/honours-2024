static void tun_detach(struct tun_struct *tun)
{
	rtnl_lock();
	__tun_detach(tun);
	rtnl_unlock();
}