static BOOL update_send_surface_frame_bits(rdpContext* context, const SURFACE_BITS_COMMAND* cmd,
                                           BOOL first, BOOL last, UINT32 frameId)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	update_force_flush(context);
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (first)
	{
		if (!update_write_surfcmd_frame_marker(s, SURFACECMD_FRAMEACTION_BEGIN, frameId))
			goto out_fail;
	}

	if (!update_write_surfcmd_surface_bits(s, cmd))
		goto out_fail;

	if (last)
	{
		if (!update_write_surfcmd_frame_marker(s, SURFACECMD_FRAMEACTION_END, frameId))
			goto out_fail;
	}

	ret = fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_SURFCMDS, s,
	                               cmd->skipCompression);
	update_force_flush(context);
out_fail:
	Stream_Release(s);
	return ret;
}