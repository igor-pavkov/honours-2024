GF_Err stsz_box_size(GF_Box *s)
{
	u32 i, fieldSize, size;
	GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;

	ptr->size += 8;
	if (!ptr->sampleCount) return GF_OK;

	//regular table
	if (ptr->type == GF_ISOM_BOX_TYPE_STSZ) {
		if (ptr->sampleSize) return GF_OK;
		ptr->size += (4 * ptr->sampleCount);
		return GF_OK;
	}
	if (!ptr->sizes) return GF_ISOM_INVALID_FILE;

	//compact size table
	fieldSize = 4;
	size = ptr->sizes[0];

	for (i=0; i < ptr->sampleCount; i++) {
		if (ptr->sizes[i] <= 0xF) {
		}
		//switch to 8-bit table
		else if (ptr->sizes[i] <= 0xFF) {
			fieldSize = 8;
		}
		//switch to 16-bit table
		else if (ptr->sizes[i] <= 0xFFFF) {
			fieldSize = 16;
		}
		//switch to 32-bit table
		else {
			fieldSize = 32;
		}

		//check the size
		if (size != ptr->sizes[i]) size = 0;
	}
	//if all samples are of the same size, switch to regular (more compact)
	if (size) {
		ptr->type = GF_ISOM_BOX_TYPE_STSZ;
		ptr->sampleSize = size;
		gf_free(ptr->sizes);
		ptr->sizes = NULL;
		return GF_OK;
	}

	if (fieldSize == 32) {
		//oops, doesn't fit in a compact table
		ptr->type = GF_ISOM_BOX_TYPE_STSZ;
		ptr->size += (4 * ptr->sampleCount);
		return GF_OK;
	}

	//make sure we are a compact table (no need to change the mem representation)
	ptr->type = GF_ISOM_BOX_TYPE_STZ2;
	ptr->sampleSize = fieldSize;
	if (fieldSize == 4) {
		//do not forget the 0 padding field for odd count
		ptr->size += (ptr->sampleCount + 1) / 2;
	} else {
		ptr->size += (ptr->sampleCount) * (fieldSize/8);
	}
	return GF_OK;
}