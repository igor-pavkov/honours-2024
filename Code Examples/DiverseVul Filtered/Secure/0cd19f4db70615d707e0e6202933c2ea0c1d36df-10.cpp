GF_Err gf_vvc_change_par(GF_VVCConfig *vvcc, s32 ar_n, s32 ar_d)
{
	GF_VUIInfo vuii;
	memset(&vuii, 0, sizeof(GF_VUIInfo));
	vuii.ar_num = ar_n;
	vuii.ar_den = ar_d;
	vuii.fullrange = -1;
	vuii.video_format = -1;
	vuii.color_prim = -1;
	vuii.color_tfc = -1;
	vuii.color_matrix = -1;
	return gf_vvc_change_vui(vvcc, &vuii);
}