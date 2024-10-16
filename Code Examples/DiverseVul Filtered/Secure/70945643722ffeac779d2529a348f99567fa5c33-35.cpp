static int check2ndT2(struct smb_hdr *pSMB, unsigned int maxBufSize)
{
	struct smb_t2_rsp *pSMBt;
	int remaining;
	__u16 total_data_size, data_in_this_rsp;

	if (pSMB->Command != SMB_COM_TRANSACTION2)
		return 0;

	/* check for plausible wct, bcc and t2 data and parm sizes */
	/* check for parm and data offset going beyond end of smb */
	if (pSMB->WordCount != 10) { /* coalesce_t2 depends on this */
		cFYI(1, "invalid transact2 word count");
		return -EINVAL;
	}

	pSMBt = (struct smb_t2_rsp *)pSMB;

	total_data_size = get_unaligned_le16(&pSMBt->t2_rsp.TotalDataCount);
	data_in_this_rsp = get_unaligned_le16(&pSMBt->t2_rsp.DataCount);

	remaining = total_data_size - data_in_this_rsp;

	if (remaining == 0)
		return 0;
	else if (remaining < 0) {
		cFYI(1, "total data %d smaller than data in frame %d",
			total_data_size, data_in_this_rsp);
		return -EINVAL;
	} else {
		cFYI(1, "missing %d bytes from transact2, check next response",
			remaining);
		if (total_data_size > maxBufSize) {
			cERROR(1, "TotalDataSize %d is over maximum buffer %d",
				total_data_size, maxBufSize);
			return -EINVAL;
		}
		return remaining;
	}
}