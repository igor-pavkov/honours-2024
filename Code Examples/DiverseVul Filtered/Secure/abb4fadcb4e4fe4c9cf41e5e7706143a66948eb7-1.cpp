double ompl::geometric::VFRRT::biasedSampling(const Eigen::VectorXd &vrand, const Eigen::VectorXd &vfield,
                                              double lambdaScale)
{
    double sigma = .25 * (vrand - vfield).squaredNorm();
    updateGain();
    double scaledLambda = lambda_ * lambdaScale / meanNorm_;
    double phi = scaledLambda / (1. - std::exp(-2. * scaledLambda));
    double z = -std::log(1. - sigma * scaledLambda / phi) / scaledLambda;
    return std::sqrt(2. * z);
}