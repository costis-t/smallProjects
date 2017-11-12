# Overview - under editing
In this sample tutorial, I analyze a sales time-series for an online firm and I identify whether there is a permanent impact of given marketing campaigns on the sales time-series.
I also forecast the sales time-series for one month with and without an imminent marketing campaing using ARIMAX modelling. 
Along with the sales time-series, we also have access to the web visits time-series and the dates of the marketing campaigns.

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
This step can be skipped if we have already installed them.


```r
sapply(c('data.table', 'tsoutliers', 'colorout', 'ggplot2', 'imputeTS', 'pracma', 'strucchange', 'tseries', 'forecast'
     ), install.packages) # install packages
```

Next, load them with:
```r
sapply(c('data.table', 'tsoutliers', 'colorout', 'ggplot2', 'imputeTS', 'pracma', 'strucchange', 'tseries', 'forecast'
     ), require, character.only = TRUE) # load packages
```

## 1.2. Load and prepare the data series
I have saved the anonymized/fictional data series as `data/anonymized-forecasting-data.csv`.
We can confirm the file exists within R (under a Linux OS):

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
png(file = 'figures/01-simple-sales-graph.pdf')
ggplot(data = DT, aes(date, sales)) +
       geom_line() + ggtitle('1. Sales simple graph') +
       ylab('Sales') + xlab('Date')
dev.off()
```

![Simple sales graph](figures/01-simple-sales-graph.pdf)

## 1.3. Data quality
Let's see if we have any missing values; days with no data.
Create a sequence of dates (`date.grid`) from the first till the last day of `DT` and compare the lengths of the original dataset to the sequence of dates we just created.
```r
date.grid <- seq(as.Date(min(DT[, date])), as.Date(max(DT[, date])), by = 'day')
length(date.grid)
# [1] 1217
nrow(DT)
# [1] 1200
```

It seems we do have 17 missing values.
Which are they? 
R comes with set operation functions (check `?setdiff`) and we can use a set difference to find them.
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
Figure 2 shows the imputed values for the first 150 observations of the sales time-series

```r
date.grid.DT <- data.table(date = c(date.grid))
setkeyv(DT, c('date'))
setkeyv(date.grid.DT, c('date'))

DT <- DT[date.grid.DT]

DT[, c('webvisits.with.NAs', 'sales.with.NAs') := .(webvisits, sales)]
DT[, c('webvisits', 'sales') := .(na.kalman(webvisits), na.kalman(sales))]

