static BOOL update_send_frame_acknowledge(rdpContext* context, UINT32 frameId)
{
	rdpRdp* rdp = context->rdp;

	if (rdp->settings->ReceivedCapabilities[CAPSET_TYPE_FRAME_ACKNOWLEDGE])
	{
		wStream* s = rdp_data_pdu_init(rdp);

		if (!s)
			return FALSE;

		Stream_Write_UINT32(s, frameId);
		return rdp_send_data_pdu(rdp, s, DATA_PDU_TYPE_FRAME_ACKNOWLEDGE, rdp->mcs->userId);
	}

	return TRUE;
}