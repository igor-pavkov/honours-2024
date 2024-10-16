static void canonicalize_ea_name(files_struct *fsp,
			fstring unix_ea_name)
{
	size_t total_ea_len;
	TALLOC_CTX *mem_ctx = talloc_tos();
	struct ea_list *ea_list;
	NTSTATUS status = get_ea_list_from_fsp(mem_ctx,
					       fsp,
					       &total_ea_len,
					       &ea_list);
	if (!NT_STATUS_IS_OK(status)) {
		return;
	}

	for (; ea_list; ea_list = ea_list->next) {
		if (strequal(&unix_ea_name[5], ea_list->ea.name)) {
			DEBUG(10,("canonicalize_ea_name: %s -> %s\n",
				&unix_ea_name[5], ea_list->ea.name));
			strlcpy(&unix_ea_name[5], ea_list->ea.name, sizeof(fstring)-5);
			break;
		}
	}
}