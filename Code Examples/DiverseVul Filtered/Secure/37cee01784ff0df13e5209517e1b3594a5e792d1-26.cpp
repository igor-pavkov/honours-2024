static void lan9118_set_link(NetClientState *nc)
{
    phy_update_link(qemu_get_nic_opaque(nc));
}