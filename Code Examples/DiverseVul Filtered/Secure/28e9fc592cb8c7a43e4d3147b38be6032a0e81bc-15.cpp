static void __exit llc2_exit(void)
{
	llc_station_exit();
	llc_remove_pack(LLC_DEST_SAP);
	llc_remove_pack(LLC_DEST_CONN);
	sock_unregister(PF_LLC);
	llc_proc_exit();
	llc_sysctl_exit();
	proto_unregister(&llc_proto);
}