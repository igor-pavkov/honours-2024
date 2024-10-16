static int jpc_dec_process_siz(jpc_dec_t *dec, jpc_ms_t *ms)
{
	jpc_siz_t *siz = &ms->parms.siz;
	int compno;
	int tileno;
	jpc_dec_tile_t *tile;
	jpc_dec_tcomp_t *tcomp;
	int htileno;
	int vtileno;
	jpc_dec_cmpt_t *cmpt;
	size_t size;
	size_t num_samples;
	size_t num_samples_delta;

	size_t tile_samples;
	if (!jas_safe_size_mul(siz->tilewidth, siz->tileheight, &tile_samples) ||
	    (dec->max_samples > 0 && tile_samples > dec->max_samples)) {
		jas_eprintf("tile too large\n");
		return -1;
	}

	dec->xstart = siz->xoff;
	dec->ystart = siz->yoff;
	dec->xend = siz->width;
	dec->yend = siz->height;
	dec->tilewidth = siz->tilewidth;
	dec->tileheight = siz->tileheight;
	dec->tilexoff = siz->tilexoff;
	dec->tileyoff = siz->tileyoff;
	dec->numcomps = siz->numcomps;

	if (!(dec->cp = jpc_dec_cp_create(dec->numcomps))) {
		return -1;
	}

	if (!(dec->cmpts = jas_alloc2(dec->numcomps, sizeof(jpc_dec_cmpt_t)))) {
		return -1;
	}

	num_samples = 0;
	for (compno = 0, cmpt = dec->cmpts; compno < dec->numcomps; ++compno,
	  ++cmpt) {
		cmpt->prec = siz->comps[compno].prec;
		cmpt->sgnd = siz->comps[compno].sgnd;
		cmpt->hstep = siz->comps[compno].hsamp;
		cmpt->vstep = siz->comps[compno].vsamp;
		cmpt->width = JPC_CEILDIV(dec->xend, cmpt->hstep) -
		  JPC_CEILDIV(dec->xstart, cmpt->hstep);
		cmpt->height = JPC_CEILDIV(dec->yend, cmpt->vstep) -
		  JPC_CEILDIV(dec->ystart, cmpt->vstep);
		cmpt->hsubstep = 0;
		cmpt->vsubstep = 0;

		if (!jas_safe_size_mul(cmpt->width, cmpt->height, &num_samples_delta)) {
			jas_eprintf("image too large\n");
			return -1;
		}
		if (!jas_safe_size_add(num_samples, num_samples_delta, &num_samples)) {
			jas_eprintf("image too large\n");
			return -1;
		}
	}

	if (dec->max_samples > 0 && num_samples > dec->max_samples) {
		jas_eprintf("maximum number of samples exceeded (%zu > %zu)\n",
		  num_samples, dec->max_samples);
		return -1;
	}

	dec->image = 0;

	dec->numhtiles = JPC_CEILDIV(dec->xend - dec->tilexoff, dec->tilewidth);
	dec->numvtiles = JPC_CEILDIV(dec->yend - dec->tileyoff, dec->tileheight);
	assert(dec->numhtiles >= 0);
	assert(dec->numvtiles >= 0);
	if (!jas_safe_size_mul(dec->numhtiles, dec->numvtiles, &size) ||
	  size > INT_MAX) {
		return -1;
	}
	if (dec->max_samples > 0 && size > dec->max_samples / 16 / 16) {
		/* avoid Denial of Service by a malicious input file
		   with millions of tiny tiles; if max_samples is
		   configured, then assume the tiles are at least
		   16x16, and don't allow more than this number of
		   tiles */
		return -1;
	}
	if (dec->max_samples > 0 && size > dec->max_samples / dec->numcomps / 16) {
		/* another DoS check: since each tile allocates an
		   array of components, this check attempts to catch
		   excessive tile*component numbers */
		return -1;
	}
	dec->numtiles = size;
	JAS_DBGLOG(10, ("numtiles = %d; numhtiles = %d; numvtiles = %d;\n",
	  dec->numtiles, dec->numhtiles, dec->numvtiles));
	if (!(dec->tiles = jas_alloc2(dec->numtiles, sizeof(jpc_dec_tile_t)))) {
		return -1;
	}

	for (tileno = 0, tile = dec->tiles; tileno < dec->numtiles; ++tileno,
	  ++tile) {
		/* initialize all tiles with JPC_TILE_DONE so
		   jpc_dec_destroy() knows which ones need a
		   jpc_dec_tilefini() call; they are not actually
		   "done", of course */
		tile->state = JPC_TILE_DONE;
	}

	for (tileno = 0, tile = dec->tiles; tileno < dec->numtiles; ++tileno,
	  ++tile) {
		htileno = tileno % dec->numhtiles;
		vtileno = tileno / dec->numhtiles;
		tile->realmode = 0;
		tile->state = JPC_TILE_INIT;
		tile->xstart = JAS_MAX(dec->tilexoff + htileno * dec->tilewidth,
		  dec->xstart);
		tile->ystart = JAS_MAX(dec->tileyoff + vtileno * dec->tileheight,
		  dec->ystart);
		tile->xend = JAS_MIN(dec->tilexoff + (htileno + 1) *
		  dec->tilewidth, dec->xend);
		tile->yend = JAS_MIN(dec->tileyoff + (vtileno + 1) *
		  dec->tileheight, dec->yend);
		tile->numparts = 0;
		tile->partno = 0;
		tile->pkthdrstream = 0;
		tile->pkthdrstreampos = 0;
		tile->pptstab = 0;
		tile->cp = 0;
		tile->pi = 0;
		if (!(tile->tcomps = jas_alloc2(dec->numcomps,
		  sizeof(jpc_dec_tcomp_t)))) {
			return -1;
		}
		for (compno = 0, cmpt = dec->cmpts, tcomp = tile->tcomps;
		  compno < dec->numcomps; ++compno, ++cmpt, ++tcomp) {
			tcomp->rlvls = 0;
			tcomp->numrlvls = 0;
			tcomp->data = 0;
			tcomp->xstart = JPC_CEILDIV(tile->xstart, cmpt->hstep);
			tcomp->ystart = JPC_CEILDIV(tile->ystart, cmpt->vstep);
			tcomp->xend = JPC_CEILDIV(tile->xend, cmpt->hstep);
			tcomp->yend = JPC_CEILDIV(tile->yend, cmpt->vstep);
			tcomp->tsfb = 0;
		}
	}

	dec->pkthdrstreams = 0;

	/* We should expect to encounter other main header marker segments
	  or an SOT marker segment next. */
	dec->state = JPC_MH;

	return 0;
}