png(file = 'figures/02-first-150-imputed-values-for-sales-graph.png')
plotNA.imputations(DT[1:150, sales.with.NAs], DT[1:150, sales], ylab = 'sales', main = '2. Visualization of the Imputed Values in Sales')
dev.off()
```

![Imputed values for sales graph](figures/02-first-150-imputed-values-for-sales-graph.png)

Now, we are ready to start our analysis!

# 2. Analysis 
## 2.1. Helicopter view of the data

As in theory, there seems to be four components in the time series (trend, cycle, seasonal, random error). 
Data are very noisy. 
Ideally, we'd like to have a large increase during and after the marketing campaings, but this does not happen.
Since there is an intervention variable (marketing campains) an ARIMAX model seems appropriate. 
Other exogenous variables like the web visits time series can enter the ARIMAX, too.

Figure 1, above, shows that the `sales` seem to follow: 
 * a parallel movement in 2014--early:2015, 
 * a slightly upward trend during early:2015–early:2016, and 
 * a more vigorous upward trend since mid:2016 with some indication of a potential parallel movement after early:2017. 
 
The aforementioned movements/trends suggest that a regime-switching model could also be appropriate for such data. 
We don’t see linear, but a polynomial, trend for the whole time-series, which is common for sales data. 
We also see structural breaks (at least in level and variance) which influences the unit root tests and need further exploration. 
In the next sections:

* we create a nice figure for the time-series
* we explore some statistical characteristics (autocorrelation structure, breaks, stationarity, outliers).
I follow [Bai and Perron (2003)](http://onlinelibrary.wiley.com/doi/10.1002/jae.659/abstract) and the BIC criterion suggests 3 breaks (four would not make a large difference, though) for the whole sales time-series.
* we choose the appropriate ARIMAX model
* we proceed to the forecasts

## 2.2. Graph with outliers and smoothing curves
Let's create a beautiful graph for the data!

First, we detect the outliers, *in terms of data quality*, using an uncalibrated Hampel filter.
For more information on the Hampel filter, please consult [Pearson, Neuvo, Astola and Gabbouj (2016)](https://asp-eurasipjournals.springeropen.com/articles/10.1186/s13634-016-0383-6).
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
Ideally, we forecast with and without the outliers, calibrate their corrections and identify whether they are indeed outliers or indicate a response to an event. 
The last, seems plausible for the outliers during the marketing campaigns. 
Please refer to [Tsay (1988)](http://onlinelibrary.wiley.com/doi/10.1002/for.3980070102/abstract) and [Watson (2001)](http://eprints.whiterose.ac.uk/2209/1/ITS261_WP362_uploadable.pdf) for various outlier types (additive/level shifts/etc.) and techniques for their correction as a starting point. 
For example, at a more advanced stage, we could use a non-linear moving window and parameter calibration.

The following Figure 3, also includes three smoothing functions with different coarseness, following a LOESS fitted curve with correspondingly different parameters.

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
png(file = 'figures/04-ACF-and-PACF-of-sales.png')
ggtsdisplay(DT[, sales], main = "4. DT[, sales]")
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
Bai and Perron (2003) method indicates  three breaks as optimal. 
Even in non-optimal cases with 2, 4 or 5 breaks the last break is on 2016-10-30. Hence, I consider 2016-09-01 as the start of my estimation window. 
For further exploration, Pesaran and Timmermann propose a calculation for the ideal estimation windows which I skip as it does not seem so critical for the purposes of this tutorial analysis.
Another possibility would be to consider outlier events, like level shifts.
There is no need for extensive intervention analysis throughout the series for this analysis, but I briefly indicate how the `tsoutliers` helps us improve the ARIMAX model.
For more information on `tsoutliers`, please consult its documentation in [López-de-Lacalle (2016)](https://jalobe.com/doc/tsoutliers.pdf).

For this simple tutorial analysis, to locate the breaks we work with the `strucchange` package in R.
Also, I do not consider decomposing seasonality with sophisticated methods, e.g. using the  `Rlibeemd` R package.
Moreover, I consider the time-series with yearly and weekly seasonal frequency and I create the  `sales.ts.yearly.frequency`, `webvisits.ts.yearly.frequency` and `ts.weekly.frequency`, `sales.ts.weekly.frequency`, `webvisits.ts.weekly.frequency` respectively, because it is important for the ARIMA modeling.

```r
ts.DT <- DT[, .(date, sales, webvisits)] 
day.of.year <- as.numeric(format(ts.DT[, date][1], '%j')) 
sales.yearly.frequency <- ts(ts.DT[, sales], start = c(2014, day.of.year), frequency = 365.25)
sales.weekly.frequency <- ts(ts.DT[, sales], start = c(1, 1), frequency = 7) # with weekly frequency because ARIMA can not handle multiple seasonalities
webvisits.yearly.frequency <- ts(ts.DT[, webvisits], start = c(2014, day.of.year), frequency = 365.25)
webvisits.weekly.frequency <- ts(ts.DT[, webvisits], start = c(1, 1), frequency = 7)
```

Now we can detect the break points.
```r
bp.ri <- breakpoints(sales.yearly.frequency ~ 1) # using different seasonal frequency does not make a difference
summary(bp.ri)
#          Optimal (m+1)-segment partition: 
# 
# Call:
# breakpoints.formula(formula = sales.yearly.frequency ~ 1)
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
plot(sales.yearly.frequency, type = 'l')
lines(fitted(bp.ri, breaks = 3), col = 4)
lines(confint(bp.ri, breaks = 3))
```

Hence, for the day of the break, we choose the 1034th day of our time-series which corresponds to 30th of October of 2016:

```r
ts.DT[1034]
#          date sales webvisits
# 1: 2016-10-30   273       455
```

For further exploration, other functions of the `strucchange` package in R are a good starting point.
Example graphs:

```r
plot(Fstats(sales.yearly.frequency ~ 1))
plot(efp(sales.yearly.frequency ~ 1, type = 'Rec-CUSUM'))
plot(efp(sales.yearly.frequency ~ 1, type = 'OLS-CUSUM'))
```
Following  [Bai and Perron (2003)](http://onlinelibrary.wiley.com/doi/10.1002/jae.659/abstract), the estimation window contains 3 breaks with BIC 14404 as shown above from `summary(bp.ri)`.
To keep this tutorial analysis simple but not too simple, I consider only one break in mean on 2016-12-25, which I let (or not) enter the ARIMAX model as an external regressor (using the non-adjusted sales time-series as I explain below) and I do not proceed to a BoxCox transformation.
Ideally, we should also proceed to out-of-sample tests (also known as training/test set differences) and cross-validation but this is out of the scope of this tutorial.
I arbitrarily set the start of the estimation window about 4 months before the break in mean, on 01 September 2016.
Figure 5 presents the estimation window used to identify the ARIMA specification and proceed to the forecast in order to have a better view of the data series.
```r
day.of.year <- as.numeric(format(as.Date('2016-09-01'), '%j')) 
day.of.year
# [1] 245
sales.est.window.weekly.frequency <- ts(ts.DT[date >= '2016-09-01', sales], start = c(1, 1), frequency = 7)
sales.est.window.yearly.frequency <- ts(ts.DT[date >= '2016-09-01', sales], start = c(2016, 245), frequency = 365.25)
autoplot(sales.est.window.yearly.frequency)
campaign.dates.truncated <- data.frame( # campaign.dates for the estimation window
				   campaign.start =  c(
						   as.Date('2016-09-15'),
						   as.Date('2016-11-25'),
						   as.Date('2017-01-25')
						   ), campaign.end = c(
						   as.Date('2016-09-23'),
						   as.Date('2016-11-29'),
						   as.Date('2017-02-03')))
