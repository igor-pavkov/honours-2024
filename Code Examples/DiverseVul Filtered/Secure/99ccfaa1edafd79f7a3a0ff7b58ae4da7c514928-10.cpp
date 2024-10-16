static void pcnet_poll(PCNetState *s)
{
    if (CSR_RXON(s)) {
        pcnet_rdte_poll(s);
    }

    if (CSR_TDMD(s) || (CSR_TXON(s) && !CSR_DPOLL(s) && pcnet_tdte_poll(s))) {
        /* prevent recursion */
        if (s->tx_busy) {
            return;
        }
        pcnet_transmit(s);
    }
}