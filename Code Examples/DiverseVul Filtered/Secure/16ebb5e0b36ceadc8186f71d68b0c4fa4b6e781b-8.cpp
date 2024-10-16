static int __init pktsched_init(void)
{
	register_qdisc(&pfifo_qdisc_ops);
	register_qdisc(&bfifo_qdisc_ops);
	proc_net_fops_create(&init_net, "psched", 0, &psched_fops);

	rtnl_register(PF_UNSPEC, RTM_NEWQDISC, tc_modify_qdisc, NULL);
	rtnl_register(PF_UNSPEC, RTM_DELQDISC, tc_get_qdisc, NULL);
	rtnl_register(PF_UNSPEC, RTM_GETQDISC, tc_get_qdisc, tc_dump_qdisc);
	rtnl_register(PF_UNSPEC, RTM_NEWTCLASS, tc_ctl_tclass, NULL);
	rtnl_register(PF_UNSPEC, RTM_DELTCLASS, tc_ctl_tclass, NULL);
	rtnl_register(PF_UNSPEC, RTM_GETTCLASS, tc_ctl_tclass, tc_dump_tclass);

	return 0;
}