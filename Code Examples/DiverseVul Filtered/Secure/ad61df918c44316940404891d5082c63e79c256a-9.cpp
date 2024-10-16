static int __init tc_filter_init(void)
{
	rtnl_register(PF_UNSPEC, RTM_NEWTFILTER, tc_ctl_tfilter, NULL);
	rtnl_register(PF_UNSPEC, RTM_DELTFILTER, tc_ctl_tfilter, NULL);
	rtnl_register(PF_UNSPEC, RTM_GETTFILTER, tc_ctl_tfilter,
						 tc_dump_tfilter);

	return 0;
}