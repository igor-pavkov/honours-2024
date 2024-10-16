static void _gdImageGd2 (gdImagePtr im, gdIOCtx * out, int cs, int fmt)
{
	int ncx, ncy, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int chunkLen;
	int chunkNum = 0;
	char *chunkData = NULL;	/* So we can gdFree it with impunity. */
	char *compData = NULL;	/* So we can gdFree it with impunity. */
	uLongf compLen;
	int idxPos = 0;
	int idxSize;
	t_chunk_info *chunkIdx = NULL; /* So we can gdFree it with impunity. */
	int posSave;
	int bytesPerPixel = im->trueColor ? 4 : 1;
	int compMax = 0;

	/* Force fmt to a valid value since we don't return anything. */
	if ((fmt != GD2_FMT_RAW) && (fmt != GD2_FMT_COMPRESSED)) {
		fmt = im->trueColor ? GD2_FMT_TRUECOLOR_COMPRESSED : GD2_FMT_COMPRESSED;
	}
	if (im->trueColor) {
		fmt += 2;
	}
	/* Make sure chunk size is valid. These are arbitrary values; 64 because it seems
	 * a little silly to expect performance improvements on a 64x64 bit scale, and
	 * 4096 because we buffer one chunk, and a 16MB buffer seems a little large - it may be
	 * OK for one user, but for another to read it, they require the buffer.
	 */
	if (cs == 0) {
		cs = GD2_CHUNKSIZE;
	} else if (cs < GD2_CHUNKSIZE_MIN) {
		cs = GD2_CHUNKSIZE_MIN;
	} else if (cs > GD2_CHUNKSIZE_MAX) {
		cs = GD2_CHUNKSIZE_MAX;
	}

	/* Work out number of chunks. */
	ncx = im->sx / cs + 1;
	ncy = im->sy / cs + 1;

	/* Write the standard header. */
	_gd2PutHeader (im, out, cs, fmt, ncx, ncy);

	if (gd2_compressed(fmt)) {
		/* Work out size of buffer for compressed data, If CHUNKSIZE is large,
	 	 * then these will be large!
	 	 */

		/* The zlib notes say output buffer size should be (input size) * 1.01 * 12
		 * - we'll use 1.02 to be paranoid.
		 */
		compMax = (int)(cs * bytesPerPixel * cs * 1.02f) + 12;

		/* Allocate the buffers.  */
		chunkData = safe_emalloc(cs * bytesPerPixel, cs, 0);
		memset(chunkData, 0, cs * bytesPerPixel * cs);
		if (compMax <= 0) {
			goto fail;
		}
		compData = gdCalloc(compMax, 1);

		/* Save the file position of chunk index, and allocate enough space for
		 * each chunk_info block .
		 */
		idxPos = gdTell(out);
		idxSize = ncx * ncy * sizeof(t_chunk_info);
		GD2_DBG(php_gd_error("Index size is %d", idxSize));
		gdSeek(out, idxPos + idxSize);

		chunkIdx = safe_emalloc(idxSize, sizeof(t_chunk_info), 0);
		memset(chunkIdx, 0, idxSize * sizeof(t_chunk_info));
	}

	_gdPutColors (im, out);

	GD2_DBG(php_gd_error("Size: %dx%d", im->sx, im->sy));
	GD2_DBG(php_gd_error("Chunks: %dx%d", ncx, ncy));

	for (cy = 0; (cy < ncy); cy++) {
		for (cx = 0; (cx < ncx); cx++) {
			ylo = cy * cs;
			yhi = ylo + cs;
			if (yhi > im->sy) {
				yhi = im->sy;
			}

			GD2_DBG(php_gd_error("Processing Chunk (%dx%d), y from %d to %d", cx, cy, ylo, yhi));
			chunkLen = 0;
			for (y = ylo; (y < yhi); y++) {
				GD2_DBG(php_gd_error("y=%d: ",y));
				xlo = cx * cs;
				xhi = xlo + cs;
				if (xhi > im->sx) {
					xhi = im->sx;
				}

				if (gd2_compressed(fmt)) {
					for (x = xlo; x < xhi; x++) {
						GD2_DBG(php_gd_error("%d...",x));
						if (im->trueColor) {
							int p = im->tpixels[y][x];
							chunkData[chunkLen++] = gdTrueColorGetAlpha(p);
							chunkData[chunkLen++] = gdTrueColorGetRed(p);
							chunkData[chunkLen++] = gdTrueColorGetGreen(p);
							chunkData[chunkLen++] = gdTrueColorGetBlue(p);
						} else {
							chunkData[chunkLen++] = im->pixels[y][x];
						}
					}
				} else {
					for (x = xlo; x < xhi; x++) {
						GD2_DBG(php_gd_error("%d, ",x));

						if (im->trueColor) {
							gdPutInt(im->tpixels[y][x], out);
						} else {
							gdPutC((unsigned char) im->pixels[y][x], out);
						}
					}
				}
				GD2_DBG(php_gd_error("y=%d done.",y));
			}

			if (gd2_compressed(fmt)) {
				compLen = compMax;
				if (compress((unsigned char *) &compData[0], &compLen, (unsigned char *) &chunkData[0], chunkLen) != Z_OK) {
					php_gd_error("Error from compressing");
				} else {
					chunkIdx[chunkNum].offset = gdTell(out);
					chunkIdx[chunkNum++].size = compLen;
					GD2_DBG(php_gd_error("Chunk %d size %d offset %d", chunkNum, chunkIdx[chunkNum - 1].size, chunkIdx[chunkNum - 1].offset));

					if (gdPutBuf (compData, compLen, out) <= 0) {
						/* Any alternate suggestions for handling this? */
						php_gd_error_ex(E_WARNING, "Error %d on write", errno);
					}
				}
			}
		}
    	}

	if (gd2_compressed(fmt)) {
		/* Save the position, write the index, restore position (paranoia). */
		GD2_DBG(php_gd_error("Seeking %d to write index", idxPos));
		posSave = gdTell(out);
		gdSeek(out, idxPos);
		GD2_DBG(php_gd_error("Writing index"));
		for (x = 0; x < chunkNum; x++) {
			GD2_DBG(php_gd_error("Chunk %d size %d offset %d", x, chunkIdx[x].size, chunkIdx[x].offset));
			gdPutInt(chunkIdx[x].offset, out);
			gdPutInt(chunkIdx[x].size, out);
		}
		gdSeek(out, posSave);
	}
fail:
	GD2_DBG(php_gd_error("Freeing memory"));
	if (chunkData) {
		gdFree(chunkData);
	}
	if (compData) {
		gdFree(compData);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}
	GD2_DBG(php_gd_error("Done"));
}