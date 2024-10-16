static GF_DOVIDecoderConfigurationRecord* DOVI_DuplicateConfig(GF_DOVIDecoderConfigurationRecord *cfg)
{
	GF_DOVIDecoderConfigurationRecord* out = NULL;
	GF_SAFEALLOC(out, GF_DOVIDecoderConfigurationRecord);
	if (!out) return NULL;

	out->dv_version_major = cfg->dv_version_major;
	out->dv_version_minor = cfg->dv_version_minor;
	out->dv_profile = cfg->dv_profile;
	out->dv_level = cfg->dv_level;
	out->rpu_present_flag = cfg->rpu_present_flag;
	out->el_present_flag = cfg->el_present_flag;
	out->bl_present_flag = cfg->bl_present_flag;

	return out;
}