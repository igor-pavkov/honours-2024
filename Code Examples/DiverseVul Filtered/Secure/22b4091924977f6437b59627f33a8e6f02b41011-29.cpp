static bool map_info2_flags_to_sbuf(const SMB_STRUCT_STAT *psbuf,
				const uint32_t smb_fflags,
				const uint32_t smb_fmask,
				int *stat_fflags)
{
	uint32_t max_fmask = 0;
	int i;

	*stat_fflags = psbuf->st_ex_flags;

	/* For each flags requested in smb_fmask, check the state of the
	 * corresponding flag in smb_fflags and set or clear the matching
	 * stat flag.
	 */

	for (i = 0; i < ARRAY_SIZE(info2_flags_map); ++i) {
	    max_fmask |= info2_flags_map[i].smb_fflag;
	    if (smb_fmask & info2_flags_map[i].smb_fflag) {
		    if (smb_fflags & info2_flags_map[i].smb_fflag) {
			    *stat_fflags |= info2_flags_map[i].stat_fflag;
		    } else {
			    *stat_fflags &= ~info2_flags_map[i].stat_fflag;
		    }
	    }
	}

	/* If smb_fmask is asking to set any bits that are not supported by
	 * our flag mappings, we should fail.
	 */
	if ((smb_fmask & max_fmask) != smb_fmask) {
		return False;
	}

	return True;
}