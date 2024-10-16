static inline int get_len(LZOContext *c, int x, int mask)
{
    int cnt = x & mask;
    if (!cnt) {
        while (!(x = get_byte(c))) {
            if (cnt >= INT_MAX - 1000) {
                c->error |= AV_LZO_ERROR;
                break;
            }
            cnt += 255;
        }
        cnt += mask + x;
    }
    return cnt;
}