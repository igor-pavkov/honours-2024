static bool virtio_net_attach_ebpf_to_backend(NICState *nic, int prog_fd)
{
    NetClientState *nc = qemu_get_peer(qemu_get_queue(nic), 0);
    if (nc == NULL || nc->info->set_steering_ebpf == NULL) {
        return false;
    }

    return nc->info->set_steering_ebpf(nc, prog_fd);
}