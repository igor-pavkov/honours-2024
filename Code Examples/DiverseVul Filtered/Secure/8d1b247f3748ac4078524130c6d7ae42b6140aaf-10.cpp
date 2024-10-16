static void vhost_vsock_common_post_load_timer_cleanup(VHostVSockCommon *vvc)
{
    if (!vvc->post_load_timer) {
        return;
    }

    timer_free(vvc->post_load_timer);
    vvc->post_load_timer = NULL;
}