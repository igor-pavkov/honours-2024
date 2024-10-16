static ssize_t bat_socket_write(struct file *file, const char __user *buff,
				size_t len, loff_t *off)
{
	struct socket_client *socket_client = file->private_data;
	struct bat_priv *bat_priv = socket_client->bat_priv;
	struct hard_iface *primary_if = NULL;
	struct sk_buff *skb;
	struct icmp_packet_rr *icmp_packet;

	struct orig_node *orig_node = NULL;
	struct neigh_node *neigh_node = NULL;
	size_t packet_len = sizeof(struct icmp_packet);

	if (len < sizeof(struct icmp_packet)) {
		bat_dbg(DBG_BATMAN, bat_priv,
			"Error - can't send packet from char device: "
			"invalid packet size\n");
		return -EINVAL;
	}

	primary_if = primary_if_get_selected(bat_priv);

	if (!primary_if) {
		len = -EFAULT;
		goto out;
	}

	if (len >= sizeof(struct icmp_packet_rr))
		packet_len = sizeof(struct icmp_packet_rr);

	skb = dev_alloc_skb(packet_len + sizeof(struct ethhdr));
	if (!skb) {
		len = -ENOMEM;
		goto out;
	}

	skb_reserve(skb, sizeof(struct ethhdr));
	icmp_packet = (struct icmp_packet_rr *)skb_put(skb, packet_len);

	if (copy_from_user(icmp_packet, buff, packet_len)) {
		len = -EFAULT;
		goto free_skb;
	}

	if (icmp_packet->packet_type != BAT_ICMP) {
		bat_dbg(DBG_BATMAN, bat_priv,
			"Error - can't send packet from char device: "
			"got bogus packet type (expected: BAT_ICMP)\n");
		len = -EINVAL;
		goto free_skb;
	}

	if (icmp_packet->msg_type != ECHO_REQUEST) {
		bat_dbg(DBG_BATMAN, bat_priv,
			"Error - can't send packet from char device: "
			"got bogus message type (expected: ECHO_REQUEST)\n");
		len = -EINVAL;
		goto free_skb;
	}

	icmp_packet->uid = socket_client->index;

	if (icmp_packet->version != COMPAT_VERSION) {
		icmp_packet->msg_type = PARAMETER_PROBLEM;
		icmp_packet->version = COMPAT_VERSION;
		bat_socket_add_packet(socket_client, icmp_packet, packet_len);
		goto free_skb;
	}

	if (atomic_read(&bat_priv->mesh_state) != MESH_ACTIVE)
		goto dst_unreach;

	orig_node = orig_hash_find(bat_priv, icmp_packet->dst);
	if (!orig_node)
		goto dst_unreach;

	neigh_node = orig_node_get_router(orig_node);
	if (!neigh_node)
		goto dst_unreach;

	if (!neigh_node->if_incoming)
		goto dst_unreach;

	if (neigh_node->if_incoming->if_status != IF_ACTIVE)
		goto dst_unreach;

	memcpy(icmp_packet->orig,
	       primary_if->net_dev->dev_addr, ETH_ALEN);

	if (packet_len == sizeof(struct icmp_packet_rr))
		memcpy(icmp_packet->rr,
		       neigh_node->if_incoming->net_dev->dev_addr, ETH_ALEN);

	send_skb_packet(skb, neigh_node->if_incoming, neigh_node->addr);
	goto out;

dst_unreach:
	icmp_packet->msg_type = DESTINATION_UNREACHABLE;
	bat_socket_add_packet(socket_client, icmp_packet, packet_len);
free_skb:
	kfree_skb(skb);
out:
	if (primary_if)
		hardif_free_ref(primary_if);
	if (neigh_node)
		neigh_node_free_ref(neigh_node);
	if (orig_node)
		orig_node_free_ref(orig_node);
	return len;
}