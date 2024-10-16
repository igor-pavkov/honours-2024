static void cmd_inquiry(IDEState *s, uint8_t *buf)
{
    uint8_t page_code = buf[2];
    int max_len = buf[4];

    unsigned idx = 0;
    unsigned size_idx;
    unsigned preamble_len;

    /* If the EVPD (Enable Vital Product Data) bit is set in byte 1,
     * we are being asked for a specific page of info indicated by byte 2. */
    if (buf[1] & 0x01) {
        preamble_len = 4;
        size_idx = 3;

        buf[idx++] = 0x05;      /* CD-ROM */
        buf[idx++] = page_code; /* Page Code */
        buf[idx++] = 0x00;      /* reserved */
        idx++;                  /* length (set later) */

        switch (page_code) {
        case 0x00:
            /* Supported Pages: List of supported VPD responses. */
            buf[idx++] = 0x00; /* 0x00: Supported Pages, and: */
            buf[idx++] = 0x83; /* 0x83: Device Identification. */
            break;

        case 0x83:
            /* Device Identification. Each entry is optional, but the entries
             * included here are modeled after libata's VPD responses.
             * If the response is given, at least one entry must be present. */

            /* Entry 1: Serial */
            if (idx + 24 > max_len) {
                /* Not enough room for even the first entry: */
                /* 4 byte header + 20 byte string */
                ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                                    ASC_DATA_PHASE_ERROR);
                return;
            }
            buf[idx++] = 0x02; /* Ascii */
            buf[idx++] = 0x00; /* Vendor Specific */
            buf[idx++] = 0x00;
            buf[idx++] = 20;   /* Remaining length */
            padstr8(buf + idx, 20, s->drive_serial_str);
            idx += 20;

            /* Entry 2: Drive Model and Serial */
            if (idx + 72 > max_len) {
                /* 4 (header) + 8 (vendor) + 60 (model & serial) */
                goto out;
            }
            buf[idx++] = 0x02; /* Ascii */
            buf[idx++] = 0x01; /* T10 Vendor */
            buf[idx++] = 0x00;
            buf[idx++] = 68;
            padstr8(buf + idx, 8, "ATA"); /* Generic T10 vendor */
            idx += 8;
            padstr8(buf + idx, 40, s->drive_model_str);
            idx += 40;
            padstr8(buf + idx, 20, s->drive_serial_str);
            idx += 20;

            /* Entry 3: WWN */
            if (s->wwn && (idx + 12 <= max_len)) {
                /* 4 byte header + 8 byte wwn */
                buf[idx++] = 0x01; /* Binary */
                buf[idx++] = 0x03; /* NAA */
                buf[idx++] = 0x00;
                buf[idx++] = 0x08;
                stq_be_p(&buf[idx], s->wwn);
                idx += 8;
            }
            break;

        default:
            /* SPC-3, revision 23 sec. 6.4 */
            ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                                ASC_INV_FIELD_IN_CMD_PACKET);
            return;
        }
    } else {
        preamble_len = 5;
        size_idx = 4;

        buf[0] = 0x05; /* CD-ROM */
        buf[1] = 0x80; /* removable */
        buf[2] = 0x00; /* ISO */
        buf[3] = 0x21; /* ATAPI-2 (XXX: put ATAPI-4 ?) */
        /* buf[size_idx] set below. */
        buf[5] = 0;    /* reserved */
        buf[6] = 0;    /* reserved */
        buf[7] = 0;    /* reserved */
        padstr8(buf + 8, 8, "QEMU");
        padstr8(buf + 16, 16, "QEMU DVD-ROM");
        padstr8(buf + 32, 4, s->version);
        idx = 36;
    }

 out:
    buf[size_idx] = idx - preamble_len;
    ide_atapi_cmd_reply(s, idx, max_len);
}