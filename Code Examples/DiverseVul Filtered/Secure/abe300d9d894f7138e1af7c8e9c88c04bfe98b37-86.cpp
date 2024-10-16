static void virtio_net_detach_epbf_rss(VirtIONet *n)
{
    virtio_net_attach_ebpf_to_backend(n->nic, -1);
}