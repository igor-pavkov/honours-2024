Status RoleGraph::addRoleToRole(const RoleName& recipient, const RoleName& role) {
    if (!roleExists(recipient)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << recipient.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(recipient)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot grant roles to built-in role: "
                                                << role.getFullName());
    }
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }

    if (std::find(_roleToSubordinates[recipient].begin(),
                  _roleToSubordinates[recipient].end(),
                  role) == _roleToSubordinates[recipient].end()) {
        // Only add role if it's not already present
        _roleToSubordinates[recipient].push_back(role);
        _roleToMembers[role].push_back(recipient);
    }

    return Status::OK();
}