figure <- ggplot(DT[date >= '2016-09-01']) +
	geom_line(aes(x = date, y = sales), colour = 'red', size = 0.4, alpha = 0.7)  +
	geom_rect(data = campaign.dates.truncated, aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +
	scale_x_date(date_minor_breaks = '1 week',  
	             date_labels = '%m - %Y', # http://strftime.org
		         date_breaks = '1 month') +
    ggtitle('5. Sales, estimation window') +
    ylab('Sales') + xlab('Date') +
    list()
ggsave(filename = 'figures/10-graph-estimation-window.png', plot = figure, height = 100, units = 'mm')
```

![Sales Estimation Window](figures/10-graph-estimation-window.png)


### 2.3.3. Stationarity
We must also perform stationarity tests (see `adf.test()`, `pp.test()`, `kpss.test()` of the `tseries` package in R).
The series are non-stationary for the whole period and the selected estimation window.
```r
adf.test(sales.est.window.weekly.frequency, alternative = 'stationary')
#         Augmented Dickey-Fuller Test
# data:  sales.est.window.weekly.frequency
# Dickey-Fuller = -3.1108, Lag order = 6, p-value = 0.1085 # null of non-stationarity is not rejected at 0.05
# alternative hypothesis: stationary
pp.test(sales.est.window.weekly.frequency)
#         Phillips-Perron Unit Root Test
# data:  sales.est.window.weekly.frequency
# Dickey-Fuller Z(alpha) = -131.51, Truncation lag parameter = 4, p-value = 0.01 # null of stationarity is rejected at 0.05
# alternative hypothesis: stationary
kpss.test(sales.est.window.weekly.frequency)
#         KPSS Test for Level Stationarity
# 
# data:  sales.est.window.weekly.frequency
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

fit.1.sales <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 2, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 2, trace = FALSE, stepwise = TRUE, max.order = 20, allowmean = TRUE, ic = 'bic')
fit.1.sales
# Series: sales.est.window.weekly.frequency 
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

```r
ts.DT[, campaign := 0]
ts.DT[date >= '2016-09-15' & date <= '2016-09-23', campaign := 1]
ts.DT[date >= '2016-11-25' & date <= '2016-11-29', campaign := 1]
ts.DT[date >= '2017-01-25' & date <= '2017-02-03', campaign := 1]
my.campaigns <-  as.matrix(ts.DT[date >= '2016-09-01', campaign], start = c(2014, 245), frequency = 365)
my.campaigns <-  as.matrix(ts.DT[date >= '2016-09-01', campaign])
```
The following, also suggests we have 4 outliers in our estimation window:
1. An additive outliwer on the 22nd day.
2. Two temporary changes on the 89th and 152nd day.
3. A level shift on the 117th day.

