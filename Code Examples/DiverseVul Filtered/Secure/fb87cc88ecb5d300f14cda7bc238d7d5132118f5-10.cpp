Status RoleGraph::addPrivilegeToRole(const RoleName& role, const Privilege& privilegeToAdd) {
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(role)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot grant privileges to built-in role: "
                                                << role.getFullName());
    }

    _addPrivilegeToRoleNoChecks(role, privilegeToAdd);
    return Status::OK();
}