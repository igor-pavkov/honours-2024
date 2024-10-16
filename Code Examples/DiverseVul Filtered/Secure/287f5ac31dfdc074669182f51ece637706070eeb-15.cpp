static void do_read_items(deark *c, lctx *d, i64 pos)
{
	i64 opcode;
	i64 opcode_pos;
	i64 bytes_used;
	int ret;

	while(1) {
		if(pos%2 && d->version==2) {
			pos++; // 2-byte alignment
		}

		if(pos >= c->infile->len) break;

		opcode_pos = pos;

		if(d->version==2) {
			opcode = de_getu16be(pos);
			pos+=2;
		}
		else {
			opcode = (i64)de_getbyte(pos);
			pos+=1;
		}

		ret = do_handle_item(c, d, opcode_pos, opcode, pos, &bytes_used);
		if(!ret) goto done;
		if(opcode==0x00ff) goto done; // End of image

		pos += bytes_used;
	}
done:
	;
}