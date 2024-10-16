void gf_vvc_parse_sei(char *buffer, u32 nal_size, VVCState *vvc)
{
	gf_hevc_vvc_parse_sei(buffer, nal_size, NULL, vvc);
}