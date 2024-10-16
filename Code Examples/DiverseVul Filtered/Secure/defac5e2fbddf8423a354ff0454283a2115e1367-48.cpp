static FDrive *get_cur_drv(FDCtrl *fdctrl)
{
    FDrive *cur_drv = get_drv(fdctrl, fdctrl->cur_drv);

    if (!cur_drv->blk) {
        /*
         * Kludge: empty drive line selected. Create an anonymous
         * BlockBackend to avoid NULL deref with various BlockBackend
         * API calls within this model (CVE-2021-20196).
         * Due to the controller QOM model limitations, we don't
         * attach the created to the controller device.
         */
        cur_drv->blk = blk_create_empty_drive();
    }
    return cur_drv;
}