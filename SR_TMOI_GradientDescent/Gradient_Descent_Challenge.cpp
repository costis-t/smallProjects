//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
#include <RcppArmadillo.h> // do not include Rcpp.h!
#include <RcppEigen.h> 
// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::depends(RcppEigen)]]

using namespace Rcpp;
using namespace arma;
using namespace Eigen;

// Translated from R code at https://stackoverflow.com/questions/43217617/estimating-linear-regression-with-gradient-descent-steepest-descent

// [[Rcpp::export]]
Rcpp::List GD_Cpp_eigen(NumericVector &YR, NumericMatrix &XR, NumericMatrix &thetaR, double alpha = 1, int max_iterations = 1000) {
	const Map<MatrixXd> X(as<Map<MatrixXd> >(XR));
	Map<MatrixXd> theta(as<Map<MatrixXd> >(thetaR));
	const Map<VectorXd> Y(as<Map<VectorXd> >(YR));
	int nrows = X.rows(), kcols = X.cols();  
	double oldCost = ((X * theta - Y).array().pow(2).sum() * 0.5 / nrows);
	MatrixXd error;
	MatrixXd delta = MatrixXd::Ones(kcols, 1); 
	double trialCost;
	MatrixXd trialTheta = theta - alpha * delta;
	double max_delta = delta.array().abs().maxCoeff();
	int iteration = 0;
	while (max_delta > 1e-5) {
		error = X * theta - Y;
		delta = X.transpose() * error / nrows;
		max_delta = delta.array().abs().maxCoeff();
		trialTheta = theta - alpha * delta;
		trialCost = ((X * trialTheta - Y).array().pow(2).sum() * 0.5 / nrows);
		while ((trialCost >= oldCost)) {
			trialTheta = (theta + trialTheta) * 0.5;
			trialCost = ((X * trialTheta - Y).array().pow(2).sum() * 0.5 / nrows);
		}
		oldCost = trialCost;
		theta = trialTheta;
		iteration = iteration + 1;
		if (iteration == max_iterations)
			break;
	}
	return Rcpp::List::create(Rcpp::Named("Cost") = oldCost,
			Rcpp::Named("theta") = theta);
}


// [[Rcpp::export]]
Rcpp::List GD_Cpp_arma(const colvec &Y, const mat &X, mat &theta, double alpha = 1, int max_iterations = 1000) {
	int nrows = X.n_rows, kcols = X.n_cols;
	mat delta;
	delta.ones(kcols); 
	double oldCost = sum(pow(X * theta - Y, 2)) * 0.5 / nrows;
	mat max_matrix = max(abs(delta));
	mat error(nrows, kcols);
	mat trialTheta(kcols, 1) ;
	double trialCost;
	int iteration = 0;
	while (max_matrix(0, 0) > 1e-5) {
		error = X * theta - Y;
		delta = X.t() * error / nrows;
		max_matrix = max(abs(delta));
		trialTheta = theta - alpha * delta;
		trialCost = sum(pow(X * trialTheta - Y, 2)) * 0.5 / nrows;
		while (trialCost >= oldCost) {
			trialTheta = (theta + trialTheta) * 0.5;
			trialCost = sum(pow(X * trialTheta - Y, 2)) * 0.5 / nrows;
		}
		oldCost = trialCost;
		theta = trialTheta;
		//    Rcpp::Rcout << "iteration: " << iteration << ", max: " << max_matrix(0, 0) << std::endl;
		iteration = iteration + 1;
		if (iteration == max_iterations)
			//            Rcpp::Rcout << "iteration: " << iteration << ", max: " << max_iterations << std::endl;

			break;
	}

	return Rcpp::List::create(Rcpp::Named("Cost") = oldCost,
			Rcpp::Named("theta") = theta);
}
