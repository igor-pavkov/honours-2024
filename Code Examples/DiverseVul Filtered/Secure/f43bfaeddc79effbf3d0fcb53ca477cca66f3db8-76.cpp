static void atl2_restore_vlan(struct atl2_adapter *adapter)
{
	atl2_vlan_mode(adapter->netdev, adapter->netdev->features);
}