static void fm10k_free_q_vectors(struct fm10k_intfc *interface)
{
	int v_idx = interface->num_q_vectors;

	fm10k_reset_num_queues(interface);

	while (v_idx--)
		fm10k_free_q_vector(interface, v_idx);
}