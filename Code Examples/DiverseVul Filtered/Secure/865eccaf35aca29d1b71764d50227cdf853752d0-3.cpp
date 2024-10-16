void addQueryableBackupPrivileges(PrivilegeVector* privileges) {
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyResource(), ActionType::collStats));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyNormalResource(), ActionType::find));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyResource(), ActionType::listCollections));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyResource(), ActionType::listIndexes));

    ActionSet clusterActions;
    clusterActions << ActionType::getParameter  // To check authSchemaVersion
                   << ActionType::listDatabases << ActionType::useUUID;
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forClusterResource(), clusterActions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("config"), ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("local"), ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forCollectionName("system.js"), ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forCollectionName("system.users"), ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forCollectionName("system.profile"), ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::usersAltCollectionNamespace),
            ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(
                      AuthorizationManager::usersBackupCollectionNamespace),
                  ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::rolesCollectionNamespace),
            ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::versionCollectionNamespace),
            ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(NamespaceString("config", "settings")),
                  ActionType::find));
}