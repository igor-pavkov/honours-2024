char *get_sa_devname(unsigned int major, unsigned int minor, unsigned long long wwn[],
		     unsigned int part_nr, uint64_t flags)
{
	char *dev_name = NULL, *persist_dev_name = NULL;
	static char sid[64];
	char xsid[32] = "", pn[16] = "";

	if (DISPLAY_PERSIST_NAME_S(flags)) {
		persist_dev_name = get_persistent_name_from_pretty(get_devname(major, minor, TRUE));
	}

	if (persist_dev_name) {
		dev_name = persist_dev_name;
	}
	else {
		if ((USE_STABLE_ID(flags)) && (wwn[0] != 0)) {
			if (wwn[1] != 0) {
				sprintf(xsid, "%016llx", wwn[1]);
			}
			if (part_nr) {
				sprintf(pn, "-%d", part_nr);
			}
			snprintf(sid, sizeof(sid), "%#016llx%s%s", wwn[0], xsid, pn);
			dev_name = sid;
		}
		else if ((USE_PRETTY_OPTION(flags)) && (major == dm_major)) {
			dev_name = transform_devmapname(major, minor);
		}

		if (!dev_name) {
			dev_name = get_devname(major, minor,
					       USE_PRETTY_OPTION(flags));
		}
	}

	return dev_name;
}