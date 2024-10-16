static void tsc210x_i2s_set_rate(TSC210xState *s, int in, int out)
{
    s->i2s_tx_rate = out;
    s->i2s_rx_rate = in;
}