static bool virtio_net_attach_epbf_rss(VirtIONet *n)
{
    struct EBPFRSSConfig config = {};

    if (!ebpf_rss_is_loaded(&n->ebpf_rss)) {
        return false;
    }

    rss_data_to_rss_config(&n->rss_data, &config);

    if (!ebpf_rss_set_all(&n->ebpf_rss, &config,
                          n->rss_data.indirections_table, n->rss_data.key)) {
        return false;
    }

    if (!virtio_net_attach_ebpf_to_backend(n->nic, n->ebpf_rss.program_fd)) {
        return false;
    }

    return true;
}