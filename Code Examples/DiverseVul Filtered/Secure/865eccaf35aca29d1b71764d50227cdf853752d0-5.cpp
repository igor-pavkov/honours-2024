void RoleGraph::_createBuiltinRolesForDBIfNeeded(StringData dbname) {
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_READ, dbname));
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_READ_WRITE, dbname));
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_USER_ADMIN, dbname));
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_DB_ADMIN, dbname));
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_DB_OWNER, dbname));
    _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_ENABLE_SHARDING, dbname));

    if (dbname == "admin") {
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_READ_ANY_DB, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_READ_WRITE_ANY_DB, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_USER_ADMIN_ANY_DB, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_DB_ADMIN_ANY_DB, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_CLUSTER_MONITOR, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_HOST_MANAGEMENT, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_CLUSTER_MANAGEMENT, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_CLUSTER_ADMIN, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_BACKUP, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_RESTORE, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_ROOT, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_INTERNAL, dbname));
        _createBuiltinRoleIfNeeded(RoleName(BUILTIN_ROLE_QUERYABLE_BACKUP, dbname));
    }
}