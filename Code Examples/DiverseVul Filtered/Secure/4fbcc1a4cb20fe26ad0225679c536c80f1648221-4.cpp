static void st21nfca_se_activation_timeout(struct timer_list *t)
{
	struct st21nfca_hci_info *info = from_timer(info, t,
						    se_info.se_active_timer);

	info->se_info.se_active = false;

	complete(&info->se_info.req_completion);
}