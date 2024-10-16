NTSTATUS unix_perms_from_wire(connection_struct *conn,
			      const SMB_STRUCT_STAT *psbuf,
			      uint32_t perms,
			      enum perm_type ptype,
			      mode_t *ret_perms)
{
	mode_t ret = 0;

	if (perms == SMB_MODE_NO_CHANGE) {
		if (!VALID_STAT(*psbuf)) {
			return NT_STATUS_INVALID_PARAMETER;
		} else {
			*ret_perms = psbuf->st_ex_mode;
			return NT_STATUS_OK;
		}
	}

	ret = wire_perms_to_unix(perms);

	if (ptype == PERM_NEW_FILE) {
		/*
		 * "create mask"/"force create mode" are
		 * only applied to new files, not existing ones.
		 */
		ret &= lp_create_mask(SNUM(conn));
		/* Add in force bits */
		ret |= lp_force_create_mode(SNUM(conn));
	} else if (ptype == PERM_NEW_DIR) {
		/*
		 * "directory mask"/"force directory mode" are
		 * only applied to new directories, not existing ones.
		 */
		ret &= lp_directory_mask(SNUM(conn));
		/* Add in force bits */
		ret |= lp_force_directory_mode(SNUM(conn));
	}

	*ret_perms = ret;
	return NT_STATUS_OK;
}