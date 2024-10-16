static void pcnet_bcr_writew(PCNetState *s, uint32_t rap, uint32_t val)
{
    rap &= 127;
#ifdef PCNET_DEBUG_BCR
    printf("pcnet_bcr_writew rap=%d val=0x%04x\n", rap, val);
#endif
    switch (rap) {
    case BCR_SWS:
        if (!(CSR_STOP(s) || CSR_SPND(s)))
            return;
        val &= ~0x0300;
        switch (val & 0x00ff) {
        case 0:
            val |= 0x0200;
            break;
        case 1:
            val |= 0x0100;
            break;
        case 2:
        case 3:
            val |= 0x0300;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR, "pcnet: Bad SWSTYLE=0x%02x\n",
                          val & 0xff);
            val = 0x0200;
            break;
        }
#ifdef PCNET_DEBUG
       printf("BCR_SWS=0x%04x\n", val);
#endif
        /* fall through */
    case BCR_LNKST:
    case BCR_LED1:
    case BCR_LED2:
    case BCR_LED3:
    case BCR_MC:
    case BCR_FDC:
    case BCR_BSBC:
    case BCR_EECAS:
    case BCR_PLAT:
        s->bcr[rap] = val;
        break;
    default:
        break;
    }
}