static int handler_QuickTime(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 payload_pos;
	i64 payload_len;
	i64 endpos;
	i64 idsc_pos;

	payload_len = de_getu32be(data_pos);
	payload_pos = data_pos+4;
	de_dbg(c, "payload: pos=%"I64_FMT", len=%"I64_FMT, payload_pos, payload_len);
	endpos = payload_pos+payload_len;
	if(endpos > c->infile->len) return 0;
	*bytes_used = 4+payload_len;

	// Following the size field seems to be 68 or 50 bytes of data,
	// followed by QuickTime "idsc" data, followed by image data.
	idsc_pos = payload_pos + ((opcode==0x8201) ? 50 : 68);

	do_handle_qtif_idsc(c, d, idsc_pos, endpos-idsc_pos);
	return 1;
}