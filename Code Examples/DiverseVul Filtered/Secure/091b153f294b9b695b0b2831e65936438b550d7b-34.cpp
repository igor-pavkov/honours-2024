int make_arp_request(unsigned char *h80211, unsigned char *bssid, unsigned char *src_mac, unsigned char *dst_mac, unsigned char *src_ip, unsigned char *dst_ip, int size)
{
	unsigned char *arp_header = (unsigned char*)"\xaa\xaa\x03\x00\x00\x00\x08\x06\x00\x01\x08\x00\x06\x04\x00\x01";
	unsigned char *header80211 = (unsigned char*)"\x08\x41\x95\x00";

    // 802.11 part
    memcpy(h80211,    header80211, 4);
    memcpy(h80211+4,  bssid,       6);
    memcpy(h80211+10, src_mac,     6);
    memcpy(h80211+16, dst_mac,     6);
    h80211[22] = '\x00';
    h80211[23] = '\x00';

    // ARP part
    memcpy(h80211+24, arp_header, 16);
    memcpy(h80211+40, src_mac,     6);
    memcpy(h80211+46, src_ip,      4);
    memset(h80211+50, '\x00',      6);
    memcpy(h80211+56, dst_ip,      4);

    // Insert padding bytes
    memset(h80211+60, '\x00', size-60);

    return 0;
}