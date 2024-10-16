static int coalesce_t2(struct smb_hdr *psecond, struct smb_hdr *pTargetSMB)
{
	struct smb_t2_rsp *pSMB2 = (struct smb_t2_rsp *)psecond;
	struct smb_t2_rsp *pSMBt  = (struct smb_t2_rsp *)pTargetSMB;
	char *data_area_of_target;
	char *data_area_of_buf2;
	int remaining;
	__u16 byte_count, total_data_size, total_in_buf, total_in_buf2;

	total_data_size = get_unaligned_le16(&pSMBt->t2_rsp.TotalDataCount);

	if (total_data_size !=
	    get_unaligned_le16(&pSMB2->t2_rsp.TotalDataCount))
		cFYI(1, "total data size of primary and secondary t2 differ");

	total_in_buf = get_unaligned_le16(&pSMBt->t2_rsp.DataCount);

	remaining = total_data_size - total_in_buf;

	if (remaining < 0)
		return -EINVAL;

	if (remaining == 0) /* nothing to do, ignore */
		return 0;

	total_in_buf2 = get_unaligned_le16(&pSMB2->t2_rsp.DataCount);
	if (remaining < total_in_buf2) {
		cFYI(1, "transact2 2nd response contains too much data");
	}

	/* find end of first SMB data area */
	data_area_of_target = (char *)&pSMBt->hdr.Protocol +
				get_unaligned_le16(&pSMBt->t2_rsp.DataOffset);
	/* validate target area */

	data_area_of_buf2 = (char *)&pSMB2->hdr.Protocol +
				get_unaligned_le16(&pSMB2->t2_rsp.DataOffset);

	data_area_of_target += total_in_buf;

	/* copy second buffer into end of first buffer */
	memcpy(data_area_of_target, data_area_of_buf2, total_in_buf2);
	total_in_buf += total_in_buf2;
	put_unaligned_le16(total_in_buf, &pSMBt->t2_rsp.DataCount);
	byte_count = get_bcc_le(pTargetSMB);
	byte_count += total_in_buf2;
	put_bcc_le(byte_count, pTargetSMB);

	byte_count = pTargetSMB->smb_buf_length;
	byte_count += total_in_buf2;

	/* BB also add check that we are not beyond maximum buffer size */

	pTargetSMB->smb_buf_length = byte_count;

	if (remaining == total_in_buf2) {
		cFYI(1, "found the last secondary response");
		return 0; /* we are done */
	} else /* more responses to go */
		return 1;
}