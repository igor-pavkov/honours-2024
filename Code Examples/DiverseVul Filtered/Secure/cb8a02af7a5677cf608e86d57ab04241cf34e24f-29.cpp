int get_dscp_value(pcp_info_t *pcp_msg_info) {

	unsigned int ind;

	for (ind = 0; ind < num_dscp_values; ind++) {

		if ((dscp_values_list[ind].app_name) &&
		    (!strcmp(dscp_values_list[ind].app_name,
			     pcp_msg_info->app_name)) &&
		    (pcp_msg_info->delay_tolerance == dscp_values_list[ind].delay) &&
		    (pcp_msg_info->loss_tolerance == dscp_values_list[ind].loss) &&
		    (pcp_msg_info->jitter_tolerance == dscp_values_list[ind].jitter)
		   )
		{
			pcp_msg_info->sadscp_dscp = dscp_values_list[ind].dscp_value;
			pcp_msg_info->matched_name = 1;
			return 0;
		} else
		  if ((pcp_msg_info->app_name_len==0) &&
		      (dscp_values_list[ind].app_name_len==0) &&
		      (pcp_msg_info->delay_tolerance == dscp_values_list[ind].delay) &&
		      (pcp_msg_info->loss_tolerance == dscp_values_list[ind].loss) &&
		      (pcp_msg_info->jitter_tolerance == dscp_values_list[ind].jitter)
		     )
		{
			pcp_msg_info->sadscp_dscp = dscp_values_list[ind].dscp_value;
			pcp_msg_info->matched_name = 0;
			return 0;
		} else
		  if ((dscp_values_list[ind].app_name_len==0) &&
		      (pcp_msg_info->delay_tolerance == dscp_values_list[ind].delay) &&
		      (pcp_msg_info->loss_tolerance == dscp_values_list[ind].loss) &&
		      (pcp_msg_info->jitter_tolerance == dscp_values_list[ind].jitter)
		     )
		{
			pcp_msg_info->sadscp_dscp = dscp_values_list[ind].dscp_value;
			pcp_msg_info->matched_name = 0;
			return 0;
		}
	}
	//if nothing matched return Default value i.e. 0
	pcp_msg_info->sadscp_dscp = 0;
	pcp_msg_info->matched_name = 0;
	return 0;
}