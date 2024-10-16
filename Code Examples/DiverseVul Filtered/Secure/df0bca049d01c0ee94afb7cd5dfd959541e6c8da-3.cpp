static __init int net_inuse_init(void)
{
	if (register_pernet_subsys(&net_inuse_ops))
		panic("Cannot initialize net inuse counters");

	return 0;
}