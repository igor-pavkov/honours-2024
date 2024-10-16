void ompl::geometric::VFRRT::updateGain()
{
    if (step_ == nth_step_)
    {
        lambda_ = lambda_ * (1 - explorationInefficiency_ + explorationSetting_);
        efficientCount_ = inefficientCount_ = 0;
        explorationInefficiency_ = 0;
        step_ = 0;
    }
    else
        step_++;
}