void vhost_vsock_common_unrealize(VirtIODevice *vdev)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);

    vhost_vsock_common_post_load_timer_cleanup(vvc);

    virtio_delete_queue(vvc->recv_vq);
    virtio_delete_queue(vvc->trans_vq);
    virtio_delete_queue(vvc->event_vq);
    virtio_cleanup(vdev);
}