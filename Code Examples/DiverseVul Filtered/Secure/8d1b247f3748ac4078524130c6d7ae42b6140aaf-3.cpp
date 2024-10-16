int vhost_vsock_common_pre_save(void *opaque)
{
    VHostVSockCommon *vvc = opaque;

    /*
     * At this point, backend must be stopped, otherwise
     * it might keep writing to memory.
     */
    assert(!vvc->vhost_dev.started);

    return 0;
}