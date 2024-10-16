static inline void time_t_to_filetime(time_t t, FILETIME *ft)
{
	long long winTime = t * 10000000LL + 116444736000000000LL;
	ft->dwLowDateTime = winTime;
	ft->dwHighDateTime = winTime >> 32;
}