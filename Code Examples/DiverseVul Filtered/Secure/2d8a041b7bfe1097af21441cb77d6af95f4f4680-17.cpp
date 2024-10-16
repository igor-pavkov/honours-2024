static int ip_vs_zero_all(struct net *net)
{
	int idx;
	struct ip_vs_service *svc;

	for(idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		list_for_each_entry(svc, &ip_vs_svc_table[idx], s_list) {
			if (net_eq(svc->net, net))
				ip_vs_zero_service(svc);
		}
	}

	for(idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		list_for_each_entry(svc, &ip_vs_svc_fwm_table[idx], f_list) {
			if (net_eq(svc->net, net))
				ip_vs_zero_service(svc);
		}
	}

	ip_vs_zero_stats(&net_ipvs(net)->tot_stats);
	return 0;
}