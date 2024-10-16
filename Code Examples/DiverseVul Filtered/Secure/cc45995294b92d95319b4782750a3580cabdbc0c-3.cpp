int virtio_set_features(VirtIODevice *vdev, uint32_t val)
{
    BusState *qbus = qdev_get_parent_bus(DEVICE(vdev));
    VirtioBusClass *vbusk = VIRTIO_BUS_GET_CLASS(qbus);
    VirtioDeviceClass *k = VIRTIO_DEVICE_GET_CLASS(vdev);
    uint32_t supported_features = vbusk->get_features(qbus->parent);
    bool bad = (val & ~supported_features) != 0;

    val &= supported_features;
    if (k->set_features) {
        k->set_features(vdev, val);
    }
    vdev->guest_features = val;
    return bad ? -1 : 0;
}