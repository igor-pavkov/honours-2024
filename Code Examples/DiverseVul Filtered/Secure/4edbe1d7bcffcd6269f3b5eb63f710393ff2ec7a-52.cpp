static void list_version_get_needed(struct target_type *tt, void *needed_param)
{
    size_t *needed = needed_param;

    *needed += sizeof(struct dm_target_versions);
    *needed += strlen(tt->name);
    *needed += ALIGN_MASK;
}