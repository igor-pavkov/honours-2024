static void lan9118_mac_changed(lan9118_state *s)
{
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);
}