static void __exit af_ieee802154_remove(void)
{
	dev_remove_pack(&ieee802154_packet_type);
	sock_unregister(PF_IEEE802154);
	proto_unregister(&ieee802154_dgram_prot);
	proto_unregister(&ieee802154_raw_prot);
}