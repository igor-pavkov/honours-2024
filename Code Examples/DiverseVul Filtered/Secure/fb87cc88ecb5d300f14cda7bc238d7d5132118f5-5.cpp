Status RoleGraph::removeRoleFromRole(const RoleName& recipient, const RoleName& role) {
    if (!roleExists(recipient)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << recipient.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(recipient)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot remove roles from built-in role: "
                                                << role.getFullName());
    }
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }

    std::vector<RoleName>::iterator itToRm =
        std::find(_roleToMembers[role].begin(), _roleToMembers[role].end(), recipient);
    if (itToRm != _roleToMembers[role].end()) {
        _roleToMembers[role].erase(itToRm);
    } else {
        return Status(ErrorCodes::RolesNotRelated,
                      mongoutils::str::stream() << recipient.getFullName() << " is not a member"
                                                                              " of "
                                                << role.getFullName());
    }

    itToRm = std::find(
        _roleToSubordinates[recipient].begin(), _roleToSubordinates[recipient].end(), role);
    fassert(16827, itToRm != _roleToSubordinates[recipient].end());
    _roleToSubordinates[recipient].erase(itToRm);
    return Status::OK();
}