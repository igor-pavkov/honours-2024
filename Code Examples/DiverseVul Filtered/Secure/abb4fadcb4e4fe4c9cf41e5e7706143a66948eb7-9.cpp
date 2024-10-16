void ompl::geometric::VFRRT::updateExplorationEfficiency(Motion *m)
{
    Motion *near = nn_->nearest(m);
    if (distanceFunction(m, near) < si_->getStateValidityCheckingResolution())
        inefficientCount_++;
    else
        efficientCount_++;
    explorationInefficiency_ = inefficientCount_ / (double)(efficientCount_ + inefficientCount_);
}