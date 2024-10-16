void megasas_start_timer(struct megasas_instance *instance)
{
	struct timer_list *timer = &instance->sriov_heartbeat_timer;

	timer_setup(timer, megasas_sriov_heartbeat_handler, 0);
	timer->expires = jiffies + MEGASAS_SRIOV_HEARTBEAT_INTERVAL_VF;
	add_timer(timer);
}