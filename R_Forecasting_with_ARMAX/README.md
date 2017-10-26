# Overview
Forecasting with ARMAX

# R and Data preparation
## Required R packages
For the analysis, start by installing the necessary packages.
This step can be skipped if we have already installed the necessary packages.

```r
sapply(c('astsa', 'forecastxgb', 'TStools', 'smooth', 'GDMH', 'caret', 'nnet', 'tsoutliers', 'colorout', 'data.table', 'randomNames', 'xtable', 'lubridate', 'compare', 'ggplot2', 'zoo',
	 'scales', 'stringr', 'foreach',  'gridExtra',
	 'colorout', 
	 'tseries', 'urca',
	 # 'CADFtest',
	 'fUnitRoots', 
	 'sos', 
	 'stats', 
	 'lmtest', 
	 #'rasterVis' 
	 # 'bigalgebra', 'bigmemory', 'biglm',
	 'pracma', 
	 'Matrix',# for sparse
	 'slam', 'prophet', 'RcppArmadillo', 'Rcpp', 'padr',
	 'parallel', 'foreach', 'doParallel', 'shiny', #'lineprof',
	 'rbenchmark', 'profvis', 'formula.tools', 'sandwich',
	 'lattice', 'xts', 'imputeTS', 'strucchange', 'car', 'forecast', 'R.utils', 'GGally', 'zoom', 'TSA', 'foreign'
	 ), install.packages) # install packages
```

Next, load them with:
```r
sapply(c('astsa', 'forecastxgb', 'TStools', 'smooth', 'GDMH', 'caret', 'nnet', 'tsoutliers', 'colorout', 'data.table', 'randomNames', 'xtable', 'lubridate', 'compare', 'ggplot2', 'zoo',
	 'scales', 'stringr', 'foreach',  'gridExtra',
	 'colorout', 
	 'tseries', 'urca',
	 # 'CADFtest',
	 'fUnitRoots', 
	 'sos', 
	 'stats', 
	 'lmtest', 
	 #'rasterVis' 
	 # 'bigalgebra', 'bigmemory', 'biglm',
	 'pracma', 
	 'Matrix',# for sparse
	 'slam', 'prophet', 'RcppArmadillo', 'Rcpp', 'padr',
	 'parallel', 'foreach', 'doParallel', 'shiny', #'lineprof',
	 'rbenchmark', 'profvis', 'formula.tools', 'sandwich',
	 'lattice', 'xts', 'imputeTS', 'strucchange', 'car', 'forecast', 'R.utils', 'GGally', 'zoom', 'TSA', 'foreign'
	 ), require, character.only = TRUE) # load packages
```

## Loading and preparing the data series
I have saved the data series as `data/anonymized-forecasting-data.csv`.
We can confirm this within R (under a Linux OS):

```r
system('ls data/*.csv')
# data/anonymized-forecasting-data.csv
```

Using the amazing `data.table` package we import the data as a `DT` `data.table` object:
```r
DT <- fread('data/anonymized-forecasting-data.csv')
```

It's a good idea to check the structure and summary of the object.
```r
str(DT)
Classes ‘data.table’ and 'data.frame':  1200 obs. of  4 variables:
#  $ V1       : chr  "1" "2" "3" "4" ...
#  $ date     : chr  "2014-01-01" "2014-01-02" "2014-01-03" "2014-01-04" ...
#  $ webvisits: int  27 25 27 50 44 37 34 39 33 27 ...
#  $ sales    : int  6 119 74 63 49 131 106 137 37 23 ...
#  - attr(*, ".internal.selfref")=<externalptr> 

summary(DT)
#      date              webvisits           sales       
# Min.   :2014-01-01   Min.   :   0.00   Min.   :   0.0  
# 1st Qu.:2014-11-09   1st Qu.:  70.75   1st Qu.:  57.0  
# Median :2015-09-08   Median : 129.00   Median : 123.0  
# Mean   :2015-09-05   Mean   : 229.59   Mean   : 178.9  
# 3rd Qu.:2016-07-05   3rd Qu.: 338.25   3rd Qu.: 242.5  
# Max.   :2017-05-01   Max.   :1003.00   Max.   :1010.0  
```

Our data sources can have different layouts and some necessary adjustments may be required.
In our case, we remove the first index column, `V1`, and we fix the `date` column which currently is read as a string. Among others, converting the numbers from a character string to a `numeric` may also be required for other `.csv` files.
Of course, we could do the `date` conversion using the appropriate `fread()` option `colClasses` and drop the first column using the `drop` option.
In the rest of the document I skip such trivial details.
```r
DT[, c('V1', 'date') := .(NULL, as.Date(date))]
```

Now we have the following `DT`
```r
str(DT)
# Classes ‘data.table’ and 'data.frame':  1200 obs. of  3 variables:
#  $ date     : Date, format: "2014-01-01" "2014-01-02" ...
#  $ webvisits: int  27 25 27 50 44 37 34 39 33 27 ...
#  $ sales    : int  6 119 74 63 49 131 106 137 37 23 ...
#  - attr(*, ".internal.selfref")=<externalptr> 
```
I also create a simple time-series plot to see if everything looks fine.
```r
png(file= 'figures/01-simple-sales-graph.pdf')
plot.ts(DT$sales)
dev.off()
```

![Simple sales graph](figures/01-simple-sales-graph.pdf)

Now, we are ready to start our analysis!

Helicopter view of the data
============
As in theory, there seems to be four components in the time series (trend, cycle, seasonal, random). Data are very noisy. 
Ideally, we’d like to have a large increase during and after the marketing campaings, but this does not happen.
Since there is an intervention variable (Marketing Campains) an ARMAX model seems appropriate. 
Other instrumental variables (IVs) like the web visits time series can enter the ARMAX, too.

The following Figure shows that the `sales` seem to follow: 
 * a parallel movement in 2014-early:2015, 
 * a slightly upward trend during early:2015–early:2016, and 
 * a more vigorous upward trend since mid:2016 with some indication of a potential parallel movement after early:2017. 
 
This suggest that a regime-switching model could also be appropriate for such data. 
We don’t see linear, but a polynomial, trend for the whole time-series, which is common for sales data. 
We also see structural breaks (at least in level and variance) which influences the unit root tests and need further exploration. 
I follow Bai and Perron (2003) and the BIC criterion suggests 3 breaks (four would not make a large difference, thouggh) and confirms my visual perception.

