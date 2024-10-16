void nft_flow_rule_set_addr_type(struct nft_flow_rule *flow,
				 enum flow_dissector_key_id addr_type)
{
	struct nft_flow_match *match = &flow->match;
	struct nft_flow_key *mask = &match->mask;
	struct nft_flow_key *key = &match->key;

	if (match->dissector.used_keys & BIT(FLOW_DISSECTOR_KEY_CONTROL))
		return;

	key->control.addr_type = addr_type;
	mask->control.addr_type = 0xffff;
	match->dissector.used_keys |= BIT(FLOW_DISSECTOR_KEY_CONTROL);
	match->dissector.offset[FLOW_DISSECTOR_KEY_CONTROL] =
		offsetof(struct nft_flow_key, control);
}