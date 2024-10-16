static void virtio_scsi_hotplug(SCSIBus *bus, SCSIDevice *dev)
{
    VirtIOSCSI *s = container_of(bus, VirtIOSCSI, bus);
    VirtIODevice *vdev = VIRTIO_DEVICE(s);

    if ((vdev->guest_features >> VIRTIO_SCSI_F_HOTPLUG) & 1) {
        virtio_scsi_push_event(s, dev, VIRTIO_SCSI_T_TRANSPORT_RESET,
                               VIRTIO_SCSI_EVT_RESET_RESCAN);
    }
}