GF_Err gf_avc_change_color(GF_AVCConfig *avcc, s32 fullrange, s32 vidformat, s32 colorprim, s32 transfer, s32 colmatrix)
{
	GF_VUIInfo vuii;
	memset(&vuii, 0, sizeof(GF_VUIInfo));
	vuii.ar_num = -1;
	vuii.ar_den = -1;
	vuii.fullrange = fullrange;
	vuii.video_format = vidformat;
	vuii.color_prim = colorprim;
	vuii.color_tfc = transfer;
	vuii.color_matrix = colmatrix;
	return gf_avc_change_vui(avcc, &vuii);
}