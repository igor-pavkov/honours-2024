static void fdctrl_handle_drive_specification_command(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv = get_cur_drv(fdctrl);
    uint32_t pos;

    pos = fdctrl->data_pos - 1;
    pos %= FD_SECTOR_LEN;
    if (fdctrl->fifo[pos] & 0x80) {
        /* Command parameters done */
        if (fdctrl->fifo[pos] & 0x40) {
            fdctrl->fifo[0] = fdctrl->fifo[1];
            fdctrl->fifo[2] = 0;
            fdctrl->fifo[3] = 0;
            fdctrl_to_result_phase(fdctrl, 4);
        } else {
            fdctrl_to_command_phase(fdctrl);
        }
    } else if (fdctrl->data_len > 7) {
        /* ERROR */
        fdctrl->fifo[0] = 0x80 |
            (cur_drv->head << 2) | GET_CUR_DRV(fdctrl);
        fdctrl_to_result_phase(fdctrl, 1);
    }
}