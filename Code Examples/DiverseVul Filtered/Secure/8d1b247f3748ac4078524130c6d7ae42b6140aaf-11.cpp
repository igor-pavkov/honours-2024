static void vhost_vsock_common_post_load_timer_cb(void *opaque)
{
    VHostVSockCommon *vvc = opaque;

    vhost_vsock_common_post_load_timer_cleanup(vvc);
    vhost_vsock_common_send_transport_reset(vvc);
}