The additive outlier and the two temporary changes correspond to the three marketing campaigns.
So, the marketing campaigns do have an impact on sales, which is also evident from figure 3 above.
Nevertheless, their impact is not permanent, since around the 117th day there is no marketing campaign.
We should certainly try to identify why this permanent level shift occured!
For the particular time-series and solve this mystery of the level shift, the level shift occured due to external factors out of the influence of the specific firm and it wasn't due to a lagged effect of the previous marketing campaigns.


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
estimation.window.outlier.effects <- ts(my.estimation.window.outliers$effects, start = c(2016, as.integer(strftime('2016-09-01', format = '%j'))), frequency = 365)
png(file = 'figures/15-estimation-window-outliers.png')
ts.plot(my.estimation.window.outliers$effects, main = '5. Estimation window outliers')
dev.off()
```
![Estimation Window Outliers](figures/15-estimation-window-outliers.png)

To find the ideal model for this tutorial, I consider the models of the following table

| Object | Model | Series | External regresor(s) | BIC |
| ----- | ----------------- | --- | --- |  --- |
| `fit.1.sales` | ARIMA((1,1,1)(0,0,1)[7]) with drift | Web Sales | none | 3060.7 |
| `fit.2.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Web Visits | 3047.43 |
| `fit.3.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Campaigns | 3052.71 |
| `fit.4.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Web Visits and Campaigns | 3042.51 |
| `fit.5.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Outlier Effects | 3003.57 |
| `fit.6.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Web Visits  and Outlier Effects | 3002.02 |
| `fit.7.xreg` | ARIMA(1,0,1)(0,0,1)[7] | Web Sales | Web Visits, Campaigns and Outlier Effects | 2993.62 |

