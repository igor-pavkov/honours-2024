void addUserAdminAnyDbPrivileges(PrivilegeVector* privileges) {
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyNormalResource(), userAdminRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("local"), userAdminRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("config"), userAdminRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forClusterResource(), ActionType::listDatabases));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forClusterResource(), ActionType::authSchemaUpgrade));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forClusterResource(), ActionType::invalidateUserCache));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forClusterResource(), ActionType::viewUser));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forAnyNormalResource(), ActionType::listCachedAndActiveUsers));

    ActionSet readRoleAndIndexActions;
    readRoleAndIndexActions += readRoleActions;
    readRoleAndIndexActions << ActionType::createIndex << ActionType::dropIndex;

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forCollectionName("system.users"), readRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::usersCollectionNamespace),
            readRoleAndIndexActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::rolesCollectionNamespace),
            readRoleAndIndexActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::versionCollectionNamespace),
            readRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::usersAltCollectionNamespace),
            readRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(
                      AuthorizationManager::usersBackupCollectionNamespace),
                  readRoleActions));
}