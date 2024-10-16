static inline unsigned int rfcomm_room(struct rfcomm_dlc *dlc)
{
	/* We can't let it be zero, because we don't get a callback
	   when tx_credits becomes nonzero, hence we'd never wake up */
	return dlc->mtu * (dlc->tx_credits?:1);
}