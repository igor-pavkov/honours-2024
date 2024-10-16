Expression::ComputedPaths ExpressionMap::getComputedPaths(const std::string& exprFieldPath,
                                                          Variables::Id renamingVar) const {
    auto inputFieldPath = dynamic_cast<ExpressionFieldPath*>(_input.get());
    if (!inputFieldPath) {
        return {{exprFieldPath}, {}};
    }

    auto inputComputedPaths = inputFieldPath->getComputedPaths("", renamingVar);
    if (inputComputedPaths.renames.empty()) {
        return {{exprFieldPath}, {}};
    }
    invariant(inputComputedPaths.renames.size() == 1u);
    auto fieldPathRenameIter = inputComputedPaths.renames.find("");
    invariant(fieldPathRenameIter != inputComputedPaths.renames.end());
    const auto& oldArrayName = fieldPathRenameIter->second;

    auto eachComputedPaths = _each->getComputedPaths(exprFieldPath, _varId);
    if (eachComputedPaths.renames.empty()) {
        return {{exprFieldPath}, {}};
    }

    // Append the name of the array to the beginning of the old field path.
    for (auto&& rename : eachComputedPaths.renames) {
        eachComputedPaths.renames[rename.first] =
            FieldPath::getFullyQualifiedPath(oldArrayName, rename.second);
    }
    return eachComputedPaths;
}