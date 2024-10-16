int send_probe_request(struct wif *wi)
{
    int len;
    unsigned char p[4096], r_smac[6];

    memcpy(p, PROBE_REQ, 24);

    len = 24;

    p[24] = 0x00;      //ESSID Tag Number
    p[25] = 0x00;      //ESSID Tag Length

    len += 2;

    memcpy(p+len, RATES, 16);

    len += 16;

    r_smac[0] = 0x00;
    r_smac[1] = rand() & 0xFF;
    r_smac[2] = rand() & 0xFF;
    r_smac[3] = rand() & 0xFF;
    r_smac[4] = rand() & 0xFF;
    r_smac[5] = rand() & 0xFF;

    memcpy(p+10, r_smac, 6);

    if (wi_write(wi, p, len, NULL) == -1) {
        switch (errno) {
        case EAGAIN:
        case ENOBUFS:
            usleep(10000);
            return 0; /* XXX not sure I like this... -sorbo */
        }

        perror("wi_write()");
        return -1;
    }

    return 0;
}