static void do_handle_qtif_idsc(deark *c, lctx *d, i64 pos, i64 len)
{
	i64 idsc_dpos, idsc_dlen;
	i64 idat_dpos, idat_dlen;
	dbuf *outf = NULL;
	struct de_fourcc cmpr4cc;

	if(d->decode_qtif) {
		de_run_module_by_id_on_slice2(c, "qtif", "I", c->infile, pos, len);
		return;
	}

	// Try to construct a .qtif file.
	// This way, we do something potentially useful even if the image has
	// a compression scheme that our qtif module doesn't support.

	idsc_dpos = pos;
	idsc_dlen = de_getu32be(idsc_dpos);
	de_dbg(c, "idsc: pos=%"I64_FMT", len=%"I64_FMT, idsc_dpos, idsc_dlen);
	if(idsc_dpos+idsc_dlen > pos+len) goto done;

	dbuf_read_fourcc(c->infile, idsc_dpos+4, &cmpr4cc, 4, 0x0);
	de_dbg(c, "compression type: \"%s\"", cmpr4cc.id_dbgstr);

	idat_dpos = idsc_dpos + idsc_dlen;
	idat_dlen = de_getu32be(idsc_dpos+44);
	de_dbg(c, "idat: pos=%"I64_FMT", len=%"I64_FMT, idat_dpos, idat_dlen);
	if(idat_dlen==0) {
		idat_dlen = pos+len-idat_dpos; // ??
	}
	if(idat_dpos+idat_dlen > pos+len) goto done;

#define CODE_idat 0x69646174U
#define CODE_idsc 0x69647363U
	outf = dbuf_create_output_file(c, "qtif", NULL, 0);

	dbuf_writeu32be(outf, 8+idsc_dlen);
	dbuf_writeu32be(outf, CODE_idsc);
	dbuf_copy(c->infile, idsc_dpos, idsc_dlen, outf);

	dbuf_writeu32be(outf, 8+idat_dlen);
	dbuf_writeu32be(outf, CODE_idat);
	dbuf_copy(c->infile, idat_dpos, idat_dlen, outf);

done:
	dbuf_close(outf);
}