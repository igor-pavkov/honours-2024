Status RoleGraph::removeAllPrivilegesFromRole(const RoleName& role) {
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(role)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot remove privileges from built-in role: "
                                                << role.getFullName());
    }
    _directPrivilegesForRole[role].clear();
    return Status::OK();
}