For the purposes of accurate work, such analyses are advised to be automated through a [make-type-of-file](https://robjhyndman.com/hyndsight/makefiles/) or a master script and as mentioned above, `stepwise` should be set to `FALSE`.
Also, note that ARIMA-based intervals are generally too narrow and that historical patterns will repeat for the forecasting period.


```r
fit.2.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg = webvisits.est.window.weekly.frequency)
fit.2.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(1,0,1)(0,0,1)[7] errors 
# 
# Coefficients:
#          ar1      ma1    sma1  intercept    xreg
#       0.5859  -0.2709  0.1242    78.0997  0.6206
# s.e.  0.1327   0.1554  0.0646    47.8684  0.0768
# 
# sigma^2 estimated as 14575:  log likelihood=-1507.24
# AIC=3026.48   AICc=3026.83   BIC=3047.43
fit.3.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg = my.campaigns)
fit.3.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(2,1,2)(0,0,1)[7] errors 
# 
# Coefficients:
#           ar1     ar2      ma1      ma2    sma1   drift          
#       -0.4141  0.3046  -0.2354  -0.7082  0.2292  1.3995  188.0186
# s.e.   0.3365  0.1054   0.3436   0.3294  0.0635  0.5535   36.8917
# 
# sigma^2 estimated as 15004:  log likelihood=-1504.4
# AIC=3024.8   AICc=3025.41   BIC=3052.71
fit.4.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg = cbind(webvisits.est.window.weekly.frequency, my.campaigns))
fit.4.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(2,0,2)(1,0,1)[7] errors 
# 
# Coefficients:
#          ar1      ar2      ma1     ma2    sar1     sma1  intercept
#       1.3192  -0.6133  -1.1015  0.4989  0.8554  -0.7496    79.5246
# s.e.  0.2656   0.2137   0.2757  0.2161  0.1105   0.1326    54.7130
#       webvisits.est.window.weekly.frequency  my.campaigns
#                            0.5844      173.0715
# s.e.                       0.0913       31.7842
# 
# sigma^2 estimated as 13246:  log likelihood=-1493.79
# AIC=3007.58   AICc=3008.53   BIC=3042.51
fit.5.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg =  my.estimation.window.outliers$effects)
fit.5.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(0,0,1)(1,0,1)[7] errors 
# 
# Coefficients:
#          ma1    sar1     sma1  intercept    xreg
#       0.2589  0.9117  -0.7942   315.4138  0.9671
# s.e.  0.0572  0.0918   0.1428    20.4329  0.0820
# 
# sigma^2 estimated as 12108:  log likelihood=-1485.3
# AIC=2982.61   AICc=2982.96   BIC=3003.57
fit.6.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg =  cbind(estimation.window.outlier.effects, webvisits.est.window.weekly.frequency))
fit.6.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(0,0,1)(0,0,1)[7] errors 
# 
# Coefficients:
#          ma1    sma1  intercept  estimation.window.outlier.effects
#       0.2440  0.1619   201.4675                             0.7719
# s.e.  0.0572  0.0623    38.4975                             0.0980
#       webvisits.est.window.weekly.frequency
#                            0.2430
# s.e.                       0.0755
# 
# sigma^2 estimated as 12092:  log likelihood=-1484.53
# AIC=2981.06   AICc=2981.42   BIC=3002.02
fit.7.xreg <- auto.arima(sales.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 3, seasonal = TRUE, max.P = 7, max.Q = 7, max.D = 3, trace = TRUE, stepwise = TRUE, max.order = 20, allowmean = TRUE, xreg =  cbind(estimation.window.outlier.effects, webvisits.est.window.weekly.frequency, my.campaigns))
fit.7.xreg 
# Series: sales.est.window.weekly.frequency 
# Regression with ARIMA(0,0,1)(1,0,1)[7] errors 
# 
# Coefficients:
#          ma1    sar1     sma1  intercept  estimation.window.outlier.effects
#       0.2084  0.8657  -0.7524   189.8716                             0.6979
# s.e.  0.0588  0.1090   0.1426    41.3306                             0.0995
#       webvisits.est.window.weekly.frequency  my.campaigns
#                            0.2567      105.1177
# s.e.                       0.0777       27.1298
# 
# sigma^2 estimated as 11232:  log likelihood=-1474.84
# AIC=2965.68   AICc=2966.29   BIC=2993.62
```

Being a little bit blind of the overidentification and endogeneity problems (the information of `my.campaigns` and `estimation.window.outlier.effects` is similar, except the level shift), and only for demonstration purposes
we choose the `fit.6.xreg` model for the forecast.

```r
Box.test(fit.6.xreg$resid, lag=30, fitdf=3, type='Ljung')$p.value  # fitdf p+q + P + Q
# [1] 0.6936533
```



## 3. Forecasting
### 3.1 Without a new marketing campaign
To forecast the sales with the selected ARIMAX model, we must first forecast the selected external regressors:
* the web visits,
* the existence of a marketing campaign (without a new marketing campaign, this corresponds to 0)
* the outliers (without a new marketing campaign, this corresponds to the the level shift, wich is the last value of the `estimation.window.outlier.effects`)

```r
fit.webvisits <- auto.arima(webvisits.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE)
forecasted.webvisits <- forecast(fit.webvisits, h = 30)$mean
forecasted.marketing.campaigns <- rep(0, 30)
forecasted.outlier.effects < rep(estimation.window.outlier.effects[length(estimation.window.outlier.effects)], 30)
future.external.regressors <- cbind(forecasted.outlier.effects, forecasted.webvisits, forecasted.marketing.campaigns) # the order of the regressors must be the same as in fit.7.xreg!
fit.forecast <- forecast(fit.7.xreg , xreg = cbind(forecasted.webvisits, forecasted.marketing.campaigns, forecasted.outlier.effects)) 
plot(forecast(fit.7.xreg, xreg = future.external.regressors, h = 30))
forecast.DT <- data.table(date = seq(as.Date(min(estimation.window.DT[, date])), as.Date(max(DT[, date])) + 30, by = 'day'),
sales = c(estimation.window.DT[, sales], rep(NA, 30)),
webvisits = c(estimation.window.DT[, sales], rep(NA, 30)),
forecasted.sales = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$mean),
forecasted.upper = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$upper[, '95%']),
forecasted.lower = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$lower[, '95%'])
)
ggplot(forecast.DT) +
	geom_line(aes(x = date, y = sales), colour = 'black')  +
	geom_line(aes(x = date, y = forecasted.sales), colour = 'red') +
	geom_ribbon(aes(x = date, ymin = forecasted.lower, ymax = forecasted.upper), fill = 'darkred', alpha = .2) +
	scale_x_date(  date_minor_breaks = '1 month', date_labels = '%b', date_breaks = '1 month') +
    geom_rect(data = campaign.dates[-1, ], aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +
    labs(title = '7. The forecasted Sales for fit.7.xreg',
               subtitle = 'Marketing campaign periods are grayed. \nForecasted Sales are shown in red along with a 95% prediction interval.',
               y = 'Sales',
               x = 'Date') + 
    list() 
```

### 3.2 With a new marketing campaign
For this case, we will consider a new marketing campaign during the second week of May 2017, which starts on Monday 8th of May.
For simplicity we will consider this campaign identical to the last campaign in late January, i.e. we will use the same impulse as the new outlier.

```r
fit.webvisits <- auto.arima(webvisits.est.window.weekly.frequency, max.p = 7, max.q = 7, max.d = 1, seasonal = TRUE)
forecasted.webvisits <- forecast(fit.webvisits, h = 30)$mean
length.of.the.January.campaign <- length(seq(as.Date(campaign.dates[4, ]$campaign.start), as.Date(campaign.dates[4, ]$campaign.end), by = 'day'))
forecasted.marketing.campaigns <- c(rep(0, 6), rep(1, length.of.the.January.campaign), rep(0, 30 - 6 - length.of.the.January.campaign))
forecasted.outlier.effects <- rep(estimation.window.outlier.effects[length(estimation.window.outlier.effects)], 30)
future.external.regressors <- cbind(forecasted.outlier.effects, forecasted.webvisits, forecasted.marketing.campaigns) # the order of the regressors must be the same as in fit.7.xreg!
fit.forecast <- forecast(fit.7.xreg , xreg = cbind(forecasted.webvisits, forecasted.marketing.campaigns, forecasted.outlier.effects)) 
plot(forecast(fit.7.xreg, xreg = future.external.regressors, h = 30))
forecast.DT <- data.table(date = seq(as.Date(min(estimation.window.DT[, date])), as.Date(max(DT[, date])) + 30, by = 'day'),
sales = c(estimation.window.DT[, sales], rep(NA, 30)),
webvisits = c(estimation.window.DT[, sales], rep(NA, 30)),
forecasted.sales = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$mean),
forecasted.upper = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$upper[, '95%']),
forecasted.lower = c(rep(NA, nrow(estimation.window.DT)), forecast(fit.7.xreg, xreg = future.external.regressors, h = 30)$lower[, '95%'])
)
ggplot(forecast.DT) +
	geom_line(aes(x = date, y = sales), colour = 'black')  +
	geom_line(aes(x = date, y = forecasted.sales), colour = 'red') +
	geom_ribbon(aes(x = date, ymin = forecasted.lower, ymax = forecasted.upper), fill = 'darkred', alpha = .2) +
	scale_x_date(  date_minor_breaks = '1 month', date_labels = '%b', date_breaks = '1 month') +
    geom_rect(data = campaign.dates[-1, ], aes(xmin = campaign.start, xmax = campaign.end, ymin = -Inf, ymax = Inf), alpha = 0.4) +
    labs(title = '7. The forecasted Sales for fit.7.xreg',
               subtitle = 'Marketing campaign periods are grayed. \nForecasted Sales are shown in red along with a 95% prediction interval.',
               y = 'Sales',
               x = 'Date') + 
          list() 
```
outlier.mydata <- tso(sales.est.window.weekly.frequency)
outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))

