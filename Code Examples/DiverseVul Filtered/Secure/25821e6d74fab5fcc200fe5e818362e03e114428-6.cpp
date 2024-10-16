static diy_fp_t cached_power(int k)
{
	diy_fp_t res;
	int index = 343 + k;
	res.f = powers_ten[index];
	res.e = powers_ten_e[index];
	return res;
}