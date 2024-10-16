static int __init pep_register(void)
{
	return phonet_proto_register(PN_PROTO_PIPE, &pep_pn_proto);
}