static GF_Err gf_isom_insert_copyright(GF_ISOFile *movie)
{
	u32 i;
	GF_Box *a;
	GF_FreeSpaceBox *_free;
	i=0;
	while ((a = (GF_Box *)gf_list_enum(movie->TopBoxes, &i))) {
		if (a->type == GF_ISOM_BOX_TYPE_FREE) {
			_free = (GF_FreeSpaceBox *)a;
			if (_free->dataSize) {
				if (!strcmp(_free->data, GPAC_ISOM_CPRT_NOTICE_VERSION)) return GF_OK;
				if (strstr(_free->data, GPAC_ISOM_CPRT_NOTICE)) {
					gf_free(_free->data);
					_free->data = gf_strdup(gf_sys_is_test_mode() ? GPAC_ISOM_CPRT_NOTICE : GPAC_ISOM_CPRT_NOTICE_VERSION);
					_free->dataSize = 1 + (u32) strlen(_free->data);
					return GF_OK;
				}
			}
		}
	}
	a = gf_isom_box_new(GF_ISOM_BOX_TYPE_FREE);
	if (!a) return GF_OUT_OF_MEM;
	_free = (GF_FreeSpaceBox *)a;
	_free->data = gf_strdup(gf_sys_is_test_mode() ? GPAC_ISOM_CPRT_NOTICE : GPAC_ISOM_CPRT_NOTICE_VERSION);
	_free->dataSize = (u32) strlen(_free->data) + 1;
	if (!_free->data) return GF_OUT_OF_MEM;
	return gf_list_add(movie->TopBoxes, _free);
}