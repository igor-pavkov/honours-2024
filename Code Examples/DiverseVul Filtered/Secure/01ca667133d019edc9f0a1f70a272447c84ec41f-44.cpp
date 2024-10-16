static void fm10k_set_num_queues(struct fm10k_intfc *interface)
{
	/* Attempt to setup QoS and RSS first */
	if (fm10k_set_qos_queues(interface))
		return;

	/* If we don't have QoS, just fallback to only RSS. */
	fm10k_set_rss_queues(interface);
}