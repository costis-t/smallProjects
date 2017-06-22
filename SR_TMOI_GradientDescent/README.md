# Gradient Descent (Steepest Descent) in R, RcppEigen and Rcpp Armadilo
I translate and make some speed improvements of an R implementation for RcppArmadillo and RcppEigen from [stackoverflow][1].
Since there aren't any complicated/nested loops, using Rcpp* packages has minimal (if any) improvements over R and the differences are only due to small overheads and to the different Basic Linear Algebra Subprograms (BLAS/LAPACK) they use; I have not delved into them, though.

The challenge requests to consider only one independent variable, however the implementations can be used for multiple independent variables, too.

The License is GPLv3.

## Usage
To use, clone the repository (or extract the files) and run the `Gradient_Descent_Challenge.R`. I assume that the necessary R packages are already installed; if not, to install them run 

```R
    sapply(c('data.table', 'Hmisc', 'lattice', 
             'ggplot2', 'GGally', 'Rcpp', 'RcppEigen', 
             'RcppArmadillo', 'inline'), install.packages)
```

You should also have a C++ compiler installed (to check, run `system('g++ -v')` within R).
In case you do not meet the dependencies, [google][2] will present you with plenty of resources. 

## Data
I use the [Credit Card Fraud Detection][3] dataset from [kaggle][3].
The dataset is clean, without missing values.
The dataset is temporarily included in the current repository.

## Implementation / Mini-Interpretation
The R, RcppEigen and RcppArmadillo implementations run for mulltiple regressions, too. Following the challenge, I choose only two variables: `V2` and `Amount`.
The implementations are numerically almost identical to the regression coefficients (`.lm.fit(X, y)$coef`), as I demonstrate in `Gradient_Descent_Challenge.R`.

```R
    gd_R$theta
    # [1]  88.34962 -80.49067

    as.vector(gd_arma$theta)
    # [1]  88.34962 -80.49135
    
    as.vector(gd_eigen$theta)
    # [1]  88.34962 -80.49135
    
    .lm.fit(X, y)$coef
    # [1]  88.34962 -80.49135
```

## Plot
[![enter image description here][4]][4]

## Benchmarks
The speed differences among the implementations are negligible and only exist due to small overheads. 
Every implementation uses already optimized linear algebra subprograms (BLAS/LAPACK/etc.).
Thus, although RcppEigen in most of the benchmarks outperforms R by 0.5-5%, I consider the speed of the different implementations about the same.
We do not have any complicated loop constructs and profiling indicates that the main computational cost is due to matrix multiplication.

```R
    benchmark(R.way, Cpp.eigen.way, Cpp.armadillo.way, replications = 10^6)
    #                test replications elapsed relative user.self sys.self 
    # 3 Cpp.armadillo.way      1000000   1.769    1.001     1.765    0.003
    # 2     Cpp.eigen.way      1000000   1.767    1.000     1.764    0.004
    # 1             R.way      1000000   1.863    1.054     1.852    0.010
```


  [1]: https://stackoverflow.com/questions/43217617/estimating-linear-regression-with-gradient-descent-steepest-descent
  [2]: http://www.google.com
  [3]: https://www.kaggle.com/dalpozz/creditcardfraud
  [4]: https://i.stack.imgur.com/lJUq2.png
