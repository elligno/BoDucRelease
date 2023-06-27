# R packages
library(tidyverse) # ... to be completed
library(stringr)   # string library
library(pdftools)  # pdf util
library(tools)     # file utils

#
# BoDuc Report Creator Tool to extract text from pdf
#
# Passing argument to Rscript
#
#  https://community.rstudio.com/t/how-to-parse-command-line-arguments-with-flags/33797

# removes all objects except for functions
rm(list = setdiff( ls(), lsf.str()))

# just testing command line argument
args = commandArgs(trailingOnly = TRUE)
message(sprintf("Processing file %s", args[1L]))

# project folder (deprecate)
projectPath <- getwd() # retrieve working folder
pdfPath <-  str_c(projectPath,"/PDF Files/") #set pdf path 
csvPath <-  str_c(projectPath,"/CSV Files/") #set csv path 

# sharepoint path (as an example from OpenMind Capital with Sharepoint)
# environment variable OneDriveCommercial (SharePoint):
# C:\Users\jean\OneDrive - Elligno Inc
#lSharePtPath <- "C:\\Users\\jean\\OneDrive - Elligno Inc\\BoDuc - Documents\\RCSV Files\\"
#pdfPath <-  str_c(lSharePtPath,"/PDF Files/") #set pdf path 
#csvPath <-  str_c(lSharePtPath,"/CSV Files/") #set csv path
#file2Test <- str_c( pdfPath,"2019-03-01 952 jani 34216 4-3-19.pdf")

# retrieve file to convert 
#file2Test <- str_c( pdfPath,"/Martinco co 200957-200969.pdf")
# check white space in name
#file2Convert <- str_replace_all(args[1L], fixed(" "), "")
file2Test <- str_c( pdfPath,args[1L])

# check file extension
fileExt <-  file_ext(file2Test) # returns "pdf" on success
if( file_ext(file2Test) == "pdf")
{
  vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
  
  list2Vec <- unlist(vecSplittedLines)
#  csvSave <- str_c(csvPath, "Martinco co 200957-200969.csv")
  csvSave <- str_c(csvPath, args[1L])      # full path 
  remPdfExt <- str_sub(csvSave, end=-4)    # remove extension (.pdf)
  csvFileName <- str_c(remPdfExt, "csv")   # replace extension by .csv
  write.csv(list2Vec, file = csvFileName)
}