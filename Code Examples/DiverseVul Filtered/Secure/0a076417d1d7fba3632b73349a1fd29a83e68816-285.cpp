void ExpressionMeta::_doAddDependencies(DepsTracker* deps) const {
    if (_metaType == MetaType::TEXT_SCORE) {
        deps->setNeedTextScore(true);
    }
}