static void megasas_detach_one(struct pci_dev *pdev)
{
	int i;
	struct Scsi_Host *host;
	struct megasas_instance *instance;
	struct fusion_context *fusion;
	u32 pd_seq_map_sz;

	instance = pci_get_drvdata(pdev);
	host = instance->host;
	fusion = instance->ctrl_context;

	/* Shutdown SR-IOV heartbeat timer */
	if (instance->requestorId && !instance->skip_heartbeat_timer_del)
		del_timer_sync(&instance->sriov_heartbeat_timer);

	/* Stop the FW fault detection watchdog */
	if (instance->adapter_type != MFI_SERIES)
		megasas_fusion_stop_watchdog(instance);

	if (instance->fw_crash_state != UNAVAILABLE)
		megasas_free_host_crash_buffer(instance);
	scsi_remove_host(instance->host);
	instance->unload = 1;

	if (megasas_wait_for_adapter_operational(instance))
		goto skip_firing_dcmds;

	megasas_flush_cache(instance);
	megasas_shutdown_controller(instance, MR_DCMD_CTRL_SHUTDOWN);

skip_firing_dcmds:
	/* cancel the delayed work if this work still in queue*/
	if (instance->ev != NULL) {
		struct megasas_aen_event *ev = instance->ev;
		cancel_delayed_work_sync(&ev->hotplug_work);
		instance->ev = NULL;
	}

	/* cancel all wait events */
	wake_up_all(&instance->int_cmd_wait_q);

	tasklet_kill(&instance->isr_tasklet);

	/*
	 * Take the instance off the instance array. Note that we will not
	 * decrement the max_index. We let this array be sparse array
	 */
	for (i = 0; i < megasas_mgmt_info.max_index; i++) {
		if (megasas_mgmt_info.instance[i] == instance) {
			megasas_mgmt_info.count--;
			megasas_mgmt_info.instance[i] = NULL;

			break;
		}
	}

	instance->instancet->disable_intr(instance);

	megasas_destroy_irqs(instance);

	if (instance->msix_vectors)
		pci_free_irq_vectors(instance->pdev);

	if (instance->adapter_type >= VENTURA_SERIES) {
		for (i = 0; i < MAX_LOGICAL_DRIVES_EXT; ++i)
			kfree(fusion->stream_detect_by_ld[i]);
		kfree(fusion->stream_detect_by_ld);
		fusion->stream_detect_by_ld = NULL;
	}


	if (instance->adapter_type != MFI_SERIES) {
		megasas_release_fusion(instance);
			pd_seq_map_sz = sizeof(struct MR_PD_CFG_SEQ_NUM_SYNC) +
				(sizeof(struct MR_PD_CFG_SEQ) *
					(MAX_PHYSICAL_DEVICES - 1));
		for (i = 0; i < 2 ; i++) {
			if (fusion->ld_map[i])
				dma_free_coherent(&instance->pdev->dev,
						  fusion->max_map_sz,
						  fusion->ld_map[i],
						  fusion->ld_map_phys[i]);
			if (fusion->ld_drv_map[i]) {
				if (is_vmalloc_addr(fusion->ld_drv_map[i]))
					vfree(fusion->ld_drv_map[i]);
				else
					free_pages((ulong)fusion->ld_drv_map[i],
						   fusion->drv_map_pages);
			}

			if (fusion->pd_seq_sync[i])
				dma_free_coherent(&instance->pdev->dev,
					pd_seq_map_sz,
					fusion->pd_seq_sync[i],
					fusion->pd_seq_phys[i]);
		}
	} else {
		megasas_release_mfi(instance);
	}

	if (instance->vf_affiliation)
		dma_free_coherent(&pdev->dev, (MAX_LOGICAL_DRIVES + 1) *
				    sizeof(struct MR_LD_VF_AFFILIATION),
				    instance->vf_affiliation,
				    instance->vf_affiliation_h);

	if (instance->vf_affiliation_111)
		dma_free_coherent(&pdev->dev,
				    sizeof(struct MR_LD_VF_AFFILIATION_111),
				    instance->vf_affiliation_111,
				    instance->vf_affiliation_111_h);

	if (instance->hb_host_mem)
		dma_free_coherent(&pdev->dev, sizeof(struct MR_CTRL_HB_HOST_MEM),
				    instance->hb_host_mem,
				    instance->hb_host_mem_h);

	megasas_free_ctrl_dma_buffers(instance);

	megasas_free_ctrl_mem(instance);

	scsi_host_put(host);

	pci_disable_device(pdev);
}