static int ohci_service_iso_td(OHCIState *ohci, struct ohci_ed *ed,
                               int completion)
{
    int dir;
    size_t len = 0;
    const char *str = NULL;
    int pid;
    int ret;
    int i;
    USBDevice *dev;
    USBEndpoint *ep;
    struct ohci_iso_td iso_td;
    uint32_t addr;
    uint16_t starting_frame;
    int16_t relative_frame_number;
    int frame_count;
    uint32_t start_offset, next_offset, end_offset = 0;
    uint32_t start_addr, end_addr;

    addr = ed->head & OHCI_DPTR_MASK;

    if (ohci_read_iso_td(ohci, addr, &iso_td)) {
        trace_usb_ohci_iso_td_read_failed(addr);
        ohci_die(ohci);
        return 1;
    }

    starting_frame = OHCI_BM(iso_td.flags, TD_SF);
    frame_count = OHCI_BM(iso_td.flags, TD_FC);
    relative_frame_number = USUB(ohci->frame_number, starting_frame); 

    trace_usb_ohci_iso_td_head(
           ed->head & OHCI_DPTR_MASK, ed->tail & OHCI_DPTR_MASK,
           iso_td.flags, iso_td.bp, iso_td.next, iso_td.be,
           ohci->frame_number, starting_frame,
           frame_count, relative_frame_number);
    trace_usb_ohci_iso_td_head_offset(
           iso_td.offset[0], iso_td.offset[1],
           iso_td.offset[2], iso_td.offset[3],
           iso_td.offset[4], iso_td.offset[5],
           iso_td.offset[6], iso_td.offset[7]);

    if (relative_frame_number < 0) {
        trace_usb_ohci_iso_td_relative_frame_number_neg(relative_frame_number);
        return 1;
    } else if (relative_frame_number > frame_count) {
        /* ISO TD expired - retire the TD to the Done Queue and continue with
           the next ISO TD of the same ED */
        trace_usb_ohci_iso_td_relative_frame_number_big(relative_frame_number,
                                                        frame_count);
        OHCI_SET_BM(iso_td.flags, TD_CC, OHCI_CC_DATAOVERRUN);
        ed->head &= ~OHCI_DPTR_MASK;
        ed->head |= (iso_td.next & OHCI_DPTR_MASK);
        iso_td.next = ohci->done;
        ohci->done = addr;
        i = OHCI_BM(iso_td.flags, TD_DI);
        if (i < ohci->done_count)
            ohci->done_count = i;
        if (ohci_put_iso_td(ohci, addr, &iso_td)) {
            ohci_die(ohci);
            return 1;
        }
        return 0;
    }

    dir = OHCI_BM(ed->flags, ED_D);
    switch (dir) {
    case OHCI_TD_DIR_IN:
        str = "in";
        pid = USB_TOKEN_IN;
        break;
    case OHCI_TD_DIR_OUT:
        str = "out";
        pid = USB_TOKEN_OUT;
        break;
    case OHCI_TD_DIR_SETUP:
        str = "setup";
        pid = USB_TOKEN_SETUP;
        break;
    default:
        trace_usb_ohci_iso_td_bad_direction(dir);
        return 1;
    }

    if (!iso_td.bp || !iso_td.be) {
        trace_usb_ohci_iso_td_bad_bp_be(iso_td.bp, iso_td.be);
        return 1;
    }

    start_offset = iso_td.offset[relative_frame_number];
    next_offset = iso_td.offset[relative_frame_number + 1];

    if (!(OHCI_BM(start_offset, TD_PSW_CC) & 0xe) || 
        ((relative_frame_number < frame_count) && 
         !(OHCI_BM(next_offset, TD_PSW_CC) & 0xe))) {
        trace_usb_ohci_iso_td_bad_cc_not_accessed(start_offset, next_offset);
        return 1;
    }

    if ((relative_frame_number < frame_count) && (start_offset > next_offset)) {
        trace_usb_ohci_iso_td_bad_cc_overrun(start_offset, next_offset);
        return 1;
    }

    if ((start_offset & 0x1000) == 0) {
        start_addr = (iso_td.bp & OHCI_PAGE_MASK) |
            (start_offset & OHCI_OFFSET_MASK);
    } else {
        start_addr = (iso_td.be & OHCI_PAGE_MASK) |
            (start_offset & OHCI_OFFSET_MASK);
    }

    if (relative_frame_number < frame_count) {
        end_offset = next_offset - 1;
        if ((end_offset & 0x1000) == 0) {
            end_addr = (iso_td.bp & OHCI_PAGE_MASK) |
                (end_offset & OHCI_OFFSET_MASK);
        } else {
            end_addr = (iso_td.be & OHCI_PAGE_MASK) |
                (end_offset & OHCI_OFFSET_MASK);
        }
    } else {
        /* Last packet in the ISO TD */
        end_addr = iso_td.be;
    }

    if ((start_addr & OHCI_PAGE_MASK) != (end_addr & OHCI_PAGE_MASK)) {
        len = (end_addr & OHCI_OFFSET_MASK) + 0x1001
            - (start_addr & OHCI_OFFSET_MASK);
    } else {
        len = end_addr - start_addr + 1;
    }

    if (len && dir != OHCI_TD_DIR_IN) {
        if (ohci_copy_iso_td(ohci, start_addr, end_addr, ohci->usb_buf, len,
                             DMA_DIRECTION_TO_DEVICE)) {
            ohci_die(ohci);
            return 1;
        }
    }

    if (!completion) {
        bool int_req = relative_frame_number == frame_count &&
                       OHCI_BM(iso_td.flags, TD_DI) == 0;
        dev = ohci_find_device(ohci, OHCI_BM(ed->flags, ED_FA));
        ep = usb_ep_get(dev, pid, OHCI_BM(ed->flags, ED_EN));
        usb_packet_setup(&ohci->usb_packet, pid, ep, 0, addr, false, int_req);
        usb_packet_addbuf(&ohci->usb_packet, ohci->usb_buf, len);
        usb_handle_packet(dev, &ohci->usb_packet);
        if (ohci->usb_packet.status == USB_RET_ASYNC) {
            usb_device_flush_ep_queue(dev, ep);
            return 1;
        }
    }
    if (ohci->usb_packet.status == USB_RET_SUCCESS) {
        ret = ohci->usb_packet.actual_length;
    } else {
        ret = ohci->usb_packet.status;
    }

    trace_usb_ohci_iso_td_so(start_offset, end_offset, start_addr, end_addr,
                             str, len, ret);

    /* Writeback */
    if (dir == OHCI_TD_DIR_IN && ret >= 0 && ret <= len) {
        /* IN transfer succeeded */
        if (ohci_copy_iso_td(ohci, start_addr, end_addr, ohci->usb_buf, ret,
                             DMA_DIRECTION_FROM_DEVICE)) {
            ohci_die(ohci);
            return 1;
        }
        OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                    OHCI_CC_NOERROR);
        OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_SIZE, ret);
    } else if (dir == OHCI_TD_DIR_OUT && ret == len) {
        /* OUT transfer succeeded */
        OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                    OHCI_CC_NOERROR);
        OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_SIZE, 0);
    } else {
        if (ret > (ssize_t) len) {
            trace_usb_ohci_iso_td_data_overrun(ret, len);
            OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                        OHCI_CC_DATAOVERRUN);
            OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_SIZE,
                        len);
        } else if (ret >= 0) {
            trace_usb_ohci_iso_td_data_underrun(ret);
            OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                        OHCI_CC_DATAUNDERRUN);
        } else {
            switch (ret) {
            case USB_RET_IOERROR:
            case USB_RET_NODEV:
                OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                            OHCI_CC_DEVICENOTRESPONDING);
                OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_SIZE,
                            0);
                break;
            case USB_RET_NAK:
            case USB_RET_STALL:
                trace_usb_ohci_iso_td_nak(ret);
                OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                            OHCI_CC_STALL);
                OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_SIZE,
                            0);
                break;
            default:
                trace_usb_ohci_iso_td_bad_response(ret);
                OHCI_SET_BM(iso_td.offset[relative_frame_number], TD_PSW_CC,
                            OHCI_CC_UNDEXPETEDPID);
                break;
            }
        }
    }

    if (relative_frame_number == frame_count) {
        /* Last data packet of ISO TD - retire the TD to the Done Queue */
        OHCI_SET_BM(iso_td.flags, TD_CC, OHCI_CC_NOERROR);
        ed->head &= ~OHCI_DPTR_MASK;
        ed->head |= (iso_td.next & OHCI_DPTR_MASK);
        iso_td.next = ohci->done;
        ohci->done = addr;
        i = OHCI_BM(iso_td.flags, TD_DI);
        if (i < ohci->done_count)
            ohci->done_count = i;
    }
    if (ohci_put_iso_td(ohci, addr, &iso_td)) {
        ohci_die(ohci);
    }
    return 1;
}