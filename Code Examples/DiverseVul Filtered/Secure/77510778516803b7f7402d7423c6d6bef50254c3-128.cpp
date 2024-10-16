GF_Err saio_box_size(GF_Box *s)
{
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox*)s;

	if (ptr->aux_info_type || ptr->aux_info_type_parameter) {
		ptr->flags |= 1;
	}

	if (ptr->flags & 1) ptr->size += 8;
	ptr->size += 4;
	//a little optim here: in cenc, the saio always points to a single data block, only one entry is needed
	switch (ptr->aux_info_type) {
	case GF_ISOM_CENC_SCHEME:
	case GF_ISOM_CBC_SCHEME:
	case GF_ISOM_CENS_SCHEME:
	case GF_ISOM_CBCS_SCHEME:
		if (ptr->offsets) gf_free(ptr->offsets);
		ptr->offsets = NULL;
		ptr->entry_alloc = 0;
		ptr->entry_count = 1;
		break;
	}

	ptr->size += ((ptr->version==1) ? 8 : 4) * ptr->entry_count;
	return GF_OK;
}