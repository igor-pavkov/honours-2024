Status RoleGraph::replaceRestrictionsForRole(const RoleName& role,
                                             SharedRestrictionDocument restrictions) {
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(role)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot remove restrictions from built-in role: "
                                                << role.getFullName());
    }
    _directRestrictionsForRole[role] = std::move(restrictions);
    return Status::OK();
}