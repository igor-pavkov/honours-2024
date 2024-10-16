static int h2_frt_transfer_data(struct h2s *h2s)
{
	struct h2c *h2c = h2s->h2c;
	int block1, block2;
	unsigned int flen = 0;
	unsigned int chklen = 0;
	struct htx *htx = NULL;
	struct buffer *csbuf;

	h2c->flags &= ~H2_CF_DEM_SFULL;

	csbuf = h2_get_buf(h2c, &h2s->rxbuf);
	if (!csbuf) {
		h2c->flags |= H2_CF_DEM_SALLOC;
		goto fail;
	}

try_again:
	flen = h2c->dfl - h2c->dpl;
	if (h2c->proxy->options2 & PR_O2_USE_HTX)
		htx = htx_from_buf(csbuf);
	if (!flen)
		goto end_transfer;

	if (flen > b_data(&h2c->dbuf)) {
		flen = b_data(&h2c->dbuf);
		if (!flen)
			goto fail;
	}

	if (h2c->proxy->options2 & PR_O2_USE_HTX) {
		block1 = htx_free_data_space(htx);
		if (!block1) {
			h2c->flags |= H2_CF_DEM_SFULL;
			goto fail;
		}
		if (flen > block1)
			flen = block1;

		/* here, flen is the max we can copy into the output buffer */
		block1 = b_contig_data(&h2c->dbuf, 0);
		if (flen > block1)
			flen = block1;

		if (!htx_add_data(htx, ist2(b_head(&h2c->dbuf), flen))) {
			h2c->flags |= H2_CF_DEM_SFULL;
			goto fail;
		}

		b_del(&h2c->dbuf, flen);
		h2c->dfl    -= flen;
		h2c->rcvd_c += flen;
		h2c->rcvd_s += flen;  // warning, this can also affect the closed streams!
		goto try_again;
	}
	else if (unlikely(b_space_wraps(csbuf))) {
		/* it doesn't fit and the buffer is fragmented,
		 * so let's defragment it and try again.
		 */
		b_slow_realign(csbuf, trash.area, 0);
	}

	/* chunked-encoding requires more room */
	if (h2s->flags & H2_SF_DATA_CHNK) {
		chklen = MIN(flen, b_room(csbuf));
		chklen = (chklen < 16) ? 1 : (chklen < 256) ? 2 :
			(chklen < 4096) ? 3 : (chklen < 65536) ? 4 :
			(chklen < 1048576) ? 4 : 8;
		chklen += 4; // CRLF, CRLF
	}

	/* does it fit in output buffer or should we wait ? */
	if (flen + chklen > b_room(csbuf)) {
		if (chklen >= b_room(csbuf)) {
			h2c->flags |= H2_CF_DEM_SFULL;
			goto fail;
		}
		flen = b_room(csbuf) - chklen;
	}

	if (h2s->flags & H2_SF_DATA_CHNK) {
		/* emit the chunk size */
		unsigned int chksz = flen;
		char str[10];
		char *beg;

		beg = str + sizeof(str);
		*--beg = '\n';
		*--beg = '\r';
		do {
			*--beg = hextab[chksz & 0xF];
		} while (chksz >>= 4);
		b_putblk(csbuf, beg, str + sizeof(str) - beg);
	}

	/* Block1 is the length of the first block before the buffer wraps,
	 * block2 is the optional second block to reach the end of the frame.
	 */
	block1 = b_contig_data(&h2c->dbuf, 0);
	if (block1 > flen)
		block1 = flen;
	block2 = flen - block1;

	if (block1)
		b_putblk(csbuf, b_head(&h2c->dbuf), block1);

	if (block2)
		b_putblk(csbuf, b_peek(&h2c->dbuf, block1), block2);

	if (h2s->flags & H2_SF_DATA_CHNK) {
		/* emit the CRLF */
		b_putblk(csbuf, "\r\n", 2);
	}

	/* now mark the input data as consumed (will be deleted from the buffer
	 * by the caller when seeing FRAME_A after sending the window update).
	 */
	b_del(&h2c->dbuf, flen);
	h2c->dfl    -= flen;
	h2c->rcvd_c += flen;
	h2c->rcvd_s += flen;  // warning, this can also affect the closed streams!

	if (h2c->dfl > h2c->dpl) {
		/* more data available, transfer stalled on stream full */
		h2c->flags |= H2_CF_DEM_SFULL;
		goto fail;
	}

 end_transfer:
	/* here we're done with the frame, all the payload (except padding) was
	 * transferred.
	 */

	if (h2c->dff & H2_F_DATA_END_STREAM) {
		if (htx) {
			if (!htx_add_endof(htx, HTX_BLK_EOM)) {
				h2c->flags |= H2_CF_DEM_SFULL;
				goto fail;
			}
		}
		else if (h2s->flags & H2_SF_DATA_CHNK) {
			/* emit the trailing 0 CRLF CRLF */
			if (b_room(csbuf) < 5) {
				h2c->flags |= H2_CF_DEM_SFULL;
				goto fail;
			}
			chklen += 5;
			b_putblk(csbuf, "0\r\n\r\n", 5);
		}
	}

	h2c->rcvd_c += h2c->dpl;
	h2c->rcvd_s += h2c->dpl;
	h2c->dpl = 0;
	h2c->st0 = H2_CS_FRAME_A; // send the corresponding window update

	if (h2c->dff & H2_F_DATA_END_STREAM) {
		h2s->flags |= H2_SF_ES_RCVD;
		h2s->cs->flags |= CS_FL_REOS;
	}
	if (htx)
		htx_to_buf(htx, csbuf);
	return 1;
 fail:
	if (htx)
		htx_to_buf(htx, csbuf);
	return 0;
}