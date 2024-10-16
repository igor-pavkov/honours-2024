static void iscsi_populate_target_desc(unsigned char *desc, IscsiLun *lun)
{
    struct scsi_inquiry_device_designator *dd = lun->dd;

    memset(desc, 0, 32);
    desc[0] = 0xE4; /* IDENT_DESCR_TGT_DESCR */
    desc[4] = dd->code_set;
    desc[5] = (dd->designator_type & 0xF)
        | ((dd->association & 3) << 4);
    desc[7] = dd->designator_length;
    memcpy(desc + 8, dd->designator, MIN(dd->designator_length, 20));

    desc[28] = 0;
    desc[29] = (lun->block_size >> 16) & 0xFF;
    desc[30] = (lun->block_size >> 8) & 0xFF;
    desc[31] = lun->block_size & 0xFF;
}