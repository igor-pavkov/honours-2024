static BOOL update_send_set_keyboard_ime_status(rdpContext* context, UINT16 imeId, UINT32 imeState,
                                                UINT32 imeConvMode)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	s = rdp_data_pdu_init(rdp);

	if (!s)
		return FALSE;

	/* unitId should be 0 according to MS-RDPBCGR 2.2.8.2.2.1 */
	Stream_Write_UINT16(s, imeId);
	Stream_Write_UINT32(s, imeState);
	Stream_Write_UINT32(s, imeConvMode);
	return rdp_send_data_pdu(rdp, s, DATA_PDU_TYPE_SET_KEYBOARD_IME_STATUS, rdp->mcs->userId);
}