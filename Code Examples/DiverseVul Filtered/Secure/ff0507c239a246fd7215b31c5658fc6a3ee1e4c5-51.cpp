static char *get_initiator_name(QemuOpts *opts)
{
    const char *name;
    char *iscsi_name;
    UuidInfo *uuid_info;

    name = qemu_opt_get(opts, "initiator-name");
    if (name) {
        return g_strdup(name);
    }

    uuid_info = qmp_query_uuid(NULL);
    if (strcmp(uuid_info->UUID, UUID_NONE) == 0) {
        name = qemu_get_vm_name();
    } else {
        name = uuid_info->UUID;
    }
    iscsi_name = g_strdup_printf("iqn.2008-11.org.linux-kvm%s%s",
                                 name ? ":" : "", name ? name : "");
    qapi_free_UuidInfo(uuid_info);
    return iscsi_name;
}