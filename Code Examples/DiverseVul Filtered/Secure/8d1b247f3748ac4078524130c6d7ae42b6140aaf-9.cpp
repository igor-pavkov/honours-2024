static bool vhost_vsock_common_guest_notifier_pending(VirtIODevice *vdev,
                                               int idx)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);

    return vhost_virtqueue_pending(&vvc->vhost_dev, idx);
}