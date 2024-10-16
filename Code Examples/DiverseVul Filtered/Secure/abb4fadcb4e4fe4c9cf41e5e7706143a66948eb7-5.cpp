void ompl::geometric::VFRRT::clear()
{
    RRT::clear();
    efficientCount_ = 0;
    inefficientCount_ = 0;
    explorationInefficiency_ = 0.;
    step_ = 0;
}