jas_image_t *jas_image_copy(jas_image_t *image)
{
	jas_image_t *newimage;
	int cmptno;

	if (!(newimage = jas_image_create0())) {
		goto error;
	}

	if (jas_image_growcmpts(newimage, image->numcmpts_)) {
		goto error;
	}
	for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno) {
		if (!(newimage->cmpts_[cmptno] = jas_image_cmpt_copy(image->cmpts_[cmptno]))) {
			goto error;
		}
		++newimage->numcmpts_;
	}

	jas_image_setbbox(newimage);

	if (image->cmprof_) {
		if (!(newimage->cmprof_ = jas_cmprof_copy(image->cmprof_)))
			goto error;
	}

	return newimage;
error:
	if (newimage) {
		jas_image_destroy(newimage);
	}
	return 0;
}