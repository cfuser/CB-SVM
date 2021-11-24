#pragma once
#define _USE_MATH_DEFINES
#include "Eigen/Eigen"
#include <math.h>

/*
theory and code from BLOG: http://blog.sarantop.com/notes/mvn
*/
class MVN
{
public:
	MVN(const Eigen::VectorXd& mu, const Eigen::MatrixXd &sigma)
	{
		this->mean = mu;
		this->sigma = sigma;
	}
	~MVN()
	{
		
	}
	// pdf means Probability Density function
	double pdf(const Eigen::VectorXd& x) const;
	Eigen::VectorXd sample(unsigned int nr_iterations = 20) const;
	Eigen::VectorXd mean;
	Eigen::MatrixXd sigma;
};

double MVN::pdf(const Eigen::VectorXd& x) const
{
	double n = x.rows();
	double sqrt2pi = std::sqrt(2 * M_PI);
	double quadform = (x - mean).transpose() * sigma.inverse() * (x - mean);
	double norm = std::pow(sqrt2pi, -n) * std::pow(sigma.determinant(), -0.5);
	
	return norm * exp(-0.5 * quadform);
}


Eigen::VectorXd MVN::sample(unsigned int nr_iterations) const
{
	int n = mean.rows();
	
	Eigen::VectorXd x(n);
	Eigen::VectorXd sum(n);
	sum.setZero();
	
	for (unsigned int i = 0; i < nr_iterations; i++)
	{
		x.setRandom(); // [-1, 1]
		x = 0.5 * (x + Eigen::VectorXd::Ones(n));
		sum = sum + x;
	}

	sum = sum - (static_cast<double>(nr_iterations) / 2) * Eigen::VectorXd::Ones(n);
	x = sum / (std::sqrt(static_cast<double>(nr_iterations) / 12));

	// Find the Eigen vectors of the covariance matrix
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen_solver(sigma);
	Eigen::MatrixXd eigen_vectors = eigen_solver.eigenvectors().real();

	// Find the Eigen values of covariance matrix
	Eigen::MatrixXd eigen_values = eigen_solver.eigenvalues().real().asDiagonal();

	// Find the transformation matrix
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(eigen_values);
	Eigen::MatrixXd sqrt_eigen_values = es.operatorSqrt();
	Eigen::MatrixXd Q = sqrt_eigen_values * eigen_vectors;

	return Q * x + mean;
}
