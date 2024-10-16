static void vhost_vsock_common_guest_notifier_mask(VirtIODevice *vdev, int idx,
                                            bool mask)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);

    vhost_virtqueue_mask(&vvc->vhost_dev, vdev, idx, mask);
}