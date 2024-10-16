static void fm10k_reset_num_queues(struct fm10k_intfc *interface)
{
	interface->num_tx_queues = 0;
	interface->num_rx_queues = 0;
	interface->num_q_vectors = 0;
}