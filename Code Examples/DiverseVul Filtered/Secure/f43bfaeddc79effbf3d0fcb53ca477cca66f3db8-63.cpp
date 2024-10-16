static void atl2_reinit_locked(struct atl2_adapter *adapter)
{
	WARN_ON(in_interrupt());
	while (test_and_set_bit(__ATL2_RESETTING, &adapter->flags))
		msleep(1);
	atl2_down(adapter);
	atl2_up(adapter);
	clear_bit(__ATL2_RESETTING, &adapter->flags);
}