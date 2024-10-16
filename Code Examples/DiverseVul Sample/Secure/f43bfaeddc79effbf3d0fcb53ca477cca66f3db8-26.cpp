static void atl2_watchdog(unsigned long data)
{
	struct atl2_adapter *adapter = (struct atl2_adapter *) data;

	if (!test_bit(__ATL2_DOWN, &adapter->flags)) {
		u32 drop_rxd, drop_rxs;
		unsigned long flags;

		spin_lock_irqsave(&adapter->stats_lock, flags);
		drop_rxd = ATL2_READ_REG(&adapter->hw, REG_STS_RXD_OV);
		drop_rxs = ATL2_READ_REG(&adapter->hw, REG_STS_RXS_OV);
		spin_unlock_irqrestore(&adapter->stats_lock, flags);

		adapter->netdev->stats.rx_over_errors += drop_rxd + drop_rxs;

		/* Reset the timer */
		mod_timer(&adapter->watchdog_timer,
			  round_jiffies(jiffies + 4 * HZ));
	}
}