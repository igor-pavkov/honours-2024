static jpc_dec_t *jpc_dec_create(jpc_dec_importopts_t *impopts, jas_stream_t *in)
{
	jpc_dec_t *dec;

	if (!(dec = jas_malloc(sizeof(jpc_dec_t)))) {
		return 0;
	}

	dec->image = 0;
	dec->xstart = 0;
	dec->ystart = 0;
	dec->xend = 0;
	dec->yend = 0;
	dec->tilewidth = 0;
	dec->tileheight = 0;
	dec->tilexoff = 0;
	dec->tileyoff = 0;
	dec->numhtiles = 0;
	dec->numvtiles = 0;
	dec->numtiles = 0;
	dec->tiles = 0;
	dec->curtile = 0;
	dec->numcomps = 0;
	dec->in = in;
	dec->cp = 0;
	dec->maxlyrs = impopts->maxlyrs;
	dec->maxpkts = impopts->maxpkts;
dec->numpkts = 0;
	dec->ppmseqno = 0;
	dec->state = 0;
	dec->cmpts = 0;
	dec->pkthdrstreams = 0;
	dec->ppmstab = 0;
	dec->curtileendoff = 0;
	dec->max_samples = impopts->max_samples;

	return dec;
}