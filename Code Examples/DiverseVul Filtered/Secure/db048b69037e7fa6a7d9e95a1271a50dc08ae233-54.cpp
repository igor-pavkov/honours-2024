static void __rx_ntuple_filter_add(struct ethtool_rx_ntuple_list *list,
			struct ethtool_rx_ntuple_flow_spec *spec,
			struct ethtool_rx_ntuple_flow_spec_container *fsc)
{

	/* don't add filters forever */
	if (list->count >= ETHTOOL_MAX_NTUPLE_LIST_ENTRY) {
		/* free the container */
		kfree(fsc);
		return;
	}

	/* Copy the whole filter over */
	fsc->fs.flow_type = spec->flow_type;
	memcpy(&fsc->fs.h_u, &spec->h_u, sizeof(spec->h_u));
	memcpy(&fsc->fs.m_u, &spec->m_u, sizeof(spec->m_u));

	fsc->fs.vlan_tag = spec->vlan_tag;
	fsc->fs.vlan_tag_mask = spec->vlan_tag_mask;
	fsc->fs.data = spec->data;
	fsc->fs.data_mask = spec->data_mask;
	fsc->fs.action = spec->action;

	/* add to the list */
	list_add_tail_rcu(&fsc->list, &list->list);
	list->count++;
}