void __init xfrm_init(void)
{
	flow_cache_hp_init();
	register_pernet_subsys(&xfrm_net_ops);
	seqcount_init(&xfrm_policy_hash_generation);
	xfrm_input_init();
}