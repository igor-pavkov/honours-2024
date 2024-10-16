static void setup_struct_phy_for_init(struct b43_wldev *dev,
				      struct b43_phy *phy)
{
	phy->hardware_power_control = !!modparam_hwpctl;
	phy->next_txpwr_check_time = jiffies;
	/* PHY TX errors counter. */
	atomic_set(&phy->txerr_cnt, B43_PHY_TX_BADNESS_LIMIT);

#if B43_DEBUG
	phy->phy_locked = false;
	phy->radio_locked = false;
#endif
}