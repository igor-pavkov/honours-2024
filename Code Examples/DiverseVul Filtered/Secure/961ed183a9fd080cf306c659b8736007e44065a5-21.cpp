static int __init clusterip_tg_init(void)
{
	int ret;

	ret = xt_register_target(&clusterip_tg_reg);
	if (ret < 0)
		return ret;

	ret = nf_register_hook(&cip_arp_ops);
	if (ret < 0)
		goto cleanup_target;

#ifdef CONFIG_PROC_FS
	clusterip_procdir = proc_mkdir("ipt_CLUSTERIP", init_net.proc_net);
	if (!clusterip_procdir) {
		pr_err("Unable to proc dir entry\n");
		ret = -ENOMEM;
		goto cleanup_hook;
	}
#endif /* CONFIG_PROC_FS */

	pr_info("ClusterIP Version %s loaded successfully\n",
		CLUSTERIP_VERSION);
	return 0;

#ifdef CONFIG_PROC_FS
cleanup_hook:
	nf_unregister_hook(&cip_arp_ops);
#endif /* CONFIG_PROC_FS */
cleanup_target:
	xt_unregister_target(&clusterip_tg_reg);
	return ret;
}