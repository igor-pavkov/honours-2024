static const char *kext_short_name(RKext *kext) {
	const char *sn = strrchr (kext->name, '.');
	return sn ? sn + 1 : kext->name;
}