static int megasas_probe_one(struct pci_dev *pdev,
			     const struct pci_device_id *id)
{
	int rval, pos;
	struct Scsi_Host *host;
	struct megasas_instance *instance;
	u16 control = 0;

	switch (pdev->device) {
	case PCI_DEVICE_ID_LSI_AERO_10E1:
	case PCI_DEVICE_ID_LSI_AERO_10E5:
		dev_info(&pdev->dev, "Adapter is in configurable secure mode\n");
		break;
	}

	/* Reset MSI-X in the kdump kernel */
	if (reset_devices) {
		pos = pci_find_capability(pdev, PCI_CAP_ID_MSIX);
		if (pos) {
			pci_read_config_word(pdev, pos + PCI_MSIX_FLAGS,
					     &control);
			if (control & PCI_MSIX_FLAGS_ENABLE) {
				dev_info(&pdev->dev, "resetting MSI-X\n");
				pci_write_config_word(pdev,
						      pos + PCI_MSIX_FLAGS,
						      control &
						      ~PCI_MSIX_FLAGS_ENABLE);
			}
		}
	}

	/*
	 * PCI prepping: enable device set bus mastering and dma mask
	 */
	rval = pci_enable_device_mem(pdev);

	if (rval) {
		return rval;
	}

	pci_set_master(pdev);

	host = scsi_host_alloc(&megasas_template,
			       sizeof(struct megasas_instance));

	if (!host) {
		dev_printk(KERN_DEBUG, &pdev->dev, "scsi_host_alloc failed\n");
		goto fail_alloc_instance;
	}

	instance = (struct megasas_instance *)host->hostdata;
	memset(instance, 0, sizeof(*instance));
	atomic_set(&instance->fw_reset_no_pci_access, 0);

	/*
	 * Initialize PCI related and misc parameters
	 */
	instance->pdev = pdev;
	instance->host = host;
	instance->unique_id = pdev->bus->number << 8 | pdev->devfn;
	instance->init_id = MEGASAS_DEFAULT_INIT_ID;

	megasas_set_adapter_type(instance);

	/*
	 * Initialize MFI Firmware
	 */
	if (megasas_init_fw(instance))
		goto fail_init_mfi;

	if (instance->requestorId) {
		if (instance->PlasmaFW111) {
			instance->vf_affiliation_111 =
				dma_alloc_coherent(&pdev->dev,
					sizeof(struct MR_LD_VF_AFFILIATION_111),
					&instance->vf_affiliation_111_h,
					GFP_KERNEL);
			if (!instance->vf_affiliation_111)
				dev_warn(&pdev->dev, "Can't allocate "
				       "memory for VF affiliation buffer\n");
		} else {
			instance->vf_affiliation =
				dma_alloc_coherent(&pdev->dev,
					(MAX_LOGICAL_DRIVES + 1) *
					sizeof(struct MR_LD_VF_AFFILIATION),
					&instance->vf_affiliation_h,
					GFP_KERNEL);
			if (!instance->vf_affiliation)
				dev_warn(&pdev->dev, "Can't allocate "
				       "memory for VF affiliation buffer\n");
		}
	}

	/*
	 * Store instance in PCI softstate
	 */
	pci_set_drvdata(pdev, instance);

	/*
	 * Add this controller to megasas_mgmt_info structure so that it
	 * can be exported to management applications
	 */
	megasas_mgmt_info.count++;
	megasas_mgmt_info.instance[megasas_mgmt_info.max_index] = instance;
	megasas_mgmt_info.max_index++;

	/*
	 * Register with SCSI mid-layer
	 */
	if (megasas_io_attach(instance))
		goto fail_io_attach;

	instance->unload = 0;
	/*
	 * Trigger SCSI to scan our drives
	 */
	if (!instance->enable_fw_dev_list ||
	    (instance->host_device_list_buf->count > 0))
		scsi_scan_host(host);

	/*
	 * Initiate AEN (Asynchronous Event Notification)
	 */
	if (megasas_start_aen(instance)) {
		dev_printk(KERN_DEBUG, &pdev->dev, "start aen failed\n");
		goto fail_start_aen;
	}

	/* Get current SR-IOV LD/VF affiliation */
	if (instance->requestorId)
		megasas_get_ld_vf_affiliation(instance, 1);

	return 0;

fail_start_aen:
fail_io_attach:
	megasas_mgmt_info.count--;
	megasas_mgmt_info.max_index--;
	megasas_mgmt_info.instance[megasas_mgmt_info.max_index] = NULL;

	instance->instancet->disable_intr(instance);
	megasas_destroy_irqs(instance);

	if (instance->adapter_type != MFI_SERIES)
		megasas_release_fusion(instance);
	else
		megasas_release_mfi(instance);
	if (instance->msix_vectors)
		pci_free_irq_vectors(instance->pdev);
fail_init_mfi:
	scsi_host_put(host);
fail_alloc_instance:
	pci_disable_device(pdev);

	return -ENODEV;
}