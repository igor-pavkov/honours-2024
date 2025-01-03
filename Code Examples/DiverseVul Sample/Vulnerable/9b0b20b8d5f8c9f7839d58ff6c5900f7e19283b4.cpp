static INT AirPDcapScanForKeys(
    PAIRPDCAP_CONTEXT ctx,
    const guint8 *data,
    const guint mac_header_len,
    const guint tot_len,
    AIRPDCAP_SEC_ASSOCIATION_ID id
)
{
    const UCHAR *addr;
    guint bodyLength;
    PAIRPDCAP_SEC_ASSOCIATION sta_sa;
    PAIRPDCAP_SEC_ASSOCIATION sa;
    guint offset = 0;
    const guint8 dot1x_header[] = {
        0xAA,             /* DSAP=SNAP */
        0xAA,             /* SSAP=SNAP */
        0x03,             /* Control field=Unnumbered frame */
        0x00, 0x00, 0x00, /* Org. code=encaps. Ethernet */
        0x88, 0x8E        /* Type: 802.1X authentication */
    };
    const guint8 bt_dot1x_header[] = {
        0xAA,             /* DSAP=SNAP */
        0xAA,             /* SSAP=SNAP */
        0x03,             /* Control field=Unnumbered frame */
        0x00, 0x19, 0x58, /* Org. code=Bluetooth SIG */
        0x00, 0x03        /* Type: Bluetooth Security */
    };
    const guint8 tdls_header[] = {
        0xAA,             /* DSAP=SNAP */
        0xAA,             /* SSAP=SNAP */
        0x03,             /* Control field=Unnumbered frame */
        0x00, 0x00, 0x00, /* Org. code=encaps. Ethernet */
        0x89, 0x0D,       /* Type: 802.11 - Fast Roaming Remote Request */
        0x02,             /* Payload Type: TDLS */
        0X0C              /* Action Category: TDLS */
    };

    const EAPOL_RSN_KEY *pEAPKey;
#ifdef _DEBUG
#define MSGBUF_LEN 255
    CHAR msgbuf[MSGBUF_LEN];
#endif
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapScanForKeys");

    /* cache offset in the packet data */
    offset = mac_header_len;

    /* check if the packet has an LLC header and the packet is 802.1X authentication (IEEE 802.1X-2004, pg. 24) */
    if (memcmp(data+offset, dot1x_header, 8) == 0 || memcmp(data+offset, bt_dot1x_header, 8) == 0) {

        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Authentication: EAPOL packet", AIRPDCAP_DEBUG_LEVEL_3);

        /* skip LLC header */
        offset+=8;

        /* check if the packet is a EAPOL-Key (0x03) (IEEE 802.1X-2004, pg. 25) */
        if (data[offset+1]!=3) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Not EAPOL-Key", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* get and check the body length (IEEE 802.1X-2004, pg. 25) */
        bodyLength=pntoh16(data+offset+2);
        if ((tot_len-offset-4) < bodyLength) { /* Only check if frame is long enough for eapol header, ignore tailing garbage, see bug 9065 */
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "EAPOL body too short", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* skip EAPOL MPDU and go to the first byte of the body */
        offset+=4;

        pEAPKey = (const EAPOL_RSN_KEY *) (data+offset);

        /* check if the key descriptor type is valid (IEEE 802.1X-2004, pg. 27) */
        if (/*pEAPKey->type!=0x1 &&*/ /* RC4 Key Descriptor Type (deprecated) */
            pEAPKey->type != AIRPDCAP_RSN_WPA2_KEY_DESCRIPTOR &&             /* IEEE 802.11 Key Descriptor Type  (WPA2) */
            pEAPKey->type != AIRPDCAP_RSN_WPA_KEY_DESCRIPTOR)           /* 254 = RSN_KEY_DESCRIPTOR - WPA,              */
        {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Not valid key descriptor type", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* start with descriptor body */
        offset+=1;

        /* search for a cached Security Association for current BSSID and AP */
        sa = AirPDcapGetSaPtr(ctx, &id);
        if (sa == NULL){
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "No SA for BSSID found", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_REQ_DATA;
        }

        /* It could be a Pairwise Key exchange, check */
        if (AirPDcapRsna4WHandshake(ctx, data, sa, offset, tot_len) == AIRPDCAP_RET_SUCCESS_HANDSHAKE)
            return AIRPDCAP_RET_SUCCESS_HANDSHAKE;

        if (mac_header_len + GROUP_KEY_PAYLOAD_LEN_MIN > tot_len) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Message too short for Group Key", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* Verify the bitfields: Key = 0(groupwise) Mic = 1 Ack = 1 Secure = 1 */
        if (AIRPDCAP_EAP_KEY(data[offset+1])!=0 ||
            AIRPDCAP_EAP_ACK(data[offset+1])!=1 ||
            AIRPDCAP_EAP_MIC(data[offset]) != 1 ||
            AIRPDCAP_EAP_SEC(data[offset]) != 1){

            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Key bitfields not correct for Group Key", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* force STA address to be the broadcast MAC so we create an SA for the groupkey */
        memcpy(id.sta, broadcast_mac, AIRPDCAP_MAC_LEN);

        /* get the Security Association structure for the broadcast MAC and AP */
        sa = AirPDcapGetSaPtr(ctx, &id);
        if (sa == NULL){
            return AIRPDCAP_RET_REQ_DATA;
        }

        /* Get the SA for the STA, since we need its pairwise key to decrpyt the group key */

        /* get STA address */
        if ( (addr=AirPDcapGetStaAddress((const AIRPDCAP_MAC_FRAME_ADDR4 *)(data))) != NULL) {
            memcpy(id.sta, addr, AIRPDCAP_MAC_LEN);
#ifdef _DEBUG
            g_snprintf(msgbuf, MSGBUF_LEN, "ST_MAC: %2X.%2X.%2X.%2X.%2X.%2X\t", id.sta[0],id.sta[1],id.sta[2],id.sta[3],id.sta[4],id.sta[5]);
#endif
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", msgbuf, AIRPDCAP_DEBUG_LEVEL_3);
        } else {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "SA not found", AIRPDCAP_DEBUG_LEVEL_5);
            return AIRPDCAP_RET_REQ_DATA;
        }

        sta_sa = AirPDcapGetSaPtr(ctx, &id);
        if (sta_sa == NULL){
            return AIRPDCAP_RET_REQ_DATA;
        }

        /* Try to extract the group key and install it in the SA */
        return (AirPDcapDecryptWPABroadcastKey(pEAPKey, sta_sa->wpa.ptk+16, sa, tot_len-offset+1));

    } else if (memcmp(data+offset, tdls_header, 10) == 0) {
        const guint8 *initiator, *responder;
        guint8 action;
        guint status, offset_rsne = 0, offset_fte = 0, offset_link = 0, offset_timeout = 0;
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Authentication: TDLS Action Frame", AIRPDCAP_DEBUG_LEVEL_3);

        /* skip LLC header */
        offset+=10;

        /* check if the packet is a TDLS response or confirm */
        action = data[offset];
        if (action!=1 && action!=2) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Not Response nor confirm", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* check status */
        offset++;
        status=pntoh16(data+offset);
        if (status!=0) {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "TDLS setup not successfull", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        /* skip Token + capabilities */
        offset+=5;

        /* search for RSN, Fast BSS Transition, Link Identifier and Timeout Interval IEs */

        while(offset < (tot_len - 2)) {
            if (data[offset] == 48) {
                offset_rsne = offset;
            } else if (data[offset] == 55) {
                offset_fte = offset;
            } else if (data[offset] == 56) {
                offset_timeout = offset;
            } else if (data[offset] == 101) {
                offset_link = offset;
            }

            if (tot_len < offset + data[offset + 1] + 2) {
                return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
            }
            offset += data[offset + 1] + 2;
        }

        if (offset_rsne == 0 || offset_fte == 0 ||
            offset_timeout == 0 || offset_link == 0)
        {
            AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Cannot Find all necessary IEs", AIRPDCAP_DEBUG_LEVEL_3);
            return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
        }

        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Found RSNE/Fast BSS/Timeout Interval/Link IEs", AIRPDCAP_DEBUG_LEVEL_3);

        /* Will create a Security Association between 2 STA. Need to get both MAC address */
        initiator = &data[offset_link + 8];
        responder = &data[offset_link + 14];

        if (memcmp(initiator, responder, AIRPDCAP_MAC_LEN) < 0) {
            memcpy(id.sta, initiator, AIRPDCAP_MAC_LEN);
            memcpy(id.bssid, responder, AIRPDCAP_MAC_LEN);
        } else {
            memcpy(id.sta, responder, AIRPDCAP_MAC_LEN);
            memcpy(id.bssid, initiator, AIRPDCAP_MAC_LEN);
        }

        sa = AirPDcapGetSaPtr(ctx, &id);
        if (sa == NULL){
            return AIRPDCAP_RET_REQ_DATA;
        }

        if (sa->validKey) {
            if (memcmp(sa->wpa.nonce, data + offset_fte + 52, AIRPDCAP_WPA_NONCE_LEN) == 0) {
                /* Already have valid key for this SA, no need to redo key derivation */
                return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
            } else {
                /* We are opening a new session with the same two STA, save previous sa  */
                AIRPDCAP_SEC_ASSOCIATION *tmp_sa = g_new(AIRPDCAP_SEC_ASSOCIATION, 1);
                memcpy(tmp_sa, sa, sizeof(AIRPDCAP_SEC_ASSOCIATION));
                sa->next=tmp_sa;
                sa->validKey = FALSE;
            }
        }

        if (AirPDcapTDLSDeriveKey(sa, data, offset_rsne, offset_fte, offset_timeout, offset_link, action)
            == AIRPDCAP_RET_SUCCESS) {
            AIRPDCAP_DEBUG_TRACE_END("AirPDcapScanForKeys");
            return AIRPDCAP_RET_SUCCESS_HANDSHAKE;
        }
    } else {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapScanForKeys", "Skipping: not an EAPOL packet", AIRPDCAP_DEBUG_LEVEL_3);
    }

    AIRPDCAP_DEBUG_TRACE_END("AirPDcapScanForKeys");
    return AIRPDCAP_RET_NO_VALID_HANDSHAKE;
}