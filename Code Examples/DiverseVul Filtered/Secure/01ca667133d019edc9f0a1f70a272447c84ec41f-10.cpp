void fm10k_clear_queueing_scheme(struct fm10k_intfc *interface)
{
	fm10k_free_q_vectors(interface);
	fm10k_reset_msix_capability(interface);
}