gdImagePtr gdImageCreateFromGd2PartCtx (gdIOCtx * in, int srcx, int srcy, int w, int h)
{
	int scx, scy, ecx, ecy, fsx, fsy;
	int nc, ncx, ncy, cs, cx, cy;
	int x, y, ylo, yhi, xlo, xhi;
	int dstart, dpos;
	int i;
	/* 2.0.12: unsigned is correct; fixes problems with color munging. Thanks to Steven Brown. */
	unsigned int ch;
	int vers, fmt;
	t_chunk_info *chunkIdx = NULL;
	unsigned char *chunkBuf = NULL;
	int chunkNum;
	int chunkMax = 0;
	uLongf chunkLen;
	int chunkPos = 0;
	int compMax;
	char *compBuf = NULL;

	gdImagePtr im;

	if (w<1 || h <1) {
		return 0;
	}

	/* The next few lines are basically copied from gd2CreateFromFile
	 * we change the file size, so don't want to use the code directly.
	 * but we do need to know the file size.
	 */
	if (_gd2GetHeader(in, &fsx, &fsy, &cs, &vers, &fmt, &ncx, &ncy, &chunkIdx) != 1) {
		goto fail1;
	}

	GD2_DBG(php_gd_error("File size is %dx%d", fsx, fsy));

	/* This is the difference - make a file based on size of chunks. */
	if (gd2_truecolor(fmt)) {
		im = gdImageCreateTrueColor(w, h);
	} else {
		im = gdImageCreate(w, h);
	}
	if (im == NULL) {
		goto fail1;
	}

	if (!_gdGetColors(in, im, vers == 2)) {
		goto fail2;
	}
	GD2_DBG(php_gd_error("Image palette completed: %d colours", im->colorsTotal));

	/* Process the header info */
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

		if (im->trueColor) {
			chunkMax = cs * cs * 4;
		} else {
			chunkMax = cs * cs;
		}
		if (chunkMax <= 0) {
			goto fail2;
		}

		chunkBuf = gdCalloc(chunkMax, 1);
		compBuf = gdCalloc(compMax, 1);
	}

	/* Work out start/end chunks */
	scx = srcx / cs;
	scy = srcy / cs;
	if (scx < 0) {
		scx = 0;
	}
	if (scy < 0) {
		scy = 0;
	}

	ecx = (srcx + w) / cs;
	ecy = (srcy + h) / cs;
	if (ecx >= ncx) {
		ecx = ncx - 1;
	}
	if (ecy >= ncy) {
		ecy = ncy - 1;
	}

	/* Remember file position of image data. */
	dstart = gdTell(in);
	GD2_DBG(php_gd_error("Data starts at %d", dstart));

	/* Loop through the chunks. */
	for (cy = scy; (cy <= ecy); cy++) {
		ylo = cy * cs;
		yhi = ylo + cs;
		if (yhi > fsy) {
			yhi = fsy;
		}

		for (cx = scx; cx <= ecx; cx++) {

			xlo = cx * cs;
			xhi = xlo + cs;
			if (xhi > fsx) {
				xhi = fsx;
			}

			GD2_DBG(php_gd_error("Processing Chunk (%d, %d), from %d to %d", cx, cy, ylo, yhi));

			if (!gd2_compressed(fmt)) {
				GD2_DBG(php_gd_error("Using raw format data"));
				if (im->trueColor) {
					dpos = (cy * (cs * fsx) * 4 + cx * cs * (yhi - ylo) * 4) + dstart;
				} else {
					dpos = cy * (cs * fsx) + cx * cs * (yhi - ylo) + dstart;
				}

				/* gd 2.0.11: gdSeek returns TRUE on success, not 0. Longstanding bug. 01/16/03 */
				if (!gdSeek(in, dpos)) {
					php_gd_error_ex(E_WARNING, "Error from seek: %d", errno);
					goto fail2;
				}
				GD2_DBG(php_gd_error("Reading (%d, %d) from position %d", cx, cy, dpos - dstart));
			} else {
				chunkNum = cx + cy * ncx;

				chunkLen = chunkMax;
				if (!_gd2ReadChunk (chunkIdx[chunkNum].offset, compBuf, chunkIdx[chunkNum].size, (char *)chunkBuf, &chunkLen, in)) {
					php_gd_error("Error reading comproessed chunk");
					goto fail2;
				}
				chunkPos = 0;
				GD2_DBG(php_gd_error("Reading (%d, %d) from chunk %d", cx, cy, chunkNum));
			}

			GD2_DBG(php_gd_error("   into (%d, %d) - (%d, %d)", xlo, ylo, xhi, yhi));

			for (y = ylo; (y < yhi); y++) {
				for (x = xlo; x < xhi; x++) {
					if (!gd2_compressed(fmt)) {
						if (im->trueColor) {
							if (!gdGetInt((int *)&ch, in)) {
								ch = 0;
							}
						} else {
							ch = gdGetC(in);
							if ((int)ch == EOF) {
								ch = 0;
							}
						}
					} else {
						if (im->trueColor) {
							ch = chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
							ch = (ch << 8) + chunkBuf[chunkPos++];
						} else {
							ch = chunkBuf[chunkPos++];
						}
					}

					/* Only use a point that is in the image. */
					if ((x >= srcx) && (x < (srcx + w)) && (x < fsx) && (x >= 0) && (y >= srcy) && (y < (srcy + h)) && (y < fsy) && (y >= 0)) {
						if (im->trueColor) {
							im->tpixels[y - srcy][x - srcx] = ch;
						} else {
							im->pixels[y - srcy][x - srcx] = ch;
						}
					}
				}
			}
		}
	}

	if (chunkBuf) {
		gdFree(chunkBuf);
	}
	if (compBuf) {
		gdFree(compBuf);
	}
	if (chunkIdx) {
		gdFree(chunkIdx);
	}

	return im;

fail2:
	gdImageDestroy(im);
fail1:
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