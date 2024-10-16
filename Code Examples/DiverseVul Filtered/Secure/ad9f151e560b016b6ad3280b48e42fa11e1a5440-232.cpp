static void nft_unregister_flowtable_hook(struct net *net,
					  struct nft_flowtable *flowtable,
					  struct nft_hook *hook)
{
	nf_unregister_net_hook(net, &hook->ops);
	flowtable->data.type->setup(&flowtable->data, hook->ops.dev,
				    FLOW_BLOCK_UNBIND);
}