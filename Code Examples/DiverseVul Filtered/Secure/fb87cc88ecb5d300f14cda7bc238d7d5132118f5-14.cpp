Status RoleGraph::deleteRole(const RoleName& role) {
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(role)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot delete built-in role: "
                                                << role.getFullName());
    }

    for (std::vector<RoleName>::iterator it = _roleToSubordinates[role].begin();
         it != _roleToSubordinates[role].end();
         ++it) {
        _roleToMembers[*it].erase(
            std::find(_roleToMembers[*it].begin(), _roleToMembers[*it].end(), role));
    }
    for (std::vector<RoleName>::iterator it = _roleToMembers[role].begin();
         it != _roleToMembers[role].end();
         ++it) {
        _roleToSubordinates[*it].erase(
            std::find(_roleToSubordinates[*it].begin(), _roleToSubordinates[*it].end(), role));
    }
    _roleToSubordinates.erase(role);
    _roleToIndirectSubordinates.erase(role);
    _roleToMembers.erase(role);
    _directPrivilegesForRole.erase(role);
    _allPrivilegesForRole.erase(role);
    _allRoles.erase(role);
    return Status::OK();
}