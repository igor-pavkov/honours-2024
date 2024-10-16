void virtio_queue_set_align(VirtIODevice *vdev, int n, int align)
{
    BusState *qbus = qdev_get_parent_bus(DEVICE(vdev));
    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);

    /* Check that the transport told us it was going to do this
     * (so a buggy transport will immediately assert rather than
     * silently failing to migrate this state)
     */
    assert(k->has_variable_vring_alignment);

    vdev->vq[n].vring.align = align;
    virtqueue_init(&vdev->vq[n]);
}