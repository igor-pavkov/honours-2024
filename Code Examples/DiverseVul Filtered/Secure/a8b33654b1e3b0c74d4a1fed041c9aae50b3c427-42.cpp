static void __exit multi_exit(void)
{
	int i;

	for (i = 0; i < NR_PORTS; i++)
		mp_remove_one_port(&multi_reg, &multi_ports[i].port);

	mp_unregister_driver(&multi_reg);
}