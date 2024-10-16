Eigen::VectorXd ompl::geometric::VFRRT::computeAlphaBeta(double omega, const Eigen::VectorXd &vrand,
                                                         const Eigen::VectorXd &vfield)
{
    double w2 = omega * omega;
    double c = vfield.dot(vrand);
    double cc_1 = c * c - 1.;
    double root = std::sqrt(cc_1 * w2 * (w2 - 4.));
    double beta = -root / (2. * cc_1);
    double sign = (beta < 0.) ? -1. : 1.;
    beta *= sign;
    double alpha = (sign * c * root + cc_1 * (2. - w2)) / (2. * cc_1);
    return alpha * vfield + beta * vrand;
}