static Bool leap_year(u32 year) {
	year += 1900;
	return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) ? GF_TRUE : GF_FALSE;
}