static int is_vms_varlen_txt(__G__ ef_buf, ef_len)
    __GDEF
    uch *ef_buf;        /* buffer containing extra field */
    unsigned ef_len;    /* total length of extra field */
{
    unsigned eb_id;
    unsigned eb_len;
    uch *eb_data;
    unsigned eb_datlen;
#define VMSREC_C_UNDEF  0
#define VMSREC_C_VAR    2
    uch vms_rectype = VMSREC_C_UNDEF;
 /* uch vms_fileorg = 0; */ /* currently, fileorg is not used... */

#define VMSPK_ITEMID            0
#define VMSPK_ITEMLEN           2
#define VMSPK_ITEMHEADSZ        4

#define VMSATR_C_RECATTR        4
#define VMS_FABSIG              0x42414656      /* "VFAB" */
/* offsets of interesting fields in VMS fabdef structure */
#define VMSFAB_B_RFM            31      /* record format byte */
#define VMSFAB_B_ORG            29      /* file organization byte */

    if (ef_len == 0 || ef_buf == NULL)
        return FALSE;

    while (ef_len >= EB_HEADSIZE) {
        eb_id = makeword(EB_ID + ef_buf);
        eb_len = makeword(EB_LEN + ef_buf);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            /* discovered some extra field inconsistency! */
            Trace((stderr,
              "is_vms_varlen_txt: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        switch (eb_id) {
          case EF_PKVMS:
            /* The PKVMS e.f. raw data part consists of:
             * a) 4 bytes CRC checksum
             * b) list of uncompressed variable-length data items
             * Each data item is introduced by a fixed header
             *  - 2 bytes data type ID
             *  - 2 bytes <size> of data
             *  - <size> bytes of actual attribute data
             */

            /* get pointer to start of data and its total length */
            eb_data = ef_buf+(EB_HEADSIZE+4);
            eb_datlen = eb_len-4;

            /* test the CRC checksum */
            if (makelong(ef_buf+EB_HEADSIZE) !=
                crc32(CRCVAL_INITIAL, eb_data, (extent)eb_datlen))
            {
                Info(slide, 1, ((char *)slide,
                  "[Warning: CRC error, discarding PKWARE extra field]\n"));
                /* skip over the data analysis code */
                break;
            }

            /* scan through the attribute data items */
            while (eb_datlen > 4)
            {
                unsigned fldsize = makeword(&eb_data[VMSPK_ITEMLEN]);

                /* check the item type word */
                switch (makeword(&eb_data[VMSPK_ITEMID])) {
                  case VMSATR_C_RECATTR:
                    /* we have found the (currently only) interesting
                     * data item */
                    if (fldsize >= 1) {
                        vms_rectype = eb_data[VMSPK_ITEMHEADSZ] & 15;
                     /* vms_fileorg = eb_data[VMSPK_ITEMHEADSZ] >> 4; */
                    }
                    break;
                  default:
                    break;
                }
                /* skip to next data item */
                eb_datlen -= fldsize + VMSPK_ITEMHEADSZ;
                eb_data += fldsize + VMSPK_ITEMHEADSZ;
            }
            break;

          case EF_IZVMS:
            if (makelong(ef_buf+EB_HEADSIZE) == VMS_FABSIG) {
                if ((eb_data = extract_izvms_block(__G__
                                                   ef_buf+EB_HEADSIZE, eb_len,
                                                   &eb_datlen, NULL, 0))
                    != NULL)
                {
                    if (eb_datlen >= VMSFAB_B_RFM+1) {
                        vms_rectype = eb_data[VMSFAB_B_RFM] & 15;
                     /* vms_fileorg = eb_data[VMSFAB_B_ORG] >> 4; */
                    }
                    free(eb_data);
                }
            }
            break;

          default:
            break;
        }

        /* Skip this extra field block */
        ef_buf += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);
    }

    return (vms_rectype == VMSREC_C_VAR);

} /* end function is_vms_varlen_txtfile() */