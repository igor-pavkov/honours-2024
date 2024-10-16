static void list_version_get_info(struct target_type *tt, void *param)
{
    struct vers_iter *info = param;

    /* Check space - it might have changed since the first iteration */
    if ((char *)info->vers + sizeof(tt->version) + strlen(tt->name) + 1 >
	info->end) {

	info->flags = DM_BUFFER_FULL_FLAG;
	return;
    }

    if (info->old_vers)
	info->old_vers->next = (uint32_t) ((void *)info->vers -
					   (void *)info->old_vers);
    info->vers->version[0] = tt->version[0];
    info->vers->version[1] = tt->version[1];
    info->vers->version[2] = tt->version[2];
    info->vers->next = 0;
    strcpy(info->vers->name, tt->name);

    info->old_vers = info->vers;
    info->vers = align_ptr(((void *) ++info->vers) + strlen(tt->name) + 1);
}