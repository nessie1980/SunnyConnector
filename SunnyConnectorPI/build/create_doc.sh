#!/bin/bash

###################################################################################################
#
# Diese Script dient zur Erstellung der Dokumentation
#
###################################################################################################

cd ../Doxygen/

doxygen SunnyConnectorPI.doxyfile

cd latex

pdflatex refman.tex
pdflatex refman.tex

cp refman.pdf ../SunnyConnectorPI-Entwicklerhandbuch.pdf

evince ../SunnyConnectorPI-Entwicklerhandbuch.pdf
