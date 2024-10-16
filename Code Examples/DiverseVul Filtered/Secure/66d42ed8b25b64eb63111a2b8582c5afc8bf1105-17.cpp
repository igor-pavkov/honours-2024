static void ppp_close(struct net_device *dev)
{
	ppp_tx_flush();
}