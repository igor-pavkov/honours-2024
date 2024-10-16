static void __atl2_vlan_mode(netdev_features_t features, u32 *ctrl)
{
	if (features & NETIF_F_HW_VLAN_CTAG_RX) {
		/* enable VLAN tag insert/strip */
		*ctrl |= MAC_CTRL_RMV_VLAN;
	} else {
		/* disable VLAN tag insert/strip */
		*ctrl &= ~MAC_CTRL_RMV_VLAN;
	}
}