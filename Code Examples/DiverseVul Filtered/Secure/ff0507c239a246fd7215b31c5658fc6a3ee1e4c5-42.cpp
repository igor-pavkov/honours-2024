static void iscsi_save_designator(IscsiLun *lun,
                                  struct scsi_inquiry_device_identification *inq_di)
{
    struct scsi_inquiry_device_designator *desig, *copy = NULL;

    for (desig = inq_di->designators; desig; desig = desig->next) {
        if (desig->association ||
            desig->designator_type > SCSI_DESIGNATOR_TYPE_NAA) {
            continue;
        }
        /* NAA works better than T10 vendor ID based designator. */
        if (!copy || copy->designator_type < desig->designator_type) {
            copy = desig;
        }
    }
    if (copy) {
        lun->dd = g_new(struct scsi_inquiry_device_designator, 1);
        *lun->dd = *copy;
        lun->dd->next = NULL;
        lun->dd->designator = g_malloc(copy->designator_length);
        memcpy(lun->dd->designator, copy->designator, copy->designator_length);
    }
}