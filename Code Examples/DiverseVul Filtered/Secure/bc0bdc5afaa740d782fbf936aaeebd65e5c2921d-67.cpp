static int get_vlan_ndev_tc(struct net_device *vlan_ndev, int prio)
{
	struct net_device *dev;

	dev = vlan_dev_real_dev(vlan_ndev);
	if (dev->num_tc)
		return netdev_get_prio_tc_map(dev, prio);

	return (vlan_dev_get_egress_qos_mask(vlan_ndev, prio) &
		VLAN_PRIO_MASK) >> VLAN_PRIO_SHIFT;
}