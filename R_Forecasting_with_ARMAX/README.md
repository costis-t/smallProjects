Overview
============
Forecasting with ARMAX

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

