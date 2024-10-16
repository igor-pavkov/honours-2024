static BOOL update_send_play_sound(rdpContext* context, const PLAY_SOUND_UPDATE* play_sound)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;

	if (!rdp->settings->ReceivedCapabilities[CAPSET_TYPE_SOUND])
	{
		return TRUE;
	}

	s = rdp_data_pdu_init(rdp);

	if (!s)
		return FALSE;

	Stream_Write_UINT32(s, play_sound->duration);
	Stream_Write_UINT32(s, play_sound->frequency);
	return rdp_send_data_pdu(rdp, s, DATA_PDU_TYPE_PLAY_SOUND, rdp->mcs->userId);
}