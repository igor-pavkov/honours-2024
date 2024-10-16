static BOOL update_send_refresh_rect(rdpContext* context, BYTE count, const RECTANGLE_16* areas)
{
	rdpRdp* rdp = context->rdp;

	if (rdp->settings->RefreshRect)
	{
		wStream* s = rdp_data_pdu_init(rdp);

		if (!s)
			return FALSE;

		update_write_refresh_rect(s, count, areas);
		return rdp_send_data_pdu(rdp, s, DATA_PDU_TYPE_REFRESH_RECT, rdp->mcs->userId);
	}

	return TRUE;
}