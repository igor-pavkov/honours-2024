void ncp_extract_file_info(const void *structure, struct nw_info_struct *target)
{
	const __u8 *name_len;
	const int info_struct_size = offsetof(struct nw_info_struct, nameLen);

	memcpy(target, structure, info_struct_size);
	name_len = structure + info_struct_size;
	target->nameLen = *name_len;
	memcpy(target->entryName, name_len + 1, *name_len);
	target->entryName[*name_len] = '\0';
	target->volNumber = le32_to_cpu(target->volNumber);
	return;
}