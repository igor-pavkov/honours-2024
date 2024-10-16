static void virtio_net_unload_ebpf(VirtIONet *n)
{
    virtio_net_attach_ebpf_to_backend(n->nic, -1);
    ebpf_rss_unload(&n->ebpf_rss);
}