int check_shared_key(unsigned char *h80211, int caplen)
{
    int m_bmac, m_smac, m_dmac, n, textlen;
    char ofn[1024];
    char text[4096];
    char prga[4096];
    unsigned int long crc;

    if((unsigned)caplen > sizeof(G.sharedkey[0])) return 1;

    m_bmac = 16;
    m_smac = 10;
    m_dmac = 4;

    if( time(NULL) - G.sk_start > 5)
    {
        /* timeout(5sec) - remove all packets, restart timer */
        memset(G.sharedkey, '\x00', 4096*3);
        G.sk_start = time(NULL);
    }

    /* is auth packet */
    if( (h80211[1] & 0x40) != 0x40 )
    {
        /* not encrypted */
        if( ( h80211[24] + (h80211[25] << 8) ) == 1 )
        {
            /* Shared-Key Authentication */
            if( ( h80211[26] + (h80211[27] << 8) ) == 2 )
            {
                /* sequence == 2 */
                memcpy(G.sharedkey[0], h80211, caplen);
                G.sk_len = caplen-24;
            }
            if( ( h80211[26] + (h80211[27] << 8) ) == 4 )
            {
                /* sequence == 4 */
                memcpy(G.sharedkey[2], h80211, caplen);
            }
        }
        else return 1;
    }
    else
    {
        /* encrypted */
        memcpy(G.sharedkey[1], h80211, caplen);
        G.sk_len2 = caplen-24-4;
    }

    /* check if the 3 packets form a proper authentication */

    if( ( memcmp(G.sharedkey[0]+m_bmac, NULL_MAC, 6) == 0 ) ||
        ( memcmp(G.sharedkey[1]+m_bmac, NULL_MAC, 6) == 0 ) ||
        ( memcmp(G.sharedkey[2]+m_bmac, NULL_MAC, 6) == 0 ) ) /* some bssids == zero */
    {
        return 1;
    }

    if( ( memcmp(G.sharedkey[0]+m_bmac, G.sharedkey[1]+m_bmac, 6) != 0 ) ||
        ( memcmp(G.sharedkey[0]+m_bmac, G.sharedkey[2]+m_bmac, 6) != 0 ) ) /* all bssids aren't equal */
    {
        return 1;
    }

    if( ( memcmp(G.sharedkey[0]+m_smac, G.sharedkey[2]+m_smac, 6) != 0 ) ||
        ( memcmp(G.sharedkey[0]+m_smac, G.sharedkey[1]+m_dmac, 6) != 0 ) ) /* SA in 2&4 != DA in 3 */
    {
        return 1;
    }

    if( (memcmp(G.sharedkey[0]+m_dmac, G.sharedkey[2]+m_dmac, 6) != 0 ) ||
        (memcmp(G.sharedkey[0]+m_dmac, G.sharedkey[1]+m_smac, 6) != 0 ) ) /* DA in 2&4 != SA in 3 */
    {
        return 1;
    }

    textlen = G.sk_len;

    if(textlen+4 != G.sk_len2)
    {
        snprintf(G.message, sizeof(G.message), "][ Broken SKA: %02X:%02X:%02X:%02X:%02X:%02X ",
                    *(G.sharedkey[0]+m_bmac), *(G.sharedkey[0]+m_bmac+1), *(G.sharedkey[0]+m_bmac+2),
                *(G.sharedkey[0]+m_bmac+3), *(G.sharedkey[0]+m_bmac+4), *(G.sharedkey[0]+m_bmac+5));
        return 1;
    }

    if((unsigned)textlen > sizeof(text) - 4) return 1;

    memcpy(text, G.sharedkey[0]+24, textlen);

    /* increment sequence number from 2 to 3 */
    text[2] = text[2]+1;

    crc = 0xFFFFFFFF;

    for( n = 0; n < textlen; n++ )
        crc = crc_tbl[(crc ^ text[n]) & 0xFF] ^ (crc >> 8);

    crc = ~crc;

    /* append crc32 over body */
    text[textlen]     = (crc      ) & 0xFF;
    text[textlen+1]   = (crc >>  8) & 0xFF;
    text[textlen+2]   = (crc >> 16) & 0xFF;
    text[textlen+3]   = (crc >> 24) & 0xFF;

    /* cleartext XOR cipher */
    for(n=0; n<(textlen+4); n++)
    {
        prga[4+n] = (text[n] ^ G.sharedkey[1][28+n]) & 0xFF;
    }

    /* write IV+index */
    prga[0] = G.sharedkey[1][24] & 0xFF;
    prga[1] = G.sharedkey[1][25] & 0xFF;
    prga[2] = G.sharedkey[1][26] & 0xFF;
    prga[3] = G.sharedkey[1][27] & 0xFF;

    if( G.f_xor != NULL )
    {
        fclose(G.f_xor);
        G.f_xor = NULL;
    }

    snprintf( ofn, sizeof( ofn ) - 1, "%s-%02d-%02X-%02X-%02X-%02X-%02X-%02X.%s", G.prefix, G.f_index,
              *(G.sharedkey[0]+m_bmac), *(G.sharedkey[0]+m_bmac+1), *(G.sharedkey[0]+m_bmac+2),
              *(G.sharedkey[0]+m_bmac+3), *(G.sharedkey[0]+m_bmac+4), *(G.sharedkey[0]+m_bmac+5), "xor" );

    G.f_xor = fopen( ofn, "w");
    if(G.f_xor == NULL)
        return 1;

    for(n=0; n<textlen+8; n++)
        fputc((prga[n] & 0xFF), G.f_xor);

    fflush(G.f_xor);

    if( G.f_xor != NULL )
    {
        fclose(G.f_xor);
        G.f_xor = NULL;
    }

    snprintf(G.message, sizeof(G.message), "][ %d bytes keystream: %02X:%02X:%02X:%02X:%02X:%02X ",
                textlen+4, *(G.sharedkey[0]+m_bmac), *(G.sharedkey[0]+m_bmac+1), *(G.sharedkey[0]+m_bmac+2),
              *(G.sharedkey[0]+m_bmac+3), *(G.sharedkey[0]+m_bmac+4), *(G.sharedkey[0]+m_bmac+5));

    memset(G.sharedkey, '\x00', 512*3);
    /* ok, keystream saved */
    return 0;
}