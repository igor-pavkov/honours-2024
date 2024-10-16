void vhost_vsock_common_realize(VirtIODevice *vdev, const char *name)
{
    VHostVSockCommon *vvc = VHOST_VSOCK_COMMON(vdev);

    virtio_init(vdev, name, VIRTIO_ID_VSOCK,
                sizeof(struct virtio_vsock_config));

    /* Receive and transmit queues belong to vhost */
    vvc->recv_vq = virtio_add_queue(vdev, VHOST_VSOCK_QUEUE_SIZE,
                                      vhost_vsock_common_handle_output);
    vvc->trans_vq = virtio_add_queue(vdev, VHOST_VSOCK_QUEUE_SIZE,
                                       vhost_vsock_common_handle_output);

    /* The event queue belongs to QEMU */
    vvc->event_vq = virtio_add_queue(vdev, VHOST_VSOCK_QUEUE_SIZE,
                                       vhost_vsock_common_handle_output);

    vvc->vhost_dev.nvqs = ARRAY_SIZE(vvc->vhost_vqs);
    vvc->vhost_dev.vqs = vvc->vhost_vqs;

    vvc->post_load_timer = NULL;
}