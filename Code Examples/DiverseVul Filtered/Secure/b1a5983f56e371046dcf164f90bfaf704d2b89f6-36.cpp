void nft_flow_rule_destroy(struct nft_flow_rule *flow)
{
	struct flow_action_entry *entry;
	int i;

	flow_action_for_each(i, entry, &flow->rule->action) {
		switch (entry->id) {
		case FLOW_ACTION_REDIRECT:
		case FLOW_ACTION_MIRRED:
			dev_put(entry->dev);
			break;
		default:
			break;
		}
	}
	kfree(flow->rule);
	kfree(flow);
}