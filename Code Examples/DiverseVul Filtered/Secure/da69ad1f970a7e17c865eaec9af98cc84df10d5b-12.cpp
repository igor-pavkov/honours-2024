static const char *flac_dmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	if ((size>4) && !strncmp(data, "fLaC", 4)) {
		*score = GF_FPROBE_SUPPORTED;
		return "audio/flac";
	}
	return NULL;
}