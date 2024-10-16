static struct cnic_eth_dev *bnx2x_cnic_probe(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	/* If both iSCSI and FCoE are disabled - return NULL in
	 * order to indicate CNIC that it should not try to work
	 * with this device.
	 */
	if (NO_ISCSI(bp) && NO_FCOE(bp))
		return NULL;

	cp->drv_owner = THIS_MODULE;
	cp->chip_id = CHIP_ID(bp);
	cp->pdev = bp->pdev;
	cp->io_base = bp->regview;
	cp->io_base2 = bp->doorbells;
	cp->max_kwqe_pending = 8;
	cp->ctx_blk_size = CDU_ILT_PAGE_SZ;
	cp->ctx_tbl_offset = FUNC_ILT_BASE(BP_FUNC(bp)) +
			     bnx2x_cid_ilt_lines(bp);
	cp->ctx_tbl_len = CNIC_ILT_LINES;
	cp->starting_cid = bnx2x_cid_ilt_lines(bp) * ILT_PAGE_CIDS;
	cp->drv_submit_kwqes_16 = bnx2x_cnic_sp_queue;
	cp->drv_ctl = bnx2x_drv_ctl;
	cp->drv_get_fc_npiv_tbl = bnx2x_get_fc_npiv;
	cp->drv_register_cnic = bnx2x_register_cnic;
	cp->drv_unregister_cnic = bnx2x_unregister_cnic;
	cp->fcoe_init_cid = BNX2X_FCOE_ETH_CID(bp);
	cp->iscsi_l2_client_id =
		bnx2x_cnic_eth_cl_id(bp, BNX2X_ISCSI_ETH_CL_ID_IDX);
	cp->iscsi_l2_cid = BNX2X_ISCSI_ETH_CID(bp);

	if (NO_ISCSI_OOO(bp))
		cp->drv_state |= CNIC_DRV_STATE_NO_ISCSI_OOO;

	if (NO_ISCSI(bp))
		cp->drv_state |= CNIC_DRV_STATE_NO_ISCSI;

	if (NO_FCOE(bp))
		cp->drv_state |= CNIC_DRV_STATE_NO_FCOE;

	BNX2X_DEV_INFO(
		"page_size %d, tbl_offset %d, tbl_lines %d, starting cid %d\n",
	   cp->ctx_blk_size,
	   cp->ctx_tbl_offset,
	   cp->ctx_tbl_len,
	   cp->starting_cid);
	return cp;
}