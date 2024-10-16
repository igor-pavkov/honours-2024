static BOOL update_send_suppress_output(rdpContext* context, BYTE allow, const RECTANGLE_16* area)
{
	rdpRdp* rdp = context->rdp;

	if (rdp->settings->SuppressOutput)
	{
		wStream* s = rdp_data_pdu_init(rdp);

		if (!s)
			return FALSE;

		update_write_suppress_output(s, allow, area);
		return rdp_send_data_pdu(rdp, s, DATA_PDU_TYPE_SUPPRESS_OUTPUT, rdp->mcs->userId);
	}

	return TRUE;
}