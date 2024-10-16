int vhost_vsock_common_post_load(void *opaque, int version_id)
{
    VHostVSockCommon *vvc = opaque;
    VirtIODevice *vdev = VIRTIO_DEVICE(vvc);

    if (virtio_queue_get_addr(vdev, 2)) {
        /*
         * Defer transport reset event to a vm clock timer so that virtqueue
         * changes happen after migration has completed.
         */
        assert(!vvc->post_load_timer);
        vvc->post_load_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL,
                         vhost_vsock_common_post_load_timer_cb,
                         vvc);
        timer_mod(vvc->post_load_timer, 1);
    }
    return 0;
}