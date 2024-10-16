Status RoleGraph::createRole(const RoleName& role) {
    if (roleExists(role)) {
        return Status(ErrorCodes::DuplicateKey,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " already exists");
    }

    _createRoleDontCheckIfRoleExists(role);
    return Status::OK();
}