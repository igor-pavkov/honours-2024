static void atl2_vlan_mode(struct net_device *netdev,
	netdev_features_t features)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	u32 ctrl;

	atl2_irq_disable(adapter);

	ctrl = ATL2_READ_REG(&adapter->hw, REG_MAC_CTRL);
	__atl2_vlan_mode(features, &ctrl);
	ATL2_WRITE_REG(&adapter->hw, REG_MAC_CTRL, ctrl);

	atl2_irq_enable(adapter);
}