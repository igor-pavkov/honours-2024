static GF_AVCConfig *AVC_DuplicateConfig(GF_AVCConfig *cfg)
{
	u32 i, count;
	GF_NALUFFParam *p1, *p2;
	GF_AVCConfig *cfg_new;
	if (!cfg)
		return NULL;
	cfg_new = gf_odf_avc_cfg_new();
	cfg_new->AVCLevelIndication = cfg->AVCLevelIndication;
	cfg_new->AVCProfileIndication = cfg->AVCProfileIndication;
	cfg_new->configurationVersion = cfg->configurationVersion;
	cfg_new->nal_unit_size = cfg->nal_unit_size;
	cfg_new->profile_compatibility = cfg->profile_compatibility;
	cfg_new->complete_representation = cfg->complete_representation;
	cfg_new->chroma_bit_depth = cfg->chroma_bit_depth;
	cfg_new->luma_bit_depth = cfg->luma_bit_depth;
	cfg_new->chroma_format = cfg->chroma_format;

	count = gf_list_count(cfg->sequenceParameterSets);
	for (i=0; i<count; i++) {
		p1 = (GF_NALUFFParam*)gf_list_get(cfg->sequenceParameterSets, i);
		p2 = (GF_NALUFFParam*)gf_malloc(sizeof(GF_NALUFFParam));
		p2->size = p1->size;
		p2->id = p1->id;
		p2->data = (char *)gf_malloc(sizeof(char)*p1->size);
		memcpy(p2->data, p1->data, sizeof(char)*p1->size);
		gf_list_add(cfg_new->sequenceParameterSets, p2);
	}

	count = gf_list_count(cfg->pictureParameterSets);
	for (i=0; i<count; i++) {
		p1 = (GF_NALUFFParam*)gf_list_get(cfg->pictureParameterSets, i);
		p2 = (GF_NALUFFParam*)gf_malloc(sizeof(GF_NALUFFParam));
		p2->size = p1->size;
		p2->id = p1->id;
		p2->data = (char*)gf_malloc(sizeof(char)*p1->size);
		memcpy(p2->data, p1->data, sizeof(char)*p1->size);
		gf_list_add(cfg_new->pictureParameterSets, p2);
	}

	if (cfg->sequenceParameterSetExtensions) {
		cfg_new->sequenceParameterSetExtensions = gf_list_new();
		count = gf_list_count(cfg->sequenceParameterSetExtensions);
		for (i=0; i<count; i++) {
			p1 = (GF_NALUFFParam*)gf_list_get(cfg->sequenceParameterSetExtensions, i);
			p2 = (GF_NALUFFParam*)gf_malloc(sizeof(GF_NALUFFParam));
			p2->size = p1->size;
			p2->id = p1->id;
			p2->data = (char*)gf_malloc(sizeof(char)*p1->size);
			memcpy(p2->data, p1->data, sizeof(char)*p1->size);
			gf_list_add(cfg_new->sequenceParameterSetExtensions, p2);
		}
	}
	return cfg_new;
}