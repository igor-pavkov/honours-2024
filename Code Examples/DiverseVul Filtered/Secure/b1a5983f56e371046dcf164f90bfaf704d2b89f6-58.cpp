static int nft_flow_offload_unbind(struct flow_block_offload *bo,
				   struct nft_base_chain *basechain)
{
	struct flow_block_cb *block_cb, *next;
	struct flow_cls_offload cls_flow;
	struct netlink_ext_ack extack;
	struct nft_chain *chain;
	struct nft_rule *rule;

	chain = &basechain->chain;
	list_for_each_entry(rule, &chain->rules, list) {
		memset(&extack, 0, sizeof(extack));
		nft_flow_cls_offload_setup(&cls_flow, basechain, rule, NULL,
					   &extack, FLOW_CLS_DESTROY);
		nft_setup_cb_call(TC_SETUP_CLSFLOWER, &cls_flow, &bo->cb_list);
	}

	list_for_each_entry_safe(block_cb, next, &bo->cb_list, list) {
		list_del(&block_cb->list);
		flow_block_cb_free(block_cb);
	}

	return 0;
}