static void __exit clusterip_tg_exit(void)
{
	pr_info("ClusterIP Version %s unloading\n", CLUSTERIP_VERSION);
#ifdef CONFIG_PROC_FS
	remove_proc_entry(clusterip_procdir->name, clusterip_procdir->parent);
#endif
	nf_unregister_hook(&cip_arp_ops);
	xt_unregister_target(&clusterip_tg_reg);

	/* Wait for completion of call_rcu_bh()'s (clusterip_config_rcu_free) */
	rcu_barrier_bh();
}