static void map_info2_flags_from_sbuf(const SMB_STRUCT_STAT *psbuf,
				uint32_t *smb_fflags, uint32_t *smb_fmask)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(info2_flags_map); ++i) {
	    *smb_fmask |= info2_flags_map[i].smb_fflag;
	    if (psbuf->st_ex_flags & info2_flags_map[i].stat_fflag) {
		    *smb_fflags |= info2_flags_map[i].smb_fflag;
	    }
	}
}