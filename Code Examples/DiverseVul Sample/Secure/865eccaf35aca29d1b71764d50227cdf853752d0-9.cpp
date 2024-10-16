void addDbAdminDbPrivileges(PrivilegeVector* privileges, StringData dbName) {
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName(dbName), dbAdminRoleActions));

    ActionSet profileActions = readRoleActions;
    profileActions.addAction(ActionType::convertToCapped);
    profileActions.addAction(ActionType::createCollection);
    profileActions.addAction(ActionType::dropCollection);
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(NamespaceString(dbName, "system.profile")),
                  profileActions));
}