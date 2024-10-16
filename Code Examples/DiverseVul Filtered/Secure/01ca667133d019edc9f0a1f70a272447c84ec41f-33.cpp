int fm10k_init_queueing_scheme(struct fm10k_intfc *interface)
{
	int err;

	/* Number of supported queues */
	fm10k_set_num_queues(interface);

	/* Configure MSI-X capability */
	err = fm10k_init_msix_capability(interface);
	if (err) {
		dev_err(&interface->pdev->dev,
			"Unable to initialize MSI-X capability\n");
		goto err_init_msix;
	}

	/* Allocate memory for queues */
	err = fm10k_alloc_q_vectors(interface);
	if (err) {
		dev_err(&interface->pdev->dev,
			"Unable to allocate queue vectors\n");
		goto err_alloc_q_vectors;
	}

	/* Map rings to devices, and map devices to physical queues */
	fm10k_assign_rings(interface);

	/* Initialize RSS redirection table */
	fm10k_init_reta(interface);

	return 0;

err_alloc_q_vectors:
	fm10k_reset_msix_capability(interface);
err_init_msix:
	fm10k_reset_num_queues(interface);
	return err;
}