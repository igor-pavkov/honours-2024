static BOOL update_send_surface_command(rdpContext* context, wStream* s)
{
	wStream* update;
	rdpRdp* rdp = context->rdp;
	BOOL ret;
	update = fastpath_update_pdu_init(rdp->fastpath);

	if (!update)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(update, Stream_GetPosition(s)))
	{
		ret = FALSE;
		goto out;
	}

	Stream_Write(update, Stream_Buffer(s), Stream_GetPosition(s));
	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_SURFCMDS, update, FALSE);
out:
	Stream_Release(update);
	return ret;
}