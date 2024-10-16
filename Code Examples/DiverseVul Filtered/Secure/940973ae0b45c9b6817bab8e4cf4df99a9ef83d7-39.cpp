static void put_le16(uint16_t *p, unsigned int v)
{
    *p = cpu_to_le16(v);
}