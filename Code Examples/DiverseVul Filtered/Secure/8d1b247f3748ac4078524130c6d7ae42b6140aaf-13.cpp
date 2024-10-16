void vhost_vsock_common_stop(VirtIODevice *vdev)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);
    BusState *qbus = BUS(qdev_get_parent_bus(DEVICE(vdev)));
    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);
    int ret;

    if (!k->set_guest_notifiers) {
        return;
    }

    vhost_dev_stop(&vvc->vhost_dev, vdev);

    ret = k->set_guest_notifiers(qbus->parent, vvc->vhost_dev.nvqs, false);
    if (ret < 0) {
        error_report("vhost guest notifier cleanup failed: %d", ret);
        return;
    }

    vhost_dev_disable_notifiers(&vvc->vhost_dev, vdev);
}