static NTSTATUS fill_ea_chained_buffer(TALLOC_CTX *mem_ctx,
				       char *pdata,
				       unsigned int total_data_size,
				       unsigned int *ret_data_size,
				       connection_struct *conn,
				       struct ea_list *ea_list)
{
	uint8_t *p = (uint8_t *)pdata;
	uint8_t *last_start = NULL;
	bool do_store_data = (pdata != NULL);

	*ret_data_size = 0;

	if (!lp_ea_support(SNUM(conn))) {
		return NT_STATUS_NO_EAS_ON_FILE;
	}

	for (; ea_list; ea_list = ea_list->next) {
		size_t dos_namelen;
		fstring dos_ea_name;
		size_t this_size;
		size_t pad = 0;

		if (last_start != NULL && do_store_data) {
			SIVAL(last_start, 0, PTR_DIFF(p, last_start));
		}
		last_start = p;

		push_ascii_fstring(dos_ea_name, ea_list->ea.name);
		dos_namelen = strlen(dos_ea_name);
		if (dos_namelen > 255 || dos_namelen == 0) {
			return NT_STATUS_INTERNAL_ERROR;
		}
		if (ea_list->ea.value.length > 65535) {
			return NT_STATUS_INTERNAL_ERROR;
		}

		this_size = 0x08 + dos_namelen + 1 + ea_list->ea.value.length;

		if (ea_list->next) {
			pad = (4 - (this_size % 4)) % 4;
			this_size += pad;
		}

		if (do_store_data) {
			if (this_size > total_data_size) {
				return NT_STATUS_INFO_LENGTH_MISMATCH;
			}

			/* We know we have room. */
			SIVAL(p, 0x00, 0); /* next offset */
			SCVAL(p, 0x04, ea_list->ea.flags);
			SCVAL(p, 0x05, dos_namelen);
			SSVAL(p, 0x06, ea_list->ea.value.length);
			strlcpy((char *)(p+0x08), dos_ea_name, dos_namelen+1);
			memcpy(p + 0x08 + dos_namelen + 1, ea_list->ea.value.data, ea_list->ea.value.length);
			if (pad) {
				memset(p + 0x08 + dos_namelen + 1 + ea_list->ea.value.length,
					'\0',
					pad);
			}
			total_data_size -= this_size;
		}

		p += this_size;
	}

	*ret_data_size = PTR_DIFF(p, pdata);
	DEBUG(10,("fill_ea_chained_buffer: data_size = %u\n", *ret_data_size));
	return NT_STATUS_OK;
}