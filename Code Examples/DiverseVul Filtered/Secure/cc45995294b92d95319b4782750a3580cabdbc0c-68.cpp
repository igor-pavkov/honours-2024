void virtio_save(VirtIODevice *vdev, QEMUFile *f)
{
    BusState *qbus = qdev_get_parent_bus(DEVICE(vdev));
    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);
    int i;

    if (k->save_config) {
        k->save_config(qbus->parent, f);
    }

    qemu_put_8s(f, &vdev->status);
    qemu_put_8s(f, &vdev->isr);
    qemu_put_be16s(f, &vdev->queue_sel);
    qemu_put_be32s(f, &vdev->guest_features);
    qemu_put_be32(f, vdev->config_len);
    qemu_put_buffer(f, vdev->config, vdev->config_len);

    for (i = 0; i < VIRTIO_PCI_QUEUE_MAX; i++) {
        if (vdev->vq[i].vring.num == 0)
            break;
    }

    qemu_put_be32(f, i);

    for (i = 0; i < VIRTIO_PCI_QUEUE_MAX; i++) {
        if (vdev->vq[i].vring.num == 0)
            break;

        qemu_put_be32(f, vdev->vq[i].vring.num);
        if (k->has_variable_vring_alignment) {
            qemu_put_be32(f, vdev->vq[i].vring.align);
        }
        qemu_put_be64(f, vdev->vq[i].pa);
        qemu_put_be16s(f, &vdev->vq[i].last_avail_idx);
        if (k->save_queue) {
            k->save_queue(qbus->parent, i, f);
        }
    }
}