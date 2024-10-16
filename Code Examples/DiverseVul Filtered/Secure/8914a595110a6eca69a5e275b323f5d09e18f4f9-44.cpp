static void bnx2x_read_fwinfo(struct bnx2x *bp)
{
	int cnt, i, block_end, rodi;
	char vpd_start[BNX2X_VPD_LEN+1];
	char str_id_reg[VENDOR_ID_LEN+1];
	char str_id_cap[VENDOR_ID_LEN+1];
	char *vpd_data;
	char *vpd_extended_data = NULL;
	u8 len;

	cnt = pci_read_vpd(bp->pdev, 0, BNX2X_VPD_LEN, vpd_start);
	memset(bp->fw_ver, 0, sizeof(bp->fw_ver));

	if (cnt < BNX2X_VPD_LEN)
		goto out_not_found;

	/* VPD RO tag should be first tag after identifier string, hence
	 * we should be able to find it in first BNX2X_VPD_LEN chars
	 */
	i = pci_vpd_find_tag(vpd_start, 0, BNX2X_VPD_LEN,
			     PCI_VPD_LRDT_RO_DATA);
	if (i < 0)
		goto out_not_found;

	block_end = i + PCI_VPD_LRDT_TAG_SIZE +
		    pci_vpd_lrdt_size(&vpd_start[i]);

	i += PCI_VPD_LRDT_TAG_SIZE;

	if (block_end > BNX2X_VPD_LEN) {
		vpd_extended_data = kmalloc(block_end, GFP_KERNEL);
		if (vpd_extended_data  == NULL)
			goto out_not_found;

		/* read rest of vpd image into vpd_extended_data */
		memcpy(vpd_extended_data, vpd_start, BNX2X_VPD_LEN);
		cnt = pci_read_vpd(bp->pdev, BNX2X_VPD_LEN,
				   block_end - BNX2X_VPD_LEN,
				   vpd_extended_data + BNX2X_VPD_LEN);
		if (cnt < (block_end - BNX2X_VPD_LEN))
			goto out_not_found;
		vpd_data = vpd_extended_data;
	} else
		vpd_data = vpd_start;

	/* now vpd_data holds full vpd content in both cases */

	rodi = pci_vpd_find_info_keyword(vpd_data, i, block_end,
				   PCI_VPD_RO_KEYWORD_MFR_ID);
	if (rodi < 0)
		goto out_not_found;

	len = pci_vpd_info_field_size(&vpd_data[rodi]);

	if (len != VENDOR_ID_LEN)
		goto out_not_found;

	rodi += PCI_VPD_INFO_FLD_HDR_SIZE;

	/* vendor specific info */
	snprintf(str_id_reg, VENDOR_ID_LEN + 1, "%04x", PCI_VENDOR_ID_DELL);
	snprintf(str_id_cap, VENDOR_ID_LEN + 1, "%04X", PCI_VENDOR_ID_DELL);
	if (!strncmp(str_id_reg, &vpd_data[rodi], VENDOR_ID_LEN) ||
	    !strncmp(str_id_cap, &vpd_data[rodi], VENDOR_ID_LEN)) {

		rodi = pci_vpd_find_info_keyword(vpd_data, i, block_end,
						PCI_VPD_RO_KEYWORD_VENDOR0);
		if (rodi >= 0) {
			len = pci_vpd_info_field_size(&vpd_data[rodi]);

			rodi += PCI_VPD_INFO_FLD_HDR_SIZE;

			if (len < 32 && (len + rodi) <= BNX2X_VPD_LEN) {
				memcpy(bp->fw_ver, &vpd_data[rodi], len);
				bp->fw_ver[len] = ' ';
			}
		}
		kfree(vpd_extended_data);
		return;
	}
out_not_found:
	kfree(vpd_extended_data);
	return;
}