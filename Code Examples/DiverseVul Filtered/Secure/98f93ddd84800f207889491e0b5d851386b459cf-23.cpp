static int peer_detach(VirtIONet *n, int index)
{
    NetClientState *nc = qemu_get_subqueue(n->nic, index);

    if (!nc->peer) {
        return 0;
    }

    if (nc->peer->info->type !=  NET_CLIENT_OPTIONS_KIND_TAP) {
        return 0;
    }

    return tap_disable(nc->peer);
}