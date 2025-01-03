static int bnx2x_init_bp(struct bnx2x *bp)
{
	int func;
	int rc;

	mutex_init(&bp->port.phy_mutex);
	mutex_init(&bp->fw_mb_mutex);
	mutex_init(&bp->drv_info_mutex);
	sema_init(&bp->stats_lock, 1);
	bp->drv_info_mng_owner = false;
	INIT_LIST_HEAD(&bp->vlan_reg);

	INIT_DELAYED_WORK(&bp->sp_task, bnx2x_sp_task);
	INIT_DELAYED_WORK(&bp->sp_rtnl_task, bnx2x_sp_rtnl_task);
	INIT_DELAYED_WORK(&bp->period_task, bnx2x_period_task);
	INIT_DELAYED_WORK(&bp->iov_task, bnx2x_iov_task);
	if (IS_PF(bp)) {
		rc = bnx2x_get_hwinfo(bp);
		if (rc)
			return rc;
	} else {
		eth_zero_addr(bp->dev->dev_addr);
	}

	bnx2x_set_modes_bitmap(bp);

	rc = bnx2x_alloc_mem_bp(bp);
	if (rc)
		return rc;

	bnx2x_read_fwinfo(bp);

	func = BP_FUNC(bp);

	/* need to reset chip if undi was active */
	if (IS_PF(bp) && !BP_NOMCP(bp)) {
		/* init fw_seq */
		bp->fw_seq =
			SHMEM_RD(bp, func_mb[BP_FW_MB_IDX(bp)].drv_mb_header) &
							DRV_MSG_SEQ_NUMBER_MASK;
		BNX2X_DEV_INFO("fw_seq 0x%08x\n", bp->fw_seq);

		rc = bnx2x_prev_unload(bp);
		if (rc) {
			bnx2x_free_mem_bp(bp);
			return rc;
		}
	}

	if (CHIP_REV_IS_FPGA(bp))
		dev_err(&bp->pdev->dev, "FPGA detected\n");

	if (BP_NOMCP(bp) && (func == 0))
		dev_err(&bp->pdev->dev, "MCP disabled, must load devices in order!\n");

	bp->disable_tpa = disable_tpa;
	bp->disable_tpa |= !!IS_MF_STORAGE_ONLY(bp);
	/* Reduce memory usage in kdump environment by disabling TPA */
	bp->disable_tpa |= is_kdump_kernel();

	/* Set TPA flags */
	if (bp->disable_tpa) {
		bp->dev->hw_features &= ~(NETIF_F_LRO | NETIF_F_GRO_HW);
		bp->dev->features &= ~(NETIF_F_LRO | NETIF_F_GRO_HW);
	}

	if (CHIP_IS_E1(bp))
		bp->dropless_fc = 0;
	else
		bp->dropless_fc = dropless_fc | bnx2x_get_dropless_info(bp);

	bp->mrrs = mrrs;

	bp->tx_ring_size = IS_MF_STORAGE_ONLY(bp) ? 0 : MAX_TX_AVAIL;
	if (IS_VF(bp))
		bp->rx_ring_size = MAX_RX_AVAIL;

	/* make sure that the numbers are in the right granularity */
	bp->tx_ticks = (50 / BNX2X_BTR) * BNX2X_BTR;
	bp->rx_ticks = (25 / BNX2X_BTR) * BNX2X_BTR;

	bp->current_interval = CHIP_REV_IS_SLOW(bp) ? 5*HZ : HZ;

	timer_setup(&bp->timer, bnx2x_timer, 0);
	bp->timer.expires = jiffies + bp->current_interval;

	if (SHMEM2_HAS(bp, dcbx_lldp_params_offset) &&
	    SHMEM2_HAS(bp, dcbx_lldp_dcbx_stat_offset) &&
	    SHMEM2_HAS(bp, dcbx_en) &&
	    SHMEM2_RD(bp, dcbx_lldp_params_offset) &&
	    SHMEM2_RD(bp, dcbx_lldp_dcbx_stat_offset) &&
	    SHMEM2_RD(bp, dcbx_en[BP_PORT(bp)])) {
		bnx2x_dcbx_set_state(bp, true, BNX2X_DCBX_ENABLED_ON_NEG_ON);
		bnx2x_dcbx_init_params(bp);
	} else {
		bnx2x_dcbx_set_state(bp, false, BNX2X_DCBX_ENABLED_OFF);
	}

	if (CHIP_IS_E1x(bp))
		bp->cnic_base_cl_id = FP_SB_MAX_E1x;
	else
		bp->cnic_base_cl_id = FP_SB_MAX_E2;

	/* multiple tx priority */
	if (IS_VF(bp))
		bp->max_cos = 1;
	else if (CHIP_IS_E1x(bp))
		bp->max_cos = BNX2X_MULTI_TX_COS_E1X;
	else if (CHIP_IS_E2(bp) || CHIP_IS_E3A0(bp))
		bp->max_cos = BNX2X_MULTI_TX_COS_E2_E3A0;
	else if (CHIP_IS_E3B0(bp))
		bp->max_cos = BNX2X_MULTI_TX_COS_E3B0;
	else
		BNX2X_ERR("unknown chip %x revision %x\n",
			  CHIP_NUM(bp), CHIP_REV(bp));
	BNX2X_DEV_INFO("set bp->max_cos to %d\n", bp->max_cos);

	/* We need at least one default status block for slow-path events,
	 * second status block for the L2 queue, and a third status block for
	 * CNIC if supported.
	 */
	if (IS_VF(bp))
		bp->min_msix_vec_cnt = 1;
	else if (CNIC_SUPPORT(bp))
		bp->min_msix_vec_cnt = 3;
	else /* PF w/o cnic */
		bp->min_msix_vec_cnt = 2;
	BNX2X_DEV_INFO("bp->min_msix_vec_cnt %d", bp->min_msix_vec_cnt);

	bp->dump_preset_idx = 1;

	if (CHIP_IS_E3B0(bp))
		bp->flags |= PTP_SUPPORTED;

	return rc;
}