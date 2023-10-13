# Tradernet Widget

***
__About__

This application allows you to track the current status of open positions with the Tradernet broker through the official API in a convenient user interface without the need to constantly visit the broker's website. The Tradernet API works in conjunction with the FinhubIO API, which provides up-to-date data on securities.

Data updates occur every 15 seconds. The provided data has a 15-minute delay relative to the current price that can be seen in the broker's personal account. This delay is due to technical limitations when using the FinhubIO API.

The data is located in the menu window in the top right corner on the menu bar

<img src="https://i.imgur.com/tV1JSSv.png" width="300">

***
__First start__
1. Download all the files.
2. Open "trayiconwidget.cpp" and set up your FinhubAPI key in "getDataAboutPositionsFromFinhub" function.
3. Start program and open the settings window. You need to set up your public and private TradernetAPI keys.
4. Restart the program.

***
__Used technologies__

C++

Python

Qt

curl

JsonCPP

Tradernet API(https://tradernet.global/tradernet-api/auth-api) 

FinhubIO API(https://finnhub.io/)

