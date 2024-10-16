gdImagePtr gdImageCreateFromGd2Ctx (gdIOCtxPtr in)
{
	int sx, sy;
	int i;
	int ncx, ncy, nc, cs, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int vers, fmt;
	t_chunk_info *chunkIdx = NULL;	/* So we can gdFree it with impunity. */
	unsigned char *chunkBuf = NULL;	/* So we can gdFree it with impunity. */
	int chunkNum = 0;
	int chunkMax = 0;
	uLongf chunkLen;
	int chunkPos = 0;
	int compMax = 0;
	int bytesPerPixel;
	char *compBuf = NULL;		/* So we can gdFree it with impunity. */

	gdImagePtr im;

	/* Get the header */
	if (!(im = _gd2CreateFromFile(in, &sx, &sy, &cs, &vers, &fmt, &ncx, &ncy, &chunkIdx))) {
		 return 0;
	}

	bytesPerPixel = im->trueColor ? 4 : 1;
	nc = ncx * ncy;

	if (gd2_compressed(fmt)) {
		/* Find the maximum compressed chunk size. */
		compMax = 0;
		for (i = 0; (i < nc); i++) {
			if (chunkIdx[i].size > compMax) {
				compMax = chunkIdx[i].size;
			}
		}
		compMax++;

		/* Allocate buffers */
		chunkMax = cs * bytesPerPixel * cs;
		if (chunkMax <= 0) {
			return 0;
		}
		chunkBuf = gdCalloc(chunkMax, 1);
		compBuf = gdCalloc(compMax, 1);

		GD2_DBG(php_gd_error("Largest compressed chunk is %d bytes", compMax));
	}

	/* Read the data... */
	for (cy = 0; (cy < ncy); cy++) {
		for (cx = 0; (cx < ncx); cx++) {
			ylo = cy * cs;
			yhi = ylo + cs;
			if (yhi > im->sy) {
				yhi = im->sy;
			}

			GD2_DBG(php_gd_error("Processing Chunk %d (%d, %d), y from %d to %d", chunkNum, cx, cy, ylo, yhi));

			if (gd2_compressed(fmt)) {
				chunkLen = chunkMax;

				if (!_gd2ReadChunk(chunkIdx[chunkNum].offset, compBuf, chunkIdx[chunkNum].size, (char *) chunkBuf, &chunkLen, in)) {
					GD2_DBG(php_gd_error("Error reading comproessed chunk"));
					goto fail2;
				}

				chunkPos = 0;
			}

			for (y = ylo; (y < yhi); y++) {
				xlo = cx * cs;
				xhi = xlo + cs;
				if (xhi > im->sx) {
					xhi = im->sx;
				}

				if (!gd2_compressed(fmt)) {
					for (x = xlo; x < xhi; x++) {
						if (im->trueColor) {
							if (!gdGetInt(&im->tpixels[y][x], in)) {
								im->tpixels[y][x] = 0;
							}
						} else {
							int ch;
							if (!gdGetByte(&ch, in)) {
								ch = 0;
							}
							im->pixels[y][x] = ch;
						}
					}
				} else {
					for (x = xlo; x < xhi; x++) {
						if (im->trueColor) {
							/* 2.0.1: work around a gcc bug by being verbose. TBB */
							int a = chunkBuf[chunkPos++] << 24;
							int r = chunkBuf[chunkPos++] << 16;
							int g = chunkBuf[chunkPos++] << 8;
							int b = chunkBuf[chunkPos++];
							im->tpixels[y][x] = a + r + g + b;
						} else {
							im->pixels[y][x] = chunkBuf[chunkPos++];
						}
					}
				}
			}
			chunkNum++;
		}
	}

	GD2_DBG(php_gd_error("Freeing memory"));

	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	GD2_DBG(php_gd_error("Done"));

	return im;

fail2:
	gdImageDestroy(im);
	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	return 0;
}