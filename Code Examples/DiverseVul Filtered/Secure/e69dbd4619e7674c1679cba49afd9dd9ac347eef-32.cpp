static int __init af_ieee802154_init(void)
{
	int rc;

	rc = proto_register(&ieee802154_raw_prot, 1);
	if (rc)
		goto out;

	rc = proto_register(&ieee802154_dgram_prot, 1);
	if (rc)
		goto err_dgram;

	/* Tell SOCKET that we are alive */
	rc = sock_register(&ieee802154_family_ops);
	if (rc)
		goto err_sock;
	dev_add_pack(&ieee802154_packet_type);

	rc = 0;
	goto out;

err_sock:
	proto_unregister(&ieee802154_dgram_prot);
err_dgram:
	proto_unregister(&ieee802154_raw_prot);
out:
	return rc;
}