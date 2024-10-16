static bool mac_table_fits(void *opaque, int version_id)
{
    return VIRTIO_NET(opaque)->mac_table.in_use <= MAC_TABLE_ENTRIES;
}