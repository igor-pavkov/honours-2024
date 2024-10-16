RoleNameIterator RoleGraph::getRolesForDatabase(const std::string& dbname) {
    _createBuiltinRolesForDBIfNeeded(dbname);

    std::set<RoleName>::const_iterator lower = _allRoles.lower_bound(RoleName("", dbname));
    std::string afterDB = dbname;
    afterDB.push_back('\0');
    std::set<RoleName>::const_iterator upper = _allRoles.lower_bound(RoleName("", afterDB));
    return makeRoleNameIterator(lower, upper);
}