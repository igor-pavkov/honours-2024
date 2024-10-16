void irsock_cleanup(void)
{
	sock_unregister(PF_IRDA);
	proto_unregister(&irda_proto);
}