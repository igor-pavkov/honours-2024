static int nft_setup_cb_call(enum tc_setup_type type, void *type_data,
			     struct list_head *cb_list)
{
	struct flow_block_cb *block_cb;
	int err;

	list_for_each_entry(block_cb, cb_list, list) {
		err = block_cb->cb(type, type_data, block_cb->cb_priv);
		if (err < 0)
			return err;
	}
	return 0;
}