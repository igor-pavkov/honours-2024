static void iscsi_allocmap_invalidate(IscsiLun *iscsilun)
{
    if (iscsilun->allocmap) {
        bitmap_zero(iscsilun->allocmap, iscsilun->allocmap_size);
    }
    if (iscsilun->allocmap_valid) {
        bitmap_zero(iscsilun->allocmap_valid, iscsilun->allocmap_size);
    }
}