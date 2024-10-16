static void vhost_vsock_common_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    device_class_set_props(dc, vhost_vsock_common_properties);
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    vdc->guest_notifier_mask = vhost_vsock_common_guest_notifier_mask;
    vdc->guest_notifier_pending = vhost_vsock_common_guest_notifier_pending;
}