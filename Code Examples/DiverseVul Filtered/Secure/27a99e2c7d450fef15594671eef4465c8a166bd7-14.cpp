static uint32_t decode_key(uint8_t *buf)
{
    uint32_t key = 0;

    for (int i = 0; i < 32; i++) {
        unsigned p = keybits[i];
        key |= ((buf[p] >> ((i*5+3)&7)) & 1u) << i;
    }

    return key;
}