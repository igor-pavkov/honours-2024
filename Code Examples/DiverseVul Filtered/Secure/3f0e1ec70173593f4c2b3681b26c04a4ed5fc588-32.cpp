static int h2_frt_transfer_data(struct h2s *h2s, struct buffer *buf, int count)
{
	struct h2c *h2c = h2s->h2c;
	int block1, block2;
	unsigned int flen = h2c->dfl;

	h2s->cs->flags &= ~CS_FL_RCV_MORE;
	h2c->flags &= ~H2_CF_DEM_SFULL;

	/* The padlen is the first byte before data, and the padding appears
	 * after data. padlen+data+padding are included in flen.
	 */
	if (h2c->dff & H2_F_DATA_PADDED) {
		if (h2c->dbuf->i < 1)
			return 0;

		h2c->dpl = *(uint8_t *)bi_ptr(h2c->dbuf);
		if (h2c->dpl >= h2c->dfl) {
			/* RFC7540#6.1 : pad length = length of frame payload or greater */
			h2c_error(h2c, H2_ERR_PROTOCOL_ERROR);
			return 0;
		}

		/* skip the padlen byte */
		bi_del(h2c->dbuf, 1);
		h2c->dfl--;
		h2c->rcvd_c++; h2c->rcvd_s++;
		h2c->dff &= ~H2_F_DATA_PADDED;
	}

	flen = h2c->dfl - h2c->dpl;
	if (!flen)
		goto end_transfer;

	if (flen > h2c->dbuf->i) {
		flen = h2c->dbuf->i;
		if (!flen)
			return 0;
	}

	/* does it fit in output buffer or should we wait ? */
	if (flen > count) {
		flen = count;
		if (!flen) {
			h2c->flags |= H2_CF_DEM_SFULL;
			h2s->cs->flags |= CS_FL_RCV_MORE;
			return 0;
		}
	}

	/* Block1 is the length of the first block before the buffer wraps,
	 * block2 is the optional second block to reach the end of the frame.
	 */
	block1 = bi_contig_data(h2c->dbuf);
	if (block1 > flen)
		block1 = flen;
	block2 = flen - block1;

	if (block1)
		bi_putblk(buf, b_ptr(h2c->dbuf, 0), block1);

	if (block2)
		bi_putblk(buf, b_ptr(h2c->dbuf, block1), block2);

	/* now mark the input data as consumed (will be deleted from the buffer
	 * by the caller when seeing FRAME_A after sending the window update).
	 */
	bi_del(h2c->dbuf, flen);
	h2c->dfl    -= flen;
	h2c->rcvd_c += flen;
	h2c->rcvd_s += flen;  // warning, this can also affect the closed streams!

	if (h2c->dfl > h2c->dpl) {
		/* more data available, transfer stalled on stream full */
		h2c->flags |= H2_CF_DEM_SFULL;
		h2s->cs->flags |= CS_FL_RCV_MORE;
		return flen;
	}

 end_transfer:
	/* here we're done with the frame, all the payload (except padding) was
	 * transferred.
	 */
	h2c->rcvd_c += h2c->dpl;
	h2c->rcvd_s += h2c->dpl;
	h2c->dpl = 0;
	h2c->st0 = H2_CS_FRAME_A; // send the corresponding window update

	/* don't send it before returning data!
	 * FIXME: should we instead try to send it much later, after the
	 * response ? This would require that we keep a copy of it in h2s.
	 */
	if (h2c->dff & H2_F_DATA_END_STREAM) {
		h2s->cs->flags |= CS_FL_EOS;
		h2s->flags |= H2_SF_ES_RCVD;
	}

	return flen;
}