static void nft_flow_rule_transfer_vlan(struct nft_offload_ctx *ctx,
					struct nft_flow_rule *flow)
{
	struct nft_flow_match *match = &flow->match;
	struct nft_offload_ethertype ethertype = {
		.value	= match->key.basic.n_proto,
		.mask	= match->mask.basic.n_proto,
	};

	if (match->dissector.used_keys & BIT(FLOW_DISSECTOR_KEY_VLAN) &&
	    (match->key.vlan.vlan_tpid == htons(ETH_P_8021Q) ||
	     match->key.vlan.vlan_tpid == htons(ETH_P_8021AD))) {
		match->key.basic.n_proto = match->key.cvlan.vlan_tpid;
		match->mask.basic.n_proto = match->mask.cvlan.vlan_tpid;
		match->key.cvlan.vlan_tpid = match->key.vlan.vlan_tpid;
		match->mask.cvlan.vlan_tpid = match->mask.vlan.vlan_tpid;
		match->key.vlan.vlan_tpid = ethertype.value;
		match->mask.vlan.vlan_tpid = ethertype.mask;
		match->dissector.offset[FLOW_DISSECTOR_KEY_CVLAN] =
			offsetof(struct nft_flow_key, cvlan);
		match->dissector.used_keys |= BIT(FLOW_DISSECTOR_KEY_CVLAN);
	} else if (match->dissector.used_keys & BIT(FLOW_DISSECTOR_KEY_BASIC) &&
		   (match->key.basic.n_proto == htons(ETH_P_8021Q) ||
		    match->key.basic.n_proto == htons(ETH_P_8021AD))) {
		match->key.basic.n_proto = match->key.vlan.vlan_tpid;
		match->mask.basic.n_proto = match->mask.vlan.vlan_tpid;
		match->key.vlan.vlan_tpid = ethertype.value;
		match->mask.vlan.vlan_tpid = ethertype.mask;
		match->dissector.offset[FLOW_DISSECTOR_KEY_VLAN] =
			offsetof(struct nft_flow_key, vlan);
		match->dissector.used_keys |= BIT(FLOW_DISSECTOR_KEY_VLAN);
	}
}