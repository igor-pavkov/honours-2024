static void __exit qrtr_proto_fini(void)
{
	qrtr_ns_remove();
	sock_unregister(qrtr_family.family);
	proto_unregister(&qrtr_proto);
}