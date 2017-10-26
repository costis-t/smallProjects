# Overview
In this sample, I analyze a sales time-series and I identify whether there is a permanent impact of marketing campaigns on a sales time-series.
I also forecast the sales time-series for an imminent marketing campaing using AMRAX modelling.


# R and Data preparation
## Required R packages
For the analysis, start by installing the necessary packages.
This step can be skipped if we have already installed the necessary packages.

imputeTS

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
In the rest of the document I skip such trivial details and I also take similar steps for both of the time-series (`webvisits` and `sales`) to keep the text short.
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
ggplot(data = DT, aes(date, sales)) + geom_line()
dev.off()
```

![Simple sales graph](figures/01-simple-sales-graph.pdf)

## Data quality
Let's see if we have any missing values; days with no data.
Create a sequence of dates from the first till the last day of `DT`.
```r
date.grid <- seq(as.Date(min(DT[, date])), as.Date(max(DT[, date])), by = 'day')
length(date.grid)
# [1] 1217
row(DT)
# [1] 1200
```

It seems we do have missing values.
Which are they? 
R comes with set operation functions (?setdiff) and we can use a set difference to find them.
```r
setdiff(as.character(date.grid), as.character(DT$date))
#  [1] "2014-03-01" "2014-05-07" "2014-05-24" "2014-08-09" "2014-08-13"
#  [6] "2014-09-07" "2014-09-13" "2014-10-01" "2014-10-11" "2014-10-17"
# [11] "2014-10-18" "2014-10-19" "2014-10-24" "2014-12-06" "2014-12-21"
# [16] "2014-12-24" "2015-09-19"
```
All in all, we have 17 missing values, located in the first half of the the time series. 
We observe see noisy seasonality and complex series so we use Kalman filter to impute the missing values (in simpler cases linear interpolation could also be fine).
Note, that we proceed to interpolation because it does not change the outcome of the analysis! 

```r
date.grid.DT <- data.table(date = c(date.grid))
setkeyv(DT, c('date'))
setkeyv(date.grid.DT, c('date'))

DT <- DT[date.grid.DT]

DT[, c('webvisits.withNA', 'sales.withNA') := .(webvisits, sales)]
DT[, c('webvisits', 'sales') := .(na.kalman(webvisits), na.kalman(sales))]

png(file= 'figures/02-first-150-imputed-values-for-sales-graph.png')
plotNA.imputations(DT[1:150, sales.withNA], DT[1:150, sales], ylab = 'sales', main = 'Visualization of the Imputed Values')
dev.off()
```

![Imputed values for sales graph](figures/02-first-150-imputed-values-for-sales-graph.png)

Now, we are ready to start our analysis!

Helicopter view of the data
============
As in theory, there seems to be four components in the time series (trend, cycle, seasonal, random). 
Data are very noisy. 
Ideally, we'd like to have a large increase during and after the marketing campaings, but this does not happen.
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

Let's create a beautiful graph for the data!
The following figure shows different outliers using the Hampel filter.
If we are further interested in the outliers, we should proceed to parameter calibration for the Hampel filter, but for this analysis it is not necessary.
Ideally, we forecast with and without the outliers, calibrate their corrections and identify whether they are indeed outliers or indicate a response to an event (which seems plausible for the 3 outliers during the marketing campaigns). 
Please refer to [Tsay (1988)](http://onlinelibrary.wiley.com/doi/10.1002/for.3980070102/abstract) and [Watson (2001)](http://eprints.whiterose.ac.uk/2209/1/ITS261_WP362_uploadable.pdf) for various outlier types (additive/level shifts/etc.) and techniques for their correction as a starting point. 
At a more advanced stage, we could use a non-linear moving window and parameter calibration.

The following figure, also includes three smoothing functions with different coarseness, following a LOESS fitted curve with correspondingly different parameters.

```r
campaign.dates <- data.frame(
                            campaign.start =  c(as.Date('2016-01-23'),
                                                as.Date('2016-09-15'),
					                            as.Date('2016-11-25'),
					                            as.Date('2017-01-25')
					                            ), campaign.end = c(
					                            as.Date('2016-02-01'),
					                            as.Date('2016-09-23'),
					                            as.Date('2016-11-29'),
					                            as.Date('2017-02-03')))

figure <-   ggplot(DT) +
	geom_line(aes(x = date, y = sales), colour = 'red', size = 0.1, alpha = 0.7) +
	geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.3)$fit), colour = 'darkgreen',  size = 1) +
	geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.05)$fit), colour = 'blue',  size = 0.4) +
	geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.95)$fit), colour = 'darkred',  size = 2, alpha = 0.4) +
	scale_x_date(  date_minor_breaks = '1 month', date_labels = '%Y', date_breaks = '1 year') +
    geom_rect(data = campaign.dates, aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +
    geom_point(aes(x = date, y = outliers.NAs)) +
    ggtitle('Sales Time-series') +
    ylab('Sales') + xlab('Date')+
    list()

ggsave(filename = 'figures/03-sales-graph.png', plot = figure, height = 90, units = 'mm')
```

The Sales Time-Series. 
Marketing campaign periods are grayed. 
Dots correspond to outliers following an uncalibrated Hampel filter.

![Sales graph](figures/03-sales-graph.png)

