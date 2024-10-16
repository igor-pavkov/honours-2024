bool change_notify_fsp_has_changes(struct files_struct *fsp)
{
	if (fsp == NULL) {
		return false;
	}

	if (fsp->notify == NULL) {
		return false;
	}

	if (fsp->notify->num_changes == 0) {
		return false;
	}

	return true;
}