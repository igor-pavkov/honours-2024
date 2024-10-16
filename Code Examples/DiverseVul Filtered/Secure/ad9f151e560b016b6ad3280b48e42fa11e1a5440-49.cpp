static int nf_tables_dump_set_start(struct netlink_callback *cb)
{
	struct nft_set_dump_ctx *dump_ctx = cb->data;

	cb->data = kmemdup(dump_ctx, sizeof(*dump_ctx), GFP_ATOMIC);

	return cb->data ? 0 : -ENOMEM;
}