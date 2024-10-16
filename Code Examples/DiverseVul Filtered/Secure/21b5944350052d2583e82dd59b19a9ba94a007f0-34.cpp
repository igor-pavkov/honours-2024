static __init int net_defaults_init(void)
{
	if (register_pernet_subsys(&net_defaults_ops))
		panic("Cannot initialize net default settings");

	return 0;
}