void addBackupPrivileges(PrivilegeVector* privileges) {
    ActionSet clusterActions;
    clusterActions << ActionType::appendOplogNote;  // For BRS
    clusterActions << ActionType::serverStatus;     // For push based initial sync
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forClusterResource(), clusterActions));

    ActionSet configSettingsActions;
    configSettingsActions << ActionType::insert << ActionType::update;
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(NamespaceString("config", "settings")),
                  configSettingsActions));

    addQueryableBackupPrivileges(privileges);
}