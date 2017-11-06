# Overview - under editing
In this sample tutorial, I analyze a sales time-series and I identify whether there is a permanent impact of marketing campaigns on a sales time-series.
I also forecast the sales time-series for one month with and without an imminent marketing campaing using ARMAX modelling.

#### Table of Contents
- [1. R and Data preparation ](#1-r-and-data-preparation)
    - [1.1. Required R packages](#11-required-r-packages)
    - [1.2. Load and prepare the data series](1.2.-load-and-prepare-the-data-series)
    - [1.3. Data quality](#13-data-quality)
- [2. Analysis](#2-analysis)
    - [2.1. Helicopter view of the data](#21-helicopter-view-of-the-data)
    - [2.2. Graph with outliers and smoothing curves](#22-graph-with-outliers-and-smoothing-curves)
    - [2.3. Statistical characteristics](#23-statistical-characteristics)
       - [2.3.1. Autocorrelation structure](#231-autocorrelation-structure)
       - [2.3.2. Breaks](#232-breaks)
       - [2.3.3. Stationarity](#233-stationarity)
    - [2.4. ARIMAX modelling](#24-arimax-modelling)
- [3. Forecasting](#3-forecasting)
    - [3.1. Forecasting without breaks](#31-forecasting-without-breaks)
    - [3.2. Forecasting without breaks](#32-forecasting-without-breaks)
- [4. Decision making](#4-decision-making)
- [5. Expansions](#5-expansions)


# 1. R and Data preparation 
## 1.1. Required R packages
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

## 1.2. Load and prepare the data series
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
# Classes ‘data.table’ and 'data.frame':  1200 obs. of  4 variables:
#  $ V1       : chr  "1" "2" "3" "4" ...
#  $ date     : chr  "2014-01-01" "2014-01-02" "2014-01-03" "2014-01-04" ...
#  $ webvisits: int  27 25 27 50 44 37 34 39 33 27 ...
#  $ sales    : int  6 119 74 63 49 131 106 137 37 23 ...
#  - attr(*, ".internal.selfref")=<externalptr> 

summary(DT)
#       V1                date             webvisits           sales       
#  Length:1200        Length:1200        Min.   :   0.00   Min.   :   0.0  
#  Class :character   Class :character   1st Qu.:  70.75   1st Qu.:  57.0  
#  Mode  :character   Mode  :character   Median : 129.00   Median : 123.0  
#                                        Mean   : 229.59   Mean   : 178.9  
#                                        3rd Qu.: 338.25   3rd Qu.: 242.5  
#                                        Max.   :1003.00   Max.   :1010.0 
```

Our data sources can have different layouts and some necessary adjustments may be required.
In our case, we remove the first index column, `V1`, and we fix the `date` column which currently is read as a string. Among others, converting the numbers from a character string to a `numeric` may also be required for other `.csv` files.
Of course, we could do the `date` conversion using the appropriate `fread()` option `colClasses` and drop the first column using the `drop` option.
In the rest of the document I skip such trivial details and I also take similar steps for both of the time-series (`webvisits` and `sales`) to keep the text short.
```r
DT[, c('V1', 'date') := .(NULL, as.Date(date))]
```

Now we have the following `DT`:
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
ggplot(data = DT, aes(date, sales)) + geom_line() + ggtitle('1. Sales simple graph') + ylab('Sales') + xlab('Date')
dev.off()
```

![Simple sales graph](figures/01-simple-sales-graph.pdf)

## 1.3. Data quality
Let's see if we have any missing values; days with no data.
Create a sequence of dates from the first till the last day of `DT` and compare the lengths of the original dataset to the sequence of dates.
```r
date.grid <- seq(as.Date(min(DT[, date])), as.Date(max(DT[, date])), by = 'day')
length(date.grid)
# [1] 1217
nrow(DT)
# [1] 1200
```

It seems we do have 17 missing values.
Which are they? 
R comes with set operation functions (?setdiff) and we can use a set difference to find them.
```r
setdiff(as.character(date.grid), as.character(DT$date))
#  [1] "2014-03-01" "2014-05-07" "2014-05-24" "2014-08-09" "2014-08-13"
#  [6] "2014-09-07" "2014-09-13" "2014-10-01" "2014-10-11" "2014-10-17"
# [11] "2014-10-18" "2014-10-19" "2014-10-24" "2014-12-06" "2014-12-21"
# [16] "2014-12-24" "2015-09-19"
```
All in all, the 17 missing values are located in the first half of the the time series. 
From the graph above, we observe noisy seasonality and complex series.
Hence, we use a Kalman filter to impute the missing values (in simpler cases linear interpolation could also be fine).
Note, that we proceed to such an interpolation because it does not change the outcome of the analysis! 

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

# 2. Analysis 
## 2.1. Helicopter view of the data

As in theory, there seems to be four components in the time series (trend, cycle, seasonal, random error). 
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
I follow [Bai and Perron (2003)](http://onlinelibrary.wiley.com/doi/10.1002/jae.659/abstract) and the BIC criterion suggests 3 breaks (four would not make a large difference, thouggh) and confirms the visual perception.

## 2.2. Graph with outliers and smoothing curves
Let's create a beautiful graph for the data!

First, we detect the outliers using the Hampel filter.
```r
outliers.sales <- DT[, sales]
plot(outliers.sales, type = 'l')
omad <- hampel(outliers.sales, k = 20, t0 = 3)
plot(outliers.sales, type='l')
points(omad$ind, outliers.sales[omad$ind], pch=21, col='red')
outliers.NAs <- rep(NA, length(DT[, sales])) 
outliers.NAs[omad$ind] <- DT[, sales][omad$ind]
```

If we are further interested in the outliers, we should proceed to parameter calibration for the Hampel filter, but for this tutorial analysis it is not necessary.
Ideally, we forecast with and without the outliers, calibrate their corrections and identify whether they are indeed outliers or indicate a response to an event (which seems plausible for the 3 outliers during the marketing campaigns). 
Please refer to [Tsay (1988)](http://onlinelibrary.wiley.com/doi/10.1002/for.3980070102/abstract) and [Watson (2001)](http://eprints.whiterose.ac.uk/2209/1/ITS261_WP362_uploadable.pdf) for various outlier types (additive/level shifts/etc.) and techniques for their correction as a starting point. 
For example, at a more advanced stage, we could use a non-linear moving window and parameter calibration.

The following figure, also includes three smoothing functions with different coarseness, following a LOESS fitted curve with correspondingly different parameters.

```r
campaign.dates <- data.frame(campaign.start =  c(as.Date('2016-01-23'),
                                                as.Date('2016-09-15'),
                                                as.Date('2016-11-25'),
                                                as.Date('2017-01-25')
                                                ), campaign.end = c(
                                                as.Date('2016-02-01'),
                                                as.Date('2016-09-23'),
                                                as.Date('2016-11-29'),
                                                as.Date('2017-02-03')))

figure <- ggplot(DT) +
          geom_line(aes(x = date, y = sales), colour = 'red', size = 0.1, alpha = 0.7) +
          geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.3)$fit), colour = 'darkgreen',  size = 1) +
          geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.05)$fit), colour = 'blue',  size = 0.4) +
          geom_line(aes(x = date, y = loess(sales ~ c(1:length(sales)), span = 0.95)$fit), colour = 'darkred',  size = 2, alpha = 0.4) +
          scale_x_date(  date_minor_breaks = '1 month', date_labels = '%Y', date_breaks = '1 year') +
          geom_rect(data = campaign.dates, aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +
          geom_point(aes(x = date, y = outliers.NAs)) +
          labs(title = '3. The Sales Time-Series',
               subtitle = 'Marketing campaign periods are grayed. \nDots correspond to outliers following an uncalibrated Hampel filter.\nIncludes three smoothing functions with different coarseness.',
               y = 'Sales',
               x = 'Date') + 
          list() # I use + list() in the end as a little trick to add more options at will by commenting/uncommenting the corresponding lines
      
ggsave(filename = 'figures/03-sales-graph.png', plot = figure, height = 90, units = 'mm')
```

![Sales graph](figures/03-sales-graph.png)

## 2.3. Statistical characteristics
### 2.3.1. Autocorrelation structure

I start with correlation analysis.

```r
png(file= 'figures/04-ACF-and-PACF-of-sales.png')
tsdisplay(DT[, sales], main = "4. DT[, sales]")
dev.off()
```
![ACF and PACF graph](figures/04-ACF-and-PACF-of-sales.png)

Figure 4 shows that the ACF slope declines smoothly with shallow peaks every 7 periods (days). 
The peaks after the 7th day can be considered weak statistically, since they can be related to the aforementioned weekly seasonality. 
The smooth decline is indicative for the existence of a trend. 
Hence, the ACF implies trend and seasonality. 
The graph of the PACF confirms, since the statistical
significance of the peaks after the 7th period seems week.
For further analysis, we could also examine lagged cross-correlation between `sales` and `webvisits` (prewhitening the series and using the `ccf()` function in R). 
Cross-correlation would indicate whether people examine thoroughly the product before their order.

Moreover, the sales time-series graph of the previous section shows a gradual increase in variance over time while the level of enquiries also increases. 
This could be an indication of ARCH errors/innovations if it didn’t seem so gradual but clustered. 
Hence, I could transform the enquiries with a power transformation, which seems to be the best simple transformation. 
The optimal Box-Cox λ refers to a logarithmic transformation and transposes the increased variance position to the beginning of the series.
For experimentation, please refer to the `BoxCox.lambda()` and `BoxCox()` functions of the `forecast` package in R.
Because there are breaks and the analysis concerns a simple tutorial, I do not proceed to such a transformation.

### 2.3.2. Breaks
Ideally, we do not want mean, variance, autocorrelation and relationship breaks for the time-series for sound inference. 
[Hansen (2012)](https://www.ssc.wisc.edu/~bhansen/crete/crete5.pdf) says there is no good theory for forecasting for series with breaks and [Perron (2017)](http://www.mdpi.com/2225-1146/5/2/22/pdf) presents recent papers which deal with structural changes and stationarity. 
According to [Pesaran and Timmermann (2007)](https://www.sciencedirect.com/science00/article/pii/S0304407606000418), in not an identical case, however, the optimal window for starts a bit before the last break. 
Bai and Perron (2003) method indicates considering three breaks as optimal. 
Even in non optimal cases with 2, 4 or 5 breaks the last break is on 2016-10-30. Hence, I consider 2016-09-01 as the start of my estimation window. 
For further exploration, Pesaran and Timmermann propose a calculation for the ideal estimation windows which I skip as it does not seem so critical for the purpose of this analysis.
Another possibility would be to consider outlier events, like level shifts, but there is no need for extensive intervention analysis throughout the series for this analysis.

For this simple tutorial analysis, to locate the breaks we work with the `tsoutliers` package in R.
```r
my.ts.DT <- DT[, .(date, sales, webvisits)] 
day.of.year <- as.numeric(format(my.ts.DT[, date][1], '%j'))
my.ts <- ts(my.ts.DT[, sales], start = c(2014, day.of.year), frequency = 365)
outliers.tso <- tso(my.ts) # it takes some minutes
plot(outliers.tso)
bp.ri <- breakpoints(my.ts ~ 1)
summary(bp.ri)
# 
#          Optimal (m+1)-segment partition: 
# 
# Call:
# breakpoints.formula(formula = my.ts ~ 1)
# 
# Breakpoints at observation number:
#                             
# m = 1                   985 
# m = 2           742     1034
# m = 3       481     839 1034
# m = 4   213 481     839 1034
# m = 5   213 481 663 845 1034
# 
# Corresponding to breakdates:
#                                                          
# m = 1                                           2016(255)
# m = 2                       2016(12)            2016(304)
# m = 3             2015(116)           2016(109) 2016(304)
# m = 4   2014(213) 2015(116)           2016(109) 2016(304)
# m = 5   2014(213) 2015(116) 2015(298) 2016(115) 2016(304)
# 
# Fit:
#                                                          
# m   0        1        2        3        4        5       
# RSS 36382654 13979836 10330573  9390829  9328042  9281676
# BIC    16010    14860    14506    14404    14410    14418

plot(bp.ri) # for three breaks
plot(my.ts, type = 'l')
lines(fitted(bp.ri, breaks = 3), col = 4)
lines(confint(bp.ri, breaks = 3))
```

Hence, for the day of the break, we choose the 1034th day of our time-series which corresponds to 30th of October of 2016:
```r
my.ts.DT[1034]
#          date sales webvisits
# 1: 2016-10-30   273       455
```
For further exploration, the `strucchange` package in R is a good starting point.
Example graphs:
```r
plot(Fstats(my.ts ~ 1))
plot(efp(my.ts ~ 1, type = 'Rec-CUSUM'))
plot(efp(my.ts ~ 1, type = 'OLS-CUSUM'))
```

The following graph presents the estimation window for the forecast in order to have a better view of the data series.
```r
my.ts <- ts(my.ts.DT[date >= '2016-09-01', sales], start = c(2014, 245), frequency = 365)
plot.ts(my.ts)

campaign.dates.truncated <- data.frame(
				   campaign.start =  c(
						   #                 as.Date('2016-01-23'),
						   as.Date('2016-09-15'),
						   as.Date('2016-11-25'),
						   as.Date('2017-01-25')
						   ), campaign.end = c(
						   #                 as.Date('2016-02-01'),
						   as.Date('2016-09-23'),
						   as.Date('2016-11-29'),
						   as.Date('2017-02-03')))


figure <-   ggplot(DT[date >= '2016-09-01']) +
	geom_line(aes(x = date, y = sales), colour = 'red', size = 0.4, alpha = 0.7)  +
	geom_rect(data = campaign.dates.truncated, aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +

	#              scale_x_date(  date_minor_breaks = '1 month', date_labels = '%Y',
	scale_x_date( date_minor_breaks = '1 week',  date_labels = '%m - %Y', # http://strftime.org/
		     date_breaks = '1 month'                        ) +
#             geom_point(aes(x = date, y = outliers.NAs)) +
ggtitle('5. Sales, estimation window') +
ylab('Sales') + xlab('Date')+
list()

ggsave(filename = 'figures/10-graph-estimation-window.png', plot = figure, height = 100, units = 'mm')
```

![Sales Estimation Window](figures/10-graph-estimation-window.png)
Following  [Bai and Perron (2003)](http://onlinelibrary.wiley.com/doi/10.1002/jae.659/abstract), the estimation window contains 3 breaks:

```r
breakpoints(my.ts ~ 1)
#          Optimal 4-segment partition: 
# Call:
# breakpoints.formula(formula = my.ts ~ 1)
# 
# Breakpoints at observation number:
# 74 118 164 
# 
# Corresponding to breakdates:
# 2059(3) 2065(5) 2072(2) 
```

To keep this tutorial analysis simple, I consider only one break in mean on 2016-12-25, which I let it enter the ARMAX model as an external regressor and I do not proceed to a BoxCox transformation.

```r
estimation.window.DT <- my.ts.DT[date >= '2016-09-01']
estimation.window.ts <- ts(estimation.window.DT[, sales], start = c(2016, as.integer(strftime('2016-09-01', format = '%j'))), frequency = 365)

outliers.tso <- tso(estimation.window.ts) # it takes some minutes

plot(outliers.tso)
bp.ri <- breakpoints(estimation.window.ts ~ 1)
summary(bp.ri)
#          Optimal (m+1)-segment partition: 
# 
# Call:
# breakpoints.formula(formula = estimation.window.ts ~ 1)
# 
# Breakpoints at observation number:
#                          
# m = 1         116        
# m = 2         116 164    
# m = 3      74 118 164    
# m = 4   46 82 118 164    
# m = 5   46 82 118 164 200
# 
# Corresponding to breakdates:
#                                                                            
# m = 1                                     2016.98356164384                 
# m = 2                                     2016.98356164384 2017.11506849315
# m = 3                    2016.86849315069 2016.98904109589 2017.11506849315
# m = 4   2016.79178082192 2016.8904109589  2016.98904109589 2017.11506849315
# m = 5   2016.79178082192 2016.8904109589  2016.98904109589 2017.11506849315
#                         
# m = 1                   
# m = 2                   
# m = 3                   
# m = 4                   
# m = 5   2017.21369863014
# 
# Fit:
#                                                    
# m   0       1       2       3       4       5      
# RSS 7382873 4716773 4383379 4140361 4101369 4096778
# BIC    3209    3111    3104    3101    3110    3121
plot(bp.ri) # BIC indicates three breaks
plot(estimation.window.ts, type = 'l')
lines(fitted(bp.ri, breaks = 3), col = 4)
lines(confint(bp.ri, breaks = 3))
estimation.window.DT[116]
#          date sales webvisits
# 1: 2016-12-25   151       346
```


### 2.3.3. Stationarity
We must also perform stationarity tests (see `adf.test()`, `pp.test()`, `kpss.test()` of the `tseries` package in R).
The series are non-stationary for the whole period and the selected estimation window.
```r
adf.test(my.ts, alternative = 'stationary')
#         Augmented Dickey-Fuller Test
# data:  my.ts
# Dickey-Fuller = -3.1108, Lag order = 6, p-value = 0.1085 # null of non-stationarity is not rejected at 0.05
# alternative hypothesis: stationary

pp.test(my.ts)
#         Phillips-Perron Unit Root Test
# data:  my.ts
# Dickey-Fuller Z(alpha) = -131.51, Truncation lag parameter = 4, p-value = 0.01 # null of stationarity is rejected at 0.05
# alternative hypothesis: stationary

kpss.test(my.ts)
#         KPSS Test for Level Stationarity
# 
# data:  my.ts
# KPSS Level = 2.8092, Truncation lag parameter = 3, p-value = 0.01 # null of stationarity is rejected at 0.05
```

The stationarity tests do not indicate a stochastic trend.
Nevertheless, there seems to be visually and with a test deterministic trend (`sales` increase by 0.372453 per day (as we understand from:`summary(lm(DT[, sales] ~ seq(1:length(DT[, sales]))))`)). 
Note that proper trend analysis in serially correlated series like in our case demands the calculation of Vogelsang ([1998a](https://rmgsc.cr.usgs.gov/outgoing/threshold_articles/Vogelsang1998b.pdf), [1998b](https://www.jstor.org/stable/2527353?seq=1#page_scan_tab_contents)) statistics. 

Also, the stationarity tests, as well as the Box-Cox transformation above are heavily influenced by the presense of breaks.
For more theory, consult [Charfeddine and Gu\'egan](https://ac.els-cdn.com/S0378437112005407/1-s2.0-S0378437112005407-main.pdf?_tid=145a548e-bffb-11e7-bcb7-00000aacb360&acdnat=1509647256_f0029583e43c334803924ccb2077130c), [Glyn, Perera and Verma (2007)](https://www.upo.es/revistas/index.php/RevMetCuant/article/view/2065), [Carrion-i-Silvestre, Kim and Perron (2009)](https://www.jstor.org/stable/pdf/40388611.pdf), [Perron (2017)](http://www.mdpi.com/2225-1146/5/2/22/pdf).

## 2.4. ARIMAX modelling

`auto.arima()` provides a way to choose the optimal ARIMA model for the time-series.
Setting the `stepwise` option as `TRUE` forces a fast stepwise selection.
For real-life work this option should be set as `FALSE`, but for the purposes of this tutorial `TRUE` is fine. 
### ARIMA
```r
estimation.window.DT <- my.ts.DT[date >= '2016-09-01']
estimation.window.sales <- ts(estimation.window.DT[, sales], start = 0, frequency = 7)
estimation.window.webvisits <- ts(estimation.window.DT[, webvisits], start = 0, frequency = 7)
fit.1.sales <- auto.arima(estimation.window.sales, max.p = 7, max.q = 7, max.d = 2, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 2, trace = FALSE, stepwise = TRUE, max.order = 20, allowmean = TRUE, ic = 'bic')
fit.1.sales
# Series: estimation.window.sales 
# ARIMA(1,1,1)(0,0,1)[7] with drift 
# 
# Coefficients:
#          ar1      ma1    sma1   drift
#       0.4190  -0.9626  0.2254  1.2397
# s.e.  0.0682   0.0275  0.0601  0.7180
# 
# sigma^2 estimated as 16406:  log likelihood=-1516.63
# AIC=3043.26   AICc=3043.51   BIC=3060.7
Box.test(fit.1.sales$resid, lag=30, fitdf=3, type='Ljung')$p.value  # fitdf p+q + P + Q
# [1] 0.07378881                                                                                                           
tsdisplay(fit.1.sales$resid, lag.max = 30)
```
The best fitted simple ARIMA with seasonality is the ARIMA(1,1,1)(0,0,1)[7] with BIC 3060.7. 
For real-life analysis, we should also proceed to cross-validation/out-of-sample tests and extensive calibration without a stepwise selection, though. 
Ljung-Box shows that the residuals can be considered white noise. 
The portmanteau test does not reveal serial correlation for the first 10 lags of the model and we can proceed to forecasting.

### ARIMAX
Using external regressors on an AR(I)MAX model (creating a dynamic regression model) could potentially increase the Bayesian Information Criterion (BIC).
In our case, the only external regressors we have is the webvisits (which are highly correlated to the sales time-series), the existence of a marketing campaign and the outlier effects.
To be truthful in our analysis, the information present in our outlier effects is extracted from the sales time-series so including them as regressors could lead to endogeneity problems.
Moreover, the high Pearson correlation coefficient of 0.875 (`cor(DT[, webvisits], DT[, sales])`) between the webvisits and sales is an alert for multi-collinearity problems.
Nevertheless, purely for demonstration purposes I do consider both of them.

First, we create the matrix of the external regressors.
Let's try to incorporate the marketing campains.

my.ts.DT[, campaign := 0]
my.ts.DT[date >= '2016-09-15' & date <= '2016-09-23', campaign := 1]
my.ts.DT[date >= '2016-11-25' & date <= '2016-11-29', campaign := 1]
my.ts.DT[date >= '2017-01-25' & date <= '2017-02-03', campaign := 1]

my.campaigns <-  as.matrix(my.ts.DT[date >= '2016-09-01', campaign], start = c(2014, 245), frequency = 7)
```
The following, also suggests we have 4 outliers in our estimation window:
1. An additive outliwer on the 22nd day.
2. Two temporary changes on the 89th and 152nd day.
3. A level shift on the 117th day.

The additive outlier and the two temporary changes correspond to the marketing campaigns.
So, the marketing campaigns do have an impact on sales.
Nevertheless, their impact is not permanent, since around the 117th day there is no marketing campaign.
We should certainly try to identify why this permanent level shift occured!
For the particular time-series and solve this mystery of the level shift, the level shift occured due to external factors out of the influence of the specific firm. 

```r
my.estimation.window.outliers <- tso(estimation.window.ts)
my.estimation.window.outliers
# Series: estimation.window.ts 
# Regression with ARIMA(1,0,1) errors 
# 
# Coefficients:
#          ar1     ma1  intercept      AO22      TC89     LS117     TC152
#       0.2483  0.0557   315.5587  391.3931  573.1568  218.3963  486.9718
# s.e.  0.1819  0.1844    14.9565  108.2156  101.3114   20.6134  100.2111
# 
# sigma^2 estimated as 13103:  log likelihood=-1493.19
# AIC=3002.38   AICc=3002.99   BIC=3030.32
# 
# Outliers:
#   type ind     time coefhat  tstat
# 1   AO  22 2016:266   391.4  3.617
# 2   TC  89 2016:333   573.2  5.657
# 3   LS 117 2016:361   218.4 10.595
# 4   TC 152  2017:31   487.0  4.859

png(file= 'figures/15-estimation-window-outliers.png')
ts.plot(my.estimation.window.outliers$effects, main = "5. Estimation Window outliers")
dev.off()
```
![Estimation Window Outliers](figures/15-estimation-window-outliers.png)

```r
fit.2.xreg <- auto.arima(estimation.window.sales, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg = estimation.window.webvisits)
fit.2.xreg 
Series: estimation.window.sales 
Regression with ARIMA(1,0,1)(0,0,1)[7] errors 

Coefficients:
         ar1      ma1    sma1  intercept    xreg
      0.5859  -0.2709  0.1242    78.0997  0.6206
s.e.  0.1327   0.1554  0.0646    47.8684  0.0768

sigma^2 estimated as 14575:  log likelihood=-1507.24
AIC=3026.48   AICc=3026.83   BIC=3047.43


Box.test(fit.2.xreg$resid, lag=30, fitdf=3, type='Ljung')$p.value  # fitdf p+q + P + Q
[1] 0.8294632

tsdisplay(fit.2.xreg$resid, lag.max = 30)


fit.3 <- auto.arima((my.ts), max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 1, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = FALSE, xreg = my.campaigns)

fit.4 <- auto.arima((my.ts), max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 1, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = FALSE, xreg = cbind(my.campaigns, my.webvisits))

my.outliers <- tso(my.ts)

my.ts <- ts(my.ts.DT[date >= '2016-09-01', sales], start = c(2016, 245), frequency = 365)
fit.6 <- auto.arima((my.ts), max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 1, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = FALSE, xreg = cbind(my.Expos, my.webvisits, my.outliers$effects), parallel = TRUE, num.cores = 3, ic='bic')
```
 fit.1.sales.extended <- auto.arima(estimation.window.sales, max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 0, trace = TRUE, stepwise = FALSE, max.order = 10, allowmean = TRUE, ic = 'bic', parallel = TRUE, num.cores = 4)




> fit.1.sales.extended
Series: estimation.window.sales 
ARIMA(2,1,1)(1,0,1)[7] 

Coefficients:
         ar1     ar2      ma1    sar1     sma1
      0.3906  0.1651  -0.9757  0.9279  -0.7720
s.e.  0.0670  0.0677   0.0182  0.0501   0.0936

sigma^2 estimated as 15181:  log likelihood=-1507.51
AIC=3027.03   AICc=3027.38   BIC=3047.96


I only mention some highlights of further experimentation, to conserve space. 
For the transformed series, the best fitted ARMA without seasonality is the ARMA(2,0,1). 
The best fitted ARMA with seasonality is the ARMA(4,0,3)(0,0,2)[7]. 
For this analysis, I skip cross-validation/out-of-sample tests, to conserve space.
I would choose the ARMA(2,0,1) for conservative reasons:
it is simpler, the AIC/BIC of the two are very close and ARIMA(4,0,3)(0,0,2)[7] may be prone to overfitting. 
However, for the purposes of this particular analysis and its constraints I do not make any transformation calibration. 
For the purposes of accurate work, such analyses are advised to be automated through a [make-type-of-file](https://robjhyndman.com/hyndsight/makefiles/) or a master script.

Note that ARIMA-based intervals are generally too narrow and that historical patterns will repeat for the forecasting period.



## 3. Forecasting
## 4. Decision making


## 5. Expansions
Sales and Web Visits, although non-stationary, seem to be cointegrated by the Johansen test (see `ca.jo()` of the `urca` package in R). 
Hence, with a clear cointegration relation we could also forecast using vector autoregression (VAR/VECM/etc.). 
For futher analysis, consult [Harris and Sollis(2002)](http://eu.wiley.com/WileyCDA/WileyTitle/productCd-0470844434.html).
```r
my.ts <- ts(my.ts.DT[date >= '2016-09-01', sales], start = c(2014, 245), frequency = 7)
my.webvisits <-  as.matrix(my.ts.DT[date >= '2016-09-01', webvisits], start = c(2014, 245), frequency = 7)

my.matrix <- as.matrix(cbind(my.ts, my.webvisits))
summary(ca.jo(my.matrix))
# The value of the test statistic is: 14.9613 61.3969 
# 
# ###################### 
# # Johansen-Procedure # 
# ###################### 
# 
# Test type: maximal eigenvalue statistic (lambda max) , with linear trend 
# 
# Eigenvalues (lambda):
# [1] 0.22489667 0.06019242
# 
# Values of teststatistic and critical values of test:
# 
#           test 10pct  5pct  1pct
# r <= 1 | 14.96  6.50  8.18 11.65 # test value greater than the critical
# r = 0  | 61.40 12.91 14.90 19.19 # test value greater than the critical
# 
# Eigenvectors, normalised to first column:
# (These are the cointegration relations)
# 
#                  my.ts.l2 my.webvisits.l2
# my.ts.l2        1.0000000        1.00000
# my.webvisits.l2 -0.6967978       32.90515
# 
# Weights W:
# (This is the loading matrix)
# 
#                  my.ts.l2 my.webvisits.l2
# my.ts.d       -0.62798816   -0.001265426
# my.webvisits.d -0.02119726   -0.003196105
```


The above analysis is obviously preliminary, especially for time series that deal with thousands/millions of sales and there are a lot at stake.
The general analytical path could be consisted of these steps:
 * visual inspection, standard statistical tests
 * identify the best time series model, try to encompass various relevant variables that should influence the sales
 * compare it with other types of models as bayesian structural time series models (see the [CausalImpact](https://google.github.io/CausalImpact/CausalImpact.html) package in R), machine learning/xgb/etc. models 
 * identify causal relationships among the time-series, etc...
 * perform out-of-sample analysis/cross-validation
 * automate the process and create potential automatic report generation through the
power of R or/and python. The goal is to have automatic predictions for every
segment of the market, as real-time as possible, for various metrics and time-series.

