static jas_image_cmpt_t *jas_image_cmpt_copy(jas_image_cmpt_t *cmpt)
{
	jas_image_cmpt_t *newcmpt;

	if (!(newcmpt = jas_image_cmpt_create0())) {
		return 0;
	}
	newcmpt->tlx_ = cmpt->tlx_;
	newcmpt->tly_ = cmpt->tly_;
	newcmpt->hstep_ = cmpt->hstep_;
	newcmpt->vstep_ = cmpt->vstep_;
	newcmpt->width_ = cmpt->width_;
	newcmpt->height_ = cmpt->height_;
	newcmpt->prec_ = cmpt->prec_;
	newcmpt->sgnd_ = cmpt->sgnd_;
	newcmpt->cps_ = cmpt->cps_;
	newcmpt->type_ = cmpt->type_;
	if (!(newcmpt->stream_ = jas_stream_memopen(0, 0))) {
		goto error;
	}
	if (jas_stream_seek(cmpt->stream_, 0, SEEK_SET)) {
		goto error;
	}
	if (jas_stream_copy(newcmpt->stream_, cmpt->stream_, -1)) {
		goto error;
	}
	if (jas_stream_seek(newcmpt->stream_, 0, SEEK_SET)) {
		goto error;
	}
	return newcmpt;
error:
	if (newcmpt) {
		jas_image_cmpt_destroy(newcmpt);
	}
	return 0;
}