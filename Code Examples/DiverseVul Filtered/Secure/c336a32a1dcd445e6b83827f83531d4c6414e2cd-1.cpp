static void WAddLineToHist(Window *wp, struct mline *ml)
{
	uint32_t *q, *o;
	struct mline *hml;

	if (wp->w_histheight == 0)
		return;
	hml = &wp->w_hlines[wp->w_histidx];
	q = ml->image;
	ml->image = hml->image;
	hml->image = q;

	q = ml->attr;
	o = hml->attr;
	hml->attr = q;
	ml->attr = null;
	if (o != null)
		free(o);
	q = ml->font;
	o = hml->font;
	hml->font = q;
	ml->font = null;
	if (o != null)
		free(o);
	q = ml->fontx;
	o = hml->fontx;
	hml->fontx = q;
	ml->fontx = null;
	if (o != null)
		free(o);
	q = ml->colorbg;
	o = hml->colorbg;
	hml->colorbg = q;
	ml->colorbg = null;
	if (o != null)
		free(o);
	q = ml->colorfg;
	o = hml->colorfg;
	hml->colorfg = q;
	ml->colorfg = null;
	if (o != null)
		free(o);

	if (++wp->w_histidx >= wp->w_histheight)
		wp->w_histidx = 0;
}