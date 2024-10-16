static int virtio_net_ufo_pre_save(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    tmp->has_ufo = tmp->parent->has_ufo;

    return 0;
}