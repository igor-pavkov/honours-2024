static BOOL update_send_pointer_position(rdpContext* context,
                                         const POINTER_POSITION_UPDATE* pointerPosition)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, 16))
		goto out_fail;

	Stream_Write_UINT16(s, pointerPosition->xPos); /* xPos (2 bytes) */
	Stream_Write_UINT16(s, pointerPosition->yPos); /* yPos (2 bytes) */
	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_PTR_POSITION, s, FALSE);
out_fail:
	Stream_Release(s);
	return ret;
}