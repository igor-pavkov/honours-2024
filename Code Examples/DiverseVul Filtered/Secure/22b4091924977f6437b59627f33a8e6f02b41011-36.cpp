static unsigned int fill_ea_buffer(TALLOC_CTX *mem_ctx, char *pdata, unsigned int total_data_size,
	connection_struct *conn, struct ea_list *ea_list)
{
	unsigned int ret_data_size = 4;
	char *p = pdata;

	SMB_ASSERT(total_data_size >= 4);

	if (!lp_ea_support(SNUM(conn))) {
		SIVAL(pdata,4,0);
		return 4;
	}

	for (p = pdata + 4; ea_list; ea_list = ea_list->next) {
		size_t dos_namelen;
		fstring dos_ea_name;
		push_ascii_fstring(dos_ea_name, ea_list->ea.name);
		dos_namelen = strlen(dos_ea_name);
		if (dos_namelen > 255 || dos_namelen == 0) {
			break;
		}
		if (ea_list->ea.value.length > 65535) {
			break;
		}
		if (4 + dos_namelen + 1 + ea_list->ea.value.length > total_data_size) {
			break;
		}

		/* We know we have room. */
		SCVAL(p,0,ea_list->ea.flags);
		SCVAL(p,1,dos_namelen);
		SSVAL(p,2,ea_list->ea.value.length);
		strlcpy(p+4, dos_ea_name, dos_namelen+1);
		if (ea_list->ea.value.length > 0) {
			memcpy(p + 4 + dos_namelen + 1,
			       ea_list->ea.value.data,
			       ea_list->ea.value.length);
		}

		total_data_size -= 4 + dos_namelen + 1 + ea_list->ea.value.length;
		p += 4 + dos_namelen + 1 + ea_list->ea.value.length;
	}

	ret_data_size = PTR_DIFF(p, pdata);
	DEBUG(10,("fill_ea_buffer: data_size = %u\n", ret_data_size ));
	SIVAL(pdata,0,ret_data_size);
	return ret_data_size;
}