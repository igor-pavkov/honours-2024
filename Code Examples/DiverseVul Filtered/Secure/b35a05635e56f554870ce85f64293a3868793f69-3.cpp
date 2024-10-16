int jas_image_addcmpt(jas_image_t *image, int cmptno,
  jas_image_cmptparm_t *cmptparm)
{
	jas_image_cmpt_t *newcmpt;
	if (cmptno < 0) {
		cmptno = image->numcmpts_;
	}
	assert(cmptno >= 0 && cmptno <= image->numcmpts_);
	if (image->numcmpts_ >= image->maxcmpts_) {
		if (jas_image_growcmpts(image, image->maxcmpts_ + 128)) {
			return -1;
		}
	}
	if (!(newcmpt = jas_image_cmpt_create(cmptparm->tlx,
	  cmptparm->tly, cmptparm->hstep, cmptparm->vstep,
	  cmptparm->width, cmptparm->height, cmptparm->prec,
	  cmptparm->sgnd, 1))) {
		return -1;
	}
	if (cmptno < image->numcmpts_) {
		memmove(&image->cmpts_[cmptno + 1], &image->cmpts_[cmptno],
		  (image->numcmpts_ - cmptno) * sizeof(jas_image_cmpt_t *));
	}
	image->cmpts_[cmptno] = newcmpt;
	++image->numcmpts_;

	jas_image_setbbox(image);

	return 0;
}