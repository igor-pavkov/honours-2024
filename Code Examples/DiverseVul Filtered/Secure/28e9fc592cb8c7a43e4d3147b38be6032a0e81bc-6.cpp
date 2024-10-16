static int __init llc2_init(void)
{
	int rc = proto_register(&llc_proto, 0);

	if (rc != 0)
		goto out;

	llc_build_offset_table();
	llc_station_init();
	llc_ui_sap_last_autoport = LLC_SAP_DYN_START;
	rc = llc_proc_init();
	if (rc != 0) {
		printk(llc_proc_err_msg);
		goto out_unregister_llc_proto;
	}
	rc = llc_sysctl_init();
	if (rc) {
		printk(llc_sysctl_err_msg);
		goto out_proc;
	}
	rc = sock_register(&llc_ui_family_ops);
	if (rc) {
		printk(llc_sock_err_msg);
		goto out_sysctl;
	}
	llc_add_pack(LLC_DEST_SAP, llc_sap_handler);
	llc_add_pack(LLC_DEST_CONN, llc_conn_handler);
out:
	return rc;
out_sysctl:
	llc_sysctl_exit();
out_proc:
	llc_proc_exit();
out_unregister_llc_proto:
	proto_unregister(&llc_proto);
	goto out;
}