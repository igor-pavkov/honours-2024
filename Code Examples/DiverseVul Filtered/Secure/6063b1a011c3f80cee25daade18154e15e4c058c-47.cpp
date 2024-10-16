GF_Err traf_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	//Header first
	if (ptr->tfhd) {
		e = gf_isom_box_write((GF_Box *) ptr->tfhd, bs);
		if (e) return e;
	}
	if (ptr->sub_samples) {
		e = gf_isom_box_array_write(s, ptr->sub_samples, bs);
		if (e) return e;
	}
	if (ptr->tfdt) {
		e = gf_isom_box_write((GF_Box *) ptr->tfdt, bs);
		if (e) return e;
	}
	if (ptr->sampleGroupsDescription) {
		e = gf_isom_box_array_write(s, ptr->sampleGroupsDescription, bs);
		if (e) return e;
	}
	if (ptr->sampleGroups) {
		e = gf_isom_box_array_write(s, ptr->sampleGroups, bs);
		if (e) return e;
	}
	if (ptr->sai_sizes) {
		e = gf_isom_box_array_write(s, ptr->sai_sizes, bs);
		if (e) return e;
	}
	if (ptr->sai_offsets) {
		e = gf_isom_box_array_write(s, ptr->sai_offsets, bs);
		if (e) return e;
	}

	if (ptr->sample_encryption) {
		e = gf_isom_box_write((GF_Box *) ptr->sample_encryption, bs);
		if (e) return e;
	}

	e = gf_isom_box_array_write(s, ptr->TrackRuns, bs);
	if (e) return e;

	//when sdtp is present (smooth-like) write it after the trun box
	if (ptr->sdtp) {
		e = gf_isom_box_write((GF_Box *) ptr->sdtp, bs);
		if (e) return e;
	}
	//tfxd should be last ...
	if (ptr->tfxd) {
		e = gf_isom_box_write((GF_Box *) ptr->tfxd, bs);
		if (e) return e;
	}
	return GF_OK;
}