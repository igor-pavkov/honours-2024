static void vhdx_close(BlockDriverState *bs)
{
    BDRVVHDXState *s = bs->opaque;
    qemu_vfree(s->headers[0]);
    s->headers[0] = NULL;
    qemu_vfree(s->headers[1]);
    s->headers[1] = NULL;
    qemu_vfree(s->bat);
    s->bat = NULL;
    qemu_vfree(s->parent_entries);
    s->parent_entries = NULL;
    migrate_del_blocker(s->migration_blocker);
    error_free(s->migration_blocker);
    qemu_vfree(s->log.hdr);
    s->log.hdr = NULL;
    vhdx_region_unregister_all(s);
}