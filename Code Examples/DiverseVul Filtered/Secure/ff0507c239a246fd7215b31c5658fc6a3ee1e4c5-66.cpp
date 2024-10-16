static void iscsi_allocmap_free(IscsiLun *iscsilun)
{
    g_free(iscsilun->allocmap);
    g_free(iscsilun->allocmap_valid);
    iscsilun->allocmap = NULL;
    iscsilun->allocmap_valid = NULL;
}