tstat <- outliers.tstatistics(coefs2poly(fit.1.sales), residuals(fit.1.sales))
which(abs(tstat[,"LS","tstat"]) > 3.5)
resid <- residuals(fit)
pars <- coefs2poly(fit)
tstats <- outliers.tstatistics(pars, resid)

outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"]
tc <- rep(0, length(outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"]))
tc[152] <- 1
cor(filter(tc, filter = 0, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.3, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.5, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.7, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.8, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.9, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
cor(filter(tc, filter = 0.7, method = 'recursive'), outliers.effects(outlier.mydata$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
#

all.equal(as.numeric(filter(tc, filter = 0.7, method = 'recursive')), outliers.effects(outlier.chicken$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])
identical(as.numeric(filter(tc, filter = 0.7, method = 'recursive')), outliers.effects(outlier.chicken$outliers, n = length(sales.est.window.weekly.frequency))[, "TC152"])

## 4. Decision making


## 5. Expansions
Sales and Web Visits, although non-stationary, seem to be cointegrated by the Johansen test (see `ca.jo()` of the `urca` package in R). 
Hence, with a clear cointegration relation we could also forecast using vector autoregression (VAR/VECM/etc.). 
For futher analysis, consult [Harris and Sollis(2002)](http://eu.wiley.com/WileyCDA/WileyTitle/productCd-0470844434.html).
```r
sales.weekly.frequency <- ts(ts.DT[date >= '2016-09-01', sales], start = c(2014, 245), frequency = 7)
my.webvisits <-  as.matrix(ts.DT[date >= '2016-09-01', webvisits], start = c(2014, 245), frequency = 7)

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

