Status RoleGraph::addPrivilegesToRole(const RoleName& role,
                                      const PrivilegeVector& privilegesToAdd) {
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

    for (PrivilegeVector::const_iterator it = privilegesToAdd.begin(); it != privilegesToAdd.end();
         ++it) {
        _addPrivilegeToRoleNoChecks(role, *it);
    }
    return Status::OK();
}