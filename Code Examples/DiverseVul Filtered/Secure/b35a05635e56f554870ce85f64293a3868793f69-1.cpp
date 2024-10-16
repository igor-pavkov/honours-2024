int jas_image_copycmpt(jas_image_t *dstimage, int dstcmptno,
  jas_image_t *srcimage, int srccmptno)
{
	jas_image_cmpt_t *newcmpt;
	if (dstimage->numcmpts_ >= dstimage->maxcmpts_) {
		if (jas_image_growcmpts(dstimage, dstimage->maxcmpts_ + 128)) {
			return -1;
		}
	}
	if (!(newcmpt = jas_image_cmpt_copy(srcimage->cmpts_[srccmptno]))) {
		return -1;
	}
	if (dstcmptno < dstimage->numcmpts_) {
		memmove(&dstimage->cmpts_[dstcmptno + 1], &dstimage->cmpts_[dstcmptno],
		  (dstimage->numcmpts_ - dstcmptno) * sizeof(jas_image_cmpt_t *));
	}
	dstimage->cmpts_[dstcmptno] = newcmpt;
	++dstimage->numcmpts_;

	jas_image_setbbox(dstimage);
	return 0;
}