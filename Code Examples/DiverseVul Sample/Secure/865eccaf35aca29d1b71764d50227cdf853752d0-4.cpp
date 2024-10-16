void addRestorePrivileges(PrivilegeVector* privileges) {
    ActionSet actions;
    actions << ActionType::bypassDocumentValidation << ActionType::collMod
            << ActionType::convertToCapped << ActionType::createCollection
            << ActionType::createIndex << ActionType::dropCollection << ActionType::insert;

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyNormalResource(), actions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forCollectionName("system.js"), actions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyResource(), ActionType::listCollections));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("config"), actions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName("local"), actions));

    // Privileges for user/role management
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forAnyNormalResource(), userAdminRoleActions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(
                      AuthorizationManager::defaultTempUsersCollectionNamespace),
                  ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(
                      AuthorizationManager::defaultTempRolesCollectionNamespace),
                  ActionType::find));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::usersAltCollectionNamespace),
            actions));

    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(
                      AuthorizationManager::usersBackupCollectionNamespace),
                  actions));

    actions << ActionType::find;
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::versionCollectionNamespace),
            actions));

    // Need additional actions on system.users.
    actions << ActionType::update << ActionType::remove;
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forCollectionName("system.users"), actions));

    // Need to be able to run getParameter to check authSchemaVersion
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forClusterResource(), ActionType::getParameter));

    // Need to be able to create an index on the system.roles collection.
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(
            ResourcePattern::forExactNamespace(AuthorizationManager::rolesCollectionNamespace),
            ActionType::createIndex));

    // Need to be able to force UUID consistency in sharded restores
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forClusterResource(),
                  {ActionType::forceUUID, ActionType::useUUID}));
}