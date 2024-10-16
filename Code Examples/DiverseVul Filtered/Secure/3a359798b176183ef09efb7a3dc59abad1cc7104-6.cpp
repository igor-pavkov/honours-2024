void nfc_llcp_sock_exit(void)
{
	nfc_proto_unregister(&llcp_nfc_proto);
}