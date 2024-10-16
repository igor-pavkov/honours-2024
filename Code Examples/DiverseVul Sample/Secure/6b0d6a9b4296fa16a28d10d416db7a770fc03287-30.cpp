int br_multicast_toggle(struct net_bridge *br, unsigned long val)
{
	struct net_bridge_port *port;
	int err = 0;
	struct net_bridge_mdb_htable *mdb;

	spin_lock(&br->multicast_lock);
	if (br->multicast_disabled == !val)
		goto unlock;

	br->multicast_disabled = !val;
	if (br->multicast_disabled)
		goto unlock;

	if (!netif_running(br->dev))
		goto unlock;

	mdb = mlock_dereference(br->mdb, br);
	if (mdb) {
		if (mdb->old) {
			err = -EEXIST;
rollback:
			br->multicast_disabled = !!val;
			goto unlock;
		}

		err = br_mdb_rehash(&br->mdb, mdb->max,
				    br->hash_elasticity);
		if (err)
			goto rollback;
	}

	br_multicast_open(br);
	list_for_each_entry(port, &br->port_list, list) {
		if (port->state == BR_STATE_DISABLED ||
		    port->state == BR_STATE_BLOCKING)
			continue;

		__br_multicast_enable_port(port);
	}

unlock:
	spin_unlock(&br->multicast_lock);

	return err;
}