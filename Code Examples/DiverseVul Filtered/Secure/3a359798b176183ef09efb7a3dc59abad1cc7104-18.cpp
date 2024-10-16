int __init nfc_llcp_sock_init(void)
{
	return nfc_proto_register(&llcp_nfc_proto);
}