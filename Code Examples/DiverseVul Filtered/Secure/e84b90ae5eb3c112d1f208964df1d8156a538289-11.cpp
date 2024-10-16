static __exit void raw_module_exit(void)
{
	can_proto_unregister(&raw_can_proto);
}