int virDomainMigrateUnmanagedCheckCompat(virDomainPtr domain,
                                         unsigned int flags)
{
    VIR_EXCLUSIVE_FLAGS_RET(VIR_MIGRATE_NON_SHARED_DISK,
                            VIR_MIGRATE_NON_SHARED_INC,
                            -1);

    if (flags & VIR_MIGRATE_OFFLINE &&
        !VIR_DRV_SUPPORTS_FEATURE(domain->conn->driver, domain->conn,
                                  VIR_DRV_FEATURE_MIGRATION_OFFLINE)) {
        virReportError(VIR_ERR_ARGUMENT_UNSUPPORTED, "%s",
                       _("offline migration is not supported by "
                         "the source host"));
        return -1;
    }

    if (flags & VIR_MIGRATE_PEER2PEER) {
        if (!VIR_DRV_SUPPORTS_FEATURE(domain->conn->driver, domain->conn,
                                      VIR_DRV_FEATURE_MIGRATION_P2P)) {
            virReportError(VIR_ERR_ARGUMENT_UNSUPPORTED, "%s",
                           _("p2p migration is not supported by "
                             "the source host"));
            return -1;
        }
    } else {
        if (!VIR_DRV_SUPPORTS_FEATURE(domain->conn->driver, domain->conn,
                                      VIR_DRV_FEATURE_MIGRATION_DIRECT)) {
            virReportError(VIR_ERR_ARGUMENT_UNSUPPORTED, "%s",
                           _("direct migration is not supported by "
                             "the source host"));
            return -1;
        }
    }

    return 0;
}