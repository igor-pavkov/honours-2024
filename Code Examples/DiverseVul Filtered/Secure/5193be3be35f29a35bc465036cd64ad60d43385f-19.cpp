I2SCodec *tsc210x_codec(uWireSlave *chip)
{
    TSC210xState *s = (TSC210xState *) chip->opaque;

    return &s->codec;
}