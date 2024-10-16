static void bnx2x_set_iscsi_eth_rx_mode(struct bnx2x *bp, bool start)
{
	unsigned long accept_flags = 0, ramrod_flags = 0;
	u8 cl_id = bnx2x_cnic_eth_cl_id(bp, BNX2X_ISCSI_ETH_CL_ID_IDX);
	int sched_state = BNX2X_FILTER_ISCSI_ETH_STOP_SCHED;

	if (start) {
		/* Start accepting on iSCSI L2 ring. Accept all multicasts
		 * because it's the only way for UIO Queue to accept
		 * multicasts (in non-promiscuous mode only one Queue per
		 * function will receive multicast packets (leading in our
		 * case).
		 */
		__set_bit(BNX2X_ACCEPT_UNICAST, &accept_flags);
		__set_bit(BNX2X_ACCEPT_ALL_MULTICAST, &accept_flags);
		__set_bit(BNX2X_ACCEPT_BROADCAST, &accept_flags);
		__set_bit(BNX2X_ACCEPT_ANY_VLAN, &accept_flags);

		/* Clear STOP_PENDING bit if START is requested */
		clear_bit(BNX2X_FILTER_ISCSI_ETH_STOP_SCHED, &bp->sp_state);

		sched_state = BNX2X_FILTER_ISCSI_ETH_START_SCHED;
	} else
		/* Clear START_PENDING bit if STOP is requested */
		clear_bit(BNX2X_FILTER_ISCSI_ETH_START_SCHED, &bp->sp_state);

	if (test_bit(BNX2X_FILTER_RX_MODE_PENDING, &bp->sp_state))
		set_bit(sched_state, &bp->sp_state);
	else {
		__set_bit(RAMROD_RX, &ramrod_flags);
		bnx2x_set_q_rx_mode(bp, cl_id, 0, accept_flags, 0,
				    ramrod_flags);
	}
}