static void lan9118_reset(DeviceState *d)
{
    lan9118_state *s = LAN9118(d);

    s->irq_cfg &= (IRQ_TYPE | IRQ_POL);
    s->int_sts = 0;
    s->int_en = 0;
    s->fifo_int = 0x48000000;
    s->rx_cfg = 0;
    s->tx_cfg = 0;
    s->hw_cfg = s->mode_16bit ? 0x00050000 : 0x00050004;
    s->pmt_ctrl &= 0x45;
    s->gpio_cfg = 0;
    s->txp->fifo_used = 0;
    s->txp->state = TX_IDLE;
    s->txp->cmd_a = 0xffffffffu;
    s->txp->cmd_b = 0xffffffffu;
    s->txp->len = 0;
    s->txp->fifo_used = 0;
    s->tx_fifo_size = 4608;
    s->tx_status_fifo_used = 0;
    s->rx_status_fifo_size = 704;
    s->rx_fifo_size = 2640;
    s->rx_fifo_used = 0;
    s->rx_status_fifo_size = 176;
    s->rx_status_fifo_used = 0;
    s->rxp_offset = 0;
    s->rxp_size = 0;
    s->rxp_pad = 0;
    s->rx_packet_size_tail = s->rx_packet_size_head;
    s->rx_packet_size[s->rx_packet_size_head] = 0;
    s->mac_cmd = 0;
    s->mac_data = 0;
    s->afc_cfg = 0;
    s->e2p_cmd = 0;
    s->e2p_data = 0;
    s->free_timer_start = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) / 40;

    ptimer_transaction_begin(s->timer);
    ptimer_stop(s->timer);
    ptimer_set_count(s->timer, 0xffff);
    ptimer_transaction_commit(s->timer);
    s->gpt_cfg = 0xffff;

    s->mac_cr = MAC_CR_PRMS;
    s->mac_hashh = 0;
    s->mac_hashl = 0;
    s->mac_mii_acc = 0;
    s->mac_mii_data = 0;
    s->mac_flow = 0;

    s->read_word_n = 0;
    s->write_word_n = 0;

    phy_reset(s);

    s->eeprom_writable = 0;
    lan9118_reload_eeprom(s);
}