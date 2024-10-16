static int iboe_tos_to_sl(struct net_device *ndev, int tos)
{
	struct iboe_prio_tc_map prio_tc_map = {};
	int prio = rt_tos2priority(tos);
	struct netdev_nested_priv priv;

	/* If VLAN device, get it directly from the VLAN netdev */
	if (is_vlan_dev(ndev))
		return get_vlan_ndev_tc(ndev, prio);

	prio_tc_map.input_prio = prio;
	priv.data = (void *)&prio_tc_map;
	rcu_read_lock();
	netdev_walk_all_lower_dev_rcu(ndev,
				      get_lower_vlan_dev_tc,
				      &priv);
	rcu_read_unlock();
	/* If map is found from lower device, use it; Otherwise
	 * continue with the current netdevice to get priority to tc map.
	 */
	if (prio_tc_map.found)
		return prio_tc_map.output_tc;
	else if (ndev->num_tc)
		return netdev_get_prio_tc_map(ndev, prio);
	else
		return 0;
}