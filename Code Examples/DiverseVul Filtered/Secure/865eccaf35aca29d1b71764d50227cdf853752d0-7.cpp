void addReadOnlyDbPrivileges(PrivilegeVector* privileges, StringData dbName) {
    Privilege::addPrivilegeToPrivilegeVector(
        privileges, Privilege(ResourcePattern::forDatabaseName(dbName), readRoleActions));
    Privilege::addPrivilegeToPrivilegeVector(
        privileges,
        Privilege(ResourcePattern::forExactNamespace(NamespaceString(dbName, "system.js")),
                  readRoleActions));
}