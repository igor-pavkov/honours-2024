static void iscsi_co_init_iscsitask(IscsiLun *iscsilun, struct IscsiTask *iTask)
{
    *iTask = (struct IscsiTask) {
        .co         = qemu_coroutine_self(),
        .iscsilun   = iscsilun,
    };
}