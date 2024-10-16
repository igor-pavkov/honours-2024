static int peer_detach(VirtIONet *n, int index)
{
    NetClientState *nc = qemu_get_subqueue(n->nic, index);

    if (!nc->peer) {
        return 0;
    }

    if (nc->peer->info->type == NET_CLIENT_DRIVER_VHOST_USER) {
        vhost_set_vring_enable(nc->peer, 0);
    }

    if (nc->peer->info->type !=  NET_CLIENT_DRIVER_TAP) {
        return 0;
    }

    return tap_disable(nc->peer);
}