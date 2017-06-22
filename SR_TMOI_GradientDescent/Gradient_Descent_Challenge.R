#! /bin/R

# Thanks for your videos! They are really fun! :D

library(data.table)
library(Rcpp)
library(RcppArmadillo)
library(RcppEigen)
library(rbenchmark) 
library(ggplot2)
# library(Hmisc) # optional
# library(lattice) # optional
# library(GGally) # optional
# library(inline) # optional
# library(stats) # optional
# library(colorout) # optional

# If you haven't installed the packages please install them with:
# sapply(c('data.table', 'Hmisc', 'lattice', 'ggplot2', 'GGally', 'Rcpp', 'RcppEigen', 'RcppArmadillo', 'inline'), install.packages) # I hope you have already installed their dependencies (c++ and perhaps fortran compiler); I work on a linux system and if you have trouble somewhere, please let me know.


# The dataset is: https://www.kaggle.com/dalpozz/creditcardfraud/downloads/creditcardfraud.zip
# Download it and extract it, please, because using library(RCurl) to do it within R requires my credentials (more information at https://stackoverflow.com/questions/35303779/downloading-kaggle-zip-files-in-r )
# Run the R script in the dataset's folder.

system('ls *.csv')
# creditcard.csv

DT <- fread(file = 'creditcard.csv')

# rcor <- rcorr(as.matrix(DT[, -ncol(DT), with = FALSE]))
# abs(rcor$r) > 0.5
# DTsample <- DT[, .(V2, V12, V22, Amount)] # I choose V2~Amount because of the high correlation. The simple Gradient Descent implementations below can deal with multivariate cases, too.
#  
# ggpairs(DTsample, 
#         lower = list(continuous = wrap('points', shape = I('.'))), 
#         upper = list(combo = wrap('box', outlier.shape = I('.'))))

# I translate an R implementation for RcppArmadillo and RcppEigen from https://stackoverflow.com/questions/43217617/estimating-linear-regression-with-gradient-descent-steepest-descent .
# Since there are not extensive/nested loops, using Rcpp* packages has minimal (if any) improvements over R and the differences are only due to small overheads and different Basic Linear Algebra Subprograms (BLAS); I have not delved into them, though.
# There's already an Armadillo-based implementation (but I have not tested it) at https://gist.github.com/pin3da/8995686


#############################################################
#### Implementation of the simple Gradient Descent in R  ####
#############################################################

# # theta: initial guess on regression coef
# # alpha: initial step scaling factor
GD_R <- function(X, y, theta, alpha, max_iterations = 1000) {
	delta <- rep(1, ncol(X))
	old.cost <- sum((X %*% theta - y)^2 * 0.5)/(length(y))
	iteration <- 0
	while (max(abs(delta)) > 1e-5) {
		error <- X %*% theta - y
		delta <- crossprod(X, error) / length(y)
		trial.theta <- theta - alpha * c(delta)
		trial.cost <- sum((X %*% trial.theta - y)^2 * 0.5)/(length(y))
		while (round(trial.cost, 5)  > round(old.cost, 5)) {
			trial.theta <- (theta + trial.theta) * 0.5
			trial.cost <- sum((X %*% trial.theta - y)^2 * 0.5)/(length(y))
		}
		old.cost <- trial.cost
		theta <- trial.theta
		iteration <- iteration + 1
		if (iteration == max_iterations) break
	}
	list(cost = trial.cost, theta = trial.theta)
}

#######################################################################
#### Implementation of the simple Gradient Descent with RcppEigen  ####
#######################################################################

# GD_Cpp_eigen of Gradient_Descent_Challenge.cpp
sourceCpp(file = 'Gradient_Descent_Challenge.cpp')

###########################################################################
#### Implementation of the simple Gradient Descent with RcppArmadillo  ####
###########################################################################

# same as in RcppEigen: sourceCpp(file = 'Gradient_Descent_Challenge.cpp')

###########################################################################
#### Create the matrices and show the results  ####
###########################################################################

y <- matrix(DT[, Amount], nrow = nrow(DT), ncol = 1)
X <-  matrix(c(rep(1, nrow(DT)), DT[, V2]), nrow = nrow(DT), ncol = 2)

