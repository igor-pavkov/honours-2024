int vhost_vsock_common_start(VirtIODevice *vdev)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);
    BusState *qbus = BUS(qdev_get_parent_bus(DEVICE(vdev)));
    VirtioBusClass *k = VIRTIO_BUS_GET_CLASS(qbus);
    int ret;
    int i;

    if (!k->set_guest_notifiers) {
        error_report("binding does not support guest notifiers");
        return -ENOSYS;
    }

    ret = vhost_dev_enable_notifiers(&vvc->vhost_dev, vdev);
    if (ret < 0) {
        error_report("Error enabling host notifiers: %d", -ret);
        return ret;
    }

    ret = k->set_guest_notifiers(qbus->parent, vvc->vhost_dev.nvqs, true);
    if (ret < 0) {
        error_report("Error binding guest notifier: %d", -ret);
        goto err_host_notifiers;
    }

    vvc->vhost_dev.acked_features = vdev->guest_features;
    ret = vhost_dev_start(&vvc->vhost_dev, vdev);
    if (ret < 0) {
        error_report("Error starting vhost: %d", -ret);
        goto err_guest_notifiers;
    }

    /*
     * guest_notifier_mask/pending not used yet, so just unmask
     * everything here.  virtio-pci will do the right thing by
     * enabling/disabling irqfd.
     */
    for (i = 0; i < vvc->vhost_dev.nvqs; i++) {
        vhost_virtqueue_mask(&vvc->vhost_dev, vdev, i, false);
    }

    return 0;

err_guest_notifiers:
    k->set_guest_notifiers(qbus->parent, vvc->vhost_dev.nvqs, false);
err_host_notifiers:
    vhost_dev_disable_notifiers(&vvc->vhost_dev, vdev);
    return ret;
}