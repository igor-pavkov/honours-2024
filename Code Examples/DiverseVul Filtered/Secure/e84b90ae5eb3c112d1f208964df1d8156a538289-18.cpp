static __init int raw_module_init(void)
{
	int err;

	printk(banner);

	err = can_proto_register(&raw_can_proto);
	if (err < 0)
		printk(KERN_ERR "can: registration of raw protocol failed\n");

	return err;
}