theta <- matrix(0, nrow = ncol(X), ncol = 1) # initial values of the coefficients (zero)
gd_R <- GD_R(X, y, as.vector(theta), alpha = 1, max_iterations = 1000)

theta <- matrix(0, nrow = ncol(X), ncol = 1) # initial values of the coefficients (zero)
gd_arma <- GD_Cpp_arma(Y = y, X = X, theta = as.matrix(theta), alpha = 1, max_iterations = 1000)

theta <- matrix(0, nrow = ncol(X), ncol = 1) # initial values of the coefficients (zero)
gd_eigen <- GD_Cpp_eigen(YR = y, XR = X, thetaR = as.matrix(theta), alpha = 1, max_iterations = 1000)

gd_R$theta
# [1]  88.34962 -80.49067

as.vector(gd_arma$theta)
# [1]  88.34962 -80.49135

as.vector(gd_eigen$theta)
# [1]  88.34962 -80.49135

.lm.fit(X, y)$coef
# [1]  88.34962 -80.49135


###########################################################################
#### Benchmarks  ####
###########################################################################

R.way <- function() {
	theta <- matrix(0, nrow = ncol(X), ncol = 1)
	GD_R(X, y, as.vector(theta), alpha = 1, max_iterations = 1000)
}

Cpp.eigen.way <- function() {
	theta <- matrix(0, nrow = ncol(X), ncol = 1)
	GD_Cpp_eigen(YR = y, XR = X, thetaR = as.matrix(theta), alpha = 1, max_iterations = 1000)
}

Cpp.armadillo.way <- function() {
	theta <- matrix(0, nrow = ncol(X), ncol = 1)
	GD_Cpp_arma(Y = y, X = X, theta = as.matrix(theta), alpha = 1, max_iterations = 1000)
}

# .lm.fit(X, y)$coef is slow and I ommit it.
benchmark(R.way, Cpp.eigen.way, Cpp.armadillo.way, replications = 10^6)
#                test replications elapsed relative user.self sys.self user.child
# 3 Cpp.armadillo.way      1000000   1.769    1.001     1.765    0.003          0
# 2     Cpp.eigen.way      1000000   1.767    1.000     1.764    0.004          0
# 1             R.way      1000000   1.863    1.054     1.852    0.010          0


# The differences are negligible due to small overheads. 
# Although RcppEigen in most of the benchmarks outperforms R by 0.5-5%, I consider the speed of the different implementations about the same because every method uses already optimized linear algebra subpgrams as also explained above.
# Profiling showed that, since we do not have any complicated loop constructs, the main computational cost is due to matrix multiplication; since R already uses very optimized linear algebra subprograms there aren't any significant computational gains from Rcpp. 


###########################################################################
#### Plot  ####
###########################################################################


figure <- 
	ggplot(data = DT, aes(x = V2, y = Amount)) +
	geom_point(aes(x = V2, y = Amount)) +
	geom_smooth(method='lm', fullrange = TRUE, se = TRUE, alpha = 0.15) +
	geom_abline(aes(colour = 'R', intercept = gd_R$theta[1], slope = gd_R$theta[2]), linetype = 'solid', size = 1) +
	geom_abline(aes(colour = 'RcppArmadillo', intercept = as.vector(gd_arma$theta)[1], slope = as.vector(gd_arma$theta)[2]), linetype = 'dashed', size = 2) +
	geom_abline(aes(colour = 'RcppEigen', intercept = as.vector(gd_eigen$theta)[1], slope = as.vector(gd_eigen$theta)[2]), linetype = 'dotted', size = 3) +
	scale_color_manual(values = c( 'R' = 'red', 'RcppArmadillo' = 'green', 'RcppEigen' = 'blue')) +
	labs(colour = 'Implementation') +
	ggtitle(label = 'Gradient Descent (Steepest Descent)', subtitle = 'implementation in R, RcppArmadillo and RcppEigen' ) +
	theme(plot.title = element_text(hjust = 0.5), plot.subtitle = element_text(hjust = 0.5), legend.title= element_text(size = 10)) +
	list()

ggsave(filename = 'Gradient_Descent_3_implementations.png', plot = figure, height = 140, units = 'mm')
ggsave(filename = 'Gradient_Descent_3_implementations.pdf', plot = figure, height = 140, units = 'mm')


