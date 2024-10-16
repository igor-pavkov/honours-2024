static int fm10k_init_msix_capability(struct fm10k_intfc *interface)
{
	struct fm10k_hw *hw = &interface->hw;
	int v_budget, vector;

	/* It's easy to be greedy for MSI-X vectors, but it really
	 * doesn't do us much good if we have a lot more vectors
	 * than CPU's.  So let's be conservative and only ask for
	 * (roughly) the same number of vectors as there are CPU's.
	 * the default is to use pairs of vectors
	 */
	v_budget = max(interface->num_rx_queues, interface->num_tx_queues);
	v_budget = min_t(u16, v_budget, num_online_cpus());

	/* account for vectors not related to queues */
	v_budget += NON_Q_VECTORS(hw);

	/* At the same time, hardware can only support a maximum of
	 * hw.mac->max_msix_vectors vectors.  With features
	 * such as RSS and VMDq, we can easily surpass the number of Rx and Tx
	 * descriptor queues supported by our device.  Thus, we cap it off in
	 * those rare cases where the cpu count also exceeds our vector limit.
	 */
	v_budget = min_t(int, v_budget, hw->mac.max_msix_vectors);

	/* A failure in MSI-X entry allocation is fatal. */
	interface->msix_entries = kcalloc(v_budget, sizeof(struct msix_entry),
					  GFP_KERNEL);
	if (!interface->msix_entries)
		return -ENOMEM;

	/* populate entry values */
	for (vector = 0; vector < v_budget; vector++)
		interface->msix_entries[vector].entry = vector;

	/* Attempt to enable MSI-X with requested value */
	v_budget = pci_enable_msix_range(interface->pdev,
					 interface->msix_entries,
					 MIN_MSIX_COUNT(hw),
					 v_budget);
	if (v_budget < 0) {
		kfree(interface->msix_entries);
		interface->msix_entries = NULL;
		return v_budget;
	}

	/* record the number of queues available for q_vectors */
	interface->num_q_vectors = v_budget - NON_Q_VECTORS(hw);

	return 0;
}