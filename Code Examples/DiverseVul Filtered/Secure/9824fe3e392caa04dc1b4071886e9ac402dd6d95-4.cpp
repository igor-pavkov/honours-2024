size_t zmalloc_get_smap_bytes_by_field(char *field, long pid) {
#if defined(__APPLE__)
    struct proc_regioninfo pri;
    if (proc_pidinfo(pid, PROC_PIDREGIONINFO, 0, &pri, PROC_PIDREGIONINFO_SIZE) ==
	PROC_PIDREGIONINFO_SIZE) {
	if (!strcmp(field, "Private_Dirty:")) {
            return (size_t)pri.pri_pages_dirtied * 4096;
	} else if (!strcmp(field, "Rss:")) {
            return (size_t)pri.pri_pages_resident * 4096;
	} else if (!strcmp(field, "AnonHugePages:")) {
            return 0;
	}
    }
    return 0;
#endif
    ((void) field);
    ((void) pid);
    return 0;
}