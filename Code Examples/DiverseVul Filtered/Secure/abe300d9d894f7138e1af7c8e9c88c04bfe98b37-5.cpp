static bool mac_table_doesnt_fit(void *opaque, int version_id)
{
    return !mac_table_fits(opaque, version_id);
}