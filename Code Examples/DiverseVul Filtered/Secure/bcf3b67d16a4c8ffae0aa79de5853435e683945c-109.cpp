static int megasas_init_fw(struct megasas_instance *instance)
{
	u32 max_sectors_1;
	u32 max_sectors_2, tmp_sectors, msix_enable;
	u32 scratch_pad_1, scratch_pad_2, scratch_pad_3, status_reg;
	resource_size_t base_addr;
	struct megasas_ctrl_info *ctrl_info = NULL;
	unsigned long bar_list;
	int i, j, loop, fw_msix_count = 0;
	struct IOV_111 *iovPtr;
	struct fusion_context *fusion;
	bool do_adp_reset = true;

	fusion = instance->ctrl_context;

	/* Find first memory bar */
	bar_list = pci_select_bars(instance->pdev, IORESOURCE_MEM);
	instance->bar = find_first_bit(&bar_list, BITS_PER_LONG);
	if (pci_request_selected_regions(instance->pdev, 1<<instance->bar,
					 "megasas: LSI")) {
		dev_printk(KERN_DEBUG, &instance->pdev->dev, "IO memory region busy!\n");
		return -EBUSY;
	}

	base_addr = pci_resource_start(instance->pdev, instance->bar);
	instance->reg_set = ioremap_nocache(base_addr, 8192);

	if (!instance->reg_set) {
		dev_printk(KERN_DEBUG, &instance->pdev->dev, "Failed to map IO mem\n");
		goto fail_ioremap;
	}

	if (instance->adapter_type != MFI_SERIES)
		instance->instancet = &megasas_instance_template_fusion;
	else {
		switch (instance->pdev->device) {
		case PCI_DEVICE_ID_LSI_SAS1078R:
		case PCI_DEVICE_ID_LSI_SAS1078DE:
			instance->instancet = &megasas_instance_template_ppc;
			break;
		case PCI_DEVICE_ID_LSI_SAS1078GEN2:
		case PCI_DEVICE_ID_LSI_SAS0079GEN2:
			instance->instancet = &megasas_instance_template_gen2;
			break;
		case PCI_DEVICE_ID_LSI_SAS0073SKINNY:
		case PCI_DEVICE_ID_LSI_SAS0071SKINNY:
			instance->instancet = &megasas_instance_template_skinny;
			break;
		case PCI_DEVICE_ID_LSI_SAS1064R:
		case PCI_DEVICE_ID_DELL_PERC5:
		default:
			instance->instancet = &megasas_instance_template_xscale;
			instance->pd_list_not_supported = 1;
			break;
		}
	}

	if (megasas_transition_to_ready(instance, 0)) {
		if (instance->adapter_type >= INVADER_SERIES) {
			status_reg = instance->instancet->read_fw_status_reg(
					instance);
			do_adp_reset = status_reg & MFI_RESET_ADAPTER;
		}

		if (do_adp_reset) {
			atomic_set(&instance->fw_reset_no_pci_access, 1);
			instance->instancet->adp_reset
				(instance, instance->reg_set);
			atomic_set(&instance->fw_reset_no_pci_access, 0);
			dev_info(&instance->pdev->dev,
				 "FW restarted successfully from %s!\n",
				 __func__);

			/*waiting for about 30 second before retry*/
			ssleep(30);

			if (megasas_transition_to_ready(instance, 0))
				goto fail_ready_state;
		} else {
			goto fail_ready_state;
		}
	}

	megasas_init_ctrl_params(instance);

	if (megasas_set_dma_mask(instance))
		goto fail_ready_state;

	if (megasas_alloc_ctrl_mem(instance))
		goto fail_alloc_dma_buf;

	if (megasas_alloc_ctrl_dma_buffers(instance))
		goto fail_alloc_dma_buf;

	fusion = instance->ctrl_context;

	if (instance->adapter_type >= VENTURA_SERIES) {
		scratch_pad_2 =
			megasas_readl(instance,
				      &instance->reg_set->outbound_scratch_pad_2);
		instance->max_raid_mapsize = ((scratch_pad_2 >>
			MR_MAX_RAID_MAP_SIZE_OFFSET_SHIFT) &
			MR_MAX_RAID_MAP_SIZE_MASK);
	}

	/* Check if MSI-X is supported while in ready state */
	msix_enable = (instance->instancet->read_fw_status_reg(instance) &
		       0x4000000) >> 0x1a;
	if (msix_enable && !msix_disable) {
		int irq_flags = PCI_IRQ_MSIX;

		scratch_pad_1 = megasas_readl
			(instance, &instance->reg_set->outbound_scratch_pad_1);
		/* Check max MSI-X vectors */
		if (fusion) {
			if (instance->adapter_type == THUNDERBOLT_SERIES) {
				/* Thunderbolt Series*/
				instance->msix_vectors = (scratch_pad_1
					& MR_MAX_REPLY_QUEUES_OFFSET) + 1;
				fw_msix_count = instance->msix_vectors;
			} else {
				instance->msix_vectors = ((scratch_pad_1
					& MR_MAX_REPLY_QUEUES_EXT_OFFSET)
					>> MR_MAX_REPLY_QUEUES_EXT_OFFSET_SHIFT) + 1;

				/*
				 * For Invader series, > 8 MSI-x vectors
				 * supported by FW/HW implies combined
				 * reply queue mode is enabled.
				 * For Ventura series, > 16 MSI-x vectors
				 * supported by FW/HW implies combined
				 * reply queue mode is enabled.
				 */
				switch (instance->adapter_type) {
				case INVADER_SERIES:
					if (instance->msix_vectors > 8)
						instance->msix_combined = true;
					break;
				case AERO_SERIES:
				case VENTURA_SERIES:
					if (instance->msix_vectors > 16)
						instance->msix_combined = true;
					break;
				}

				if (rdpq_enable)
					instance->is_rdpq = (scratch_pad_1 & MR_RDPQ_MODE_OFFSET) ?
								1 : 0;
				fw_msix_count = instance->msix_vectors;
				/* Save 1-15 reply post index address to local memory
				 * Index 0 is already saved from reg offset
				 * MPI2_REPLY_POST_HOST_INDEX_OFFSET
				 */
				for (loop = 1; loop < MR_MAX_MSIX_REG_ARRAY; loop++) {
					instance->reply_post_host_index_addr[loop] =
						(u32 __iomem *)
						((u8 __iomem *)instance->reg_set +
						MPI2_SUP_REPLY_POST_HOST_INDEX_OFFSET
						+ (loop * 0x10));
				}
			}
			if (msix_vectors)
				instance->msix_vectors = min(msix_vectors,
					instance->msix_vectors);
		} else /* MFI adapters */
			instance->msix_vectors = 1;
		/* Don't bother allocating more MSI-X vectors than cpus */
		instance->msix_vectors = min(instance->msix_vectors,
					     (unsigned int)num_online_cpus());
		if (smp_affinity_enable)
			irq_flags |= PCI_IRQ_AFFINITY;
		i = pci_alloc_irq_vectors(instance->pdev, 1,
					  instance->msix_vectors, irq_flags);
		if (i > 0)
			instance->msix_vectors = i;
		else
			instance->msix_vectors = 0;
	}
	/*
	 * MSI-X host index 0 is common for all adapter.
	 * It is used for all MPT based Adapters.
	 */
	if (instance->msix_combined) {
		instance->reply_post_host_index_addr[0] =
				(u32 *)((u8 *)instance->reg_set +
				MPI2_SUP_REPLY_POST_HOST_INDEX_OFFSET);
	} else {
		instance->reply_post_host_index_addr[0] =
			(u32 *)((u8 *)instance->reg_set +
			MPI2_REPLY_POST_HOST_INDEX_OFFSET);
	}

	if (!instance->msix_vectors) {
		i = pci_alloc_irq_vectors(instance->pdev, 1, 1, PCI_IRQ_LEGACY);
		if (i < 0)
			goto fail_init_adapter;
	}

	megasas_setup_reply_map(instance);

	dev_info(&instance->pdev->dev,
		"firmware supports msix\t: (%d)", fw_msix_count);
	dev_info(&instance->pdev->dev,
		"current msix/online cpus\t: (%d/%d)\n",
		instance->msix_vectors, (unsigned int)num_online_cpus());
	dev_info(&instance->pdev->dev,
		"RDPQ mode\t: (%s)\n", instance->is_rdpq ? "enabled" : "disabled");

	tasklet_init(&instance->isr_tasklet, instance->instancet->tasklet,
		(unsigned long)instance);

	/*
	 * Below are default value for legacy Firmware.
	 * non-fusion based controllers
	 */
	instance->fw_supported_vd_count = MAX_LOGICAL_DRIVES;
	instance->fw_supported_pd_count = MAX_PHYSICAL_DEVICES;
	/* Get operational params, sge flags, send init cmd to controller */
	if (instance->instancet->init_adapter(instance))
		goto fail_init_adapter;

	if (instance->adapter_type >= VENTURA_SERIES) {
		scratch_pad_3 =
			megasas_readl(instance,
				      &instance->reg_set->outbound_scratch_pad_3);
		if ((scratch_pad_3 & MR_NVME_PAGE_SIZE_MASK) >=
			MR_DEFAULT_NVME_PAGE_SHIFT)
			instance->nvme_page_size =
				(1 << (scratch_pad_3 & MR_NVME_PAGE_SIZE_MASK));

		dev_info(&instance->pdev->dev,
			 "NVME page size\t: (%d)\n", instance->nvme_page_size);
	}

	if (instance->msix_vectors ?
		megasas_setup_irqs_msix(instance, 1) :
		megasas_setup_irqs_ioapic(instance))
		goto fail_init_adapter;

	instance->instancet->enable_intr(instance);

	dev_info(&instance->pdev->dev, "INIT adapter done\n");

	megasas_setup_jbod_map(instance);

	if (megasas_get_device_list(instance) != SUCCESS) {
		dev_err(&instance->pdev->dev,
			"%s: megasas_get_device_list failed\n",
			__func__);
		goto fail_get_ld_pd_list;
	}

	/* stream detection initialization */
	if (instance->adapter_type >= VENTURA_SERIES) {
		fusion->stream_detect_by_ld =
			kcalloc(MAX_LOGICAL_DRIVES_EXT,
				sizeof(struct LD_STREAM_DETECT *),
				GFP_KERNEL);
		if (!fusion->stream_detect_by_ld) {
			dev_err(&instance->pdev->dev,
				"unable to allocate stream detection for pool of LDs\n");
			goto fail_get_ld_pd_list;
		}
		for (i = 0; i < MAX_LOGICAL_DRIVES_EXT; ++i) {
			fusion->stream_detect_by_ld[i] =
				kzalloc(sizeof(struct LD_STREAM_DETECT),
				GFP_KERNEL);
			if (!fusion->stream_detect_by_ld[i]) {
				dev_err(&instance->pdev->dev,
					"unable to allocate stream detect by LD\n ");
				for (j = 0; j < i; ++j)
					kfree(fusion->stream_detect_by_ld[j]);
				kfree(fusion->stream_detect_by_ld);
				fusion->stream_detect_by_ld = NULL;
				goto fail_get_ld_pd_list;
			}
			fusion->stream_detect_by_ld[i]->mru_bit_map
				= MR_STREAM_BITMAP;
		}
	}

	/*
	 * Compute the max allowed sectors per IO: The controller info has two
	 * limits on max sectors. Driver should use the minimum of these two.
	 *
	 * 1 << stripe_sz_ops.min = max sectors per strip
	 *
	 * Note that older firmwares ( < FW ver 30) didn't report information
	 * to calculate max_sectors_1. So the number ended up as zero always.
	 */
	tmp_sectors = 0;
	ctrl_info = instance->ctrl_info_buf;

	max_sectors_1 = (1 << ctrl_info->stripe_sz_ops.min) *
		le16_to_cpu(ctrl_info->max_strips_per_io);
	max_sectors_2 = le32_to_cpu(ctrl_info->max_request_size);

	tmp_sectors = min_t(u32, max_sectors_1, max_sectors_2);

	instance->peerIsPresent = ctrl_info->cluster.peerIsPresent;
	instance->passive = ctrl_info->cluster.passive;
	memcpy(instance->clusterId, ctrl_info->clusterId, sizeof(instance->clusterId));
	instance->UnevenSpanSupport =
		ctrl_info->adapterOperations2.supportUnevenSpans;
	if (instance->UnevenSpanSupport) {
		struct fusion_context *fusion = instance->ctrl_context;
		if (MR_ValidateMapInfo(instance, instance->map_id))
			fusion->fast_path_io = 1;
		else
			fusion->fast_path_io = 0;

	}
	if (ctrl_info->host_interface.SRIOV) {
		instance->requestorId = ctrl_info->iov.requestorId;
		if (instance->pdev->device == PCI_DEVICE_ID_LSI_PLASMA) {
			if (!ctrl_info->adapterOperations2.activePassive)
			    instance->PlasmaFW111 = 1;

			dev_info(&instance->pdev->dev, "SR-IOV: firmware type: %s\n",
			    instance->PlasmaFW111 ? "1.11" : "new");

			if (instance->PlasmaFW111) {
			    iovPtr = (struct IOV_111 *)
				((unsigned char *)ctrl_info + IOV_111_OFFSET);
			    instance->requestorId = iovPtr->requestorId;
			}
		}
		dev_info(&instance->pdev->dev, "SRIOV: VF requestorId %d\n",
			instance->requestorId);
	}

	instance->crash_dump_fw_support =
		ctrl_info->adapterOperations3.supportCrashDump;
	instance->crash_dump_drv_support =
		(instance->crash_dump_fw_support &&
		instance->crash_dump_buf);
	if (instance->crash_dump_drv_support)
		megasas_set_crash_dump_params(instance,
			MR_CRASH_BUF_TURN_OFF);

	else {
		if (instance->crash_dump_buf)
			dma_free_coherent(&instance->pdev->dev,
				CRASH_DMA_BUF_SIZE,
				instance->crash_dump_buf,
				instance->crash_dump_h);
		instance->crash_dump_buf = NULL;
	}

	if (instance->snapdump_wait_time) {
		megasas_get_snapdump_properties(instance);
		dev_info(&instance->pdev->dev, "Snap dump wait time\t: %d\n",
			 instance->snapdump_wait_time);
	}

	dev_info(&instance->pdev->dev,
		"pci id\t\t: (0x%04x)/(0x%04x)/(0x%04x)/(0x%04x)\n",
		le16_to_cpu(ctrl_info->pci.vendor_id),
		le16_to_cpu(ctrl_info->pci.device_id),
		le16_to_cpu(ctrl_info->pci.sub_vendor_id),
		le16_to_cpu(ctrl_info->pci.sub_device_id));
	dev_info(&instance->pdev->dev, "unevenspan support	: %s\n",
		instance->UnevenSpanSupport ? "yes" : "no");
	dev_info(&instance->pdev->dev, "firmware crash dump	: %s\n",
		instance->crash_dump_drv_support ? "yes" : "no");
	dev_info(&instance->pdev->dev, "jbod sync map		: %s\n",
		instance->use_seqnum_jbod_fp ? "yes" : "no");

	instance->max_sectors_per_req = instance->max_num_sge *
						SGE_BUFFER_SIZE / 512;
	if (tmp_sectors && (instance->max_sectors_per_req > tmp_sectors))
		instance->max_sectors_per_req = tmp_sectors;

	/* Check for valid throttlequeuedepth module parameter */
	if (throttlequeuedepth &&
			throttlequeuedepth <= instance->max_scsi_cmds)
		instance->throttlequeuedepth = throttlequeuedepth;
	else
		instance->throttlequeuedepth =
				MEGASAS_THROTTLE_QUEUE_DEPTH;

	if ((resetwaittime < 1) ||
	    (resetwaittime > MEGASAS_RESET_WAIT_TIME))
		resetwaittime = MEGASAS_RESET_WAIT_TIME;

	if ((scmd_timeout < 10) || (scmd_timeout > MEGASAS_DEFAULT_CMD_TIMEOUT))
		scmd_timeout = MEGASAS_DEFAULT_CMD_TIMEOUT;

	/* Launch SR-IOV heartbeat timer */
	if (instance->requestorId) {
		if (!megasas_sriov_start_heartbeat(instance, 1)) {
			megasas_start_timer(instance);
		} else {
			instance->skip_heartbeat_timer_del = 1;
			goto fail_get_ld_pd_list;
		}
	}

	/*
	 * Create and start watchdog thread which will monitor
	 * controller state every 1 sec and trigger OCR when
	 * it enters fault state
	 */
	if (instance->adapter_type != MFI_SERIES)
		if (megasas_fusion_start_watchdog(instance) != SUCCESS)
			goto fail_start_watchdog;

	return 0;

fail_start_watchdog:
	if (instance->requestorId && !instance->skip_heartbeat_timer_del)
		del_timer_sync(&instance->sriov_heartbeat_timer);
fail_get_ld_pd_list:
	instance->instancet->disable_intr(instance);
	megasas_destroy_irqs(instance);
fail_init_adapter:
	if (instance->msix_vectors)
		pci_free_irq_vectors(instance->pdev);
	instance->msix_vectors = 0;
fail_alloc_dma_buf:
	megasas_free_ctrl_dma_buffers(instance);
	megasas_free_ctrl_mem(instance);
fail_ready_state:
	iounmap(instance->reg_set);

fail_ioremap:
	pci_release_selected_regions(instance->pdev, 1<<instance->bar);

	dev_err(&instance->pdev->dev, "Failed from %s %d\n",
		__func__, __LINE__);
	return -EINVAL;
}