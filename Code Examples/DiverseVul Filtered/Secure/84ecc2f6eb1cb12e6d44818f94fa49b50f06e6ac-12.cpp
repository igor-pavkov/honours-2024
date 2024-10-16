unsigned short *set_translate(int m, struct vc_data *vc)
{
	inv_translate[vc->vc_num] = m;
	return translations[m];
}