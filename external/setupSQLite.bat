@echo off

set SQLITE_VERSION=sqlite-amalgamation-3390400

mkdir sqlite
powershell -Command "Invoke-WebRequest https://www.sqlite.org/2022/%SQLITE_VERSION%.zip -OutFile sqlite.zip"
powershell Expand-Archive sqlite.zip -DestinationPath sqlite
del sqlite.zip

move sqlite\%SQLITE_VERSION%\* sqlite

rmdir sqlite\%SQLITE_VERSION%


