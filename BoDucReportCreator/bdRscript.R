
library(tidyverse) # ... to be completed
library(stringr)   # string library
library(pdftools)  # pdf util
library(tools)     # file utils

#
# BoDuc Report Creator Tool to extract text from pdf
#

# removes all objects except for functions
rm(list = setdiff( ls(), lsf.str()))

# just testing command line argument
args = commandArgs(trailingOnly = TRUE)
message(sprintf("Processing file %s", args[1L]))

# project folder
projectPath <- getwd() # retrieve working folder
pdfPath <-  str_c(projectPath,"/PDF Files/") #set pdf path 
csvPath <-  str_c(projectPath,"/CSV Files/") #set csv path 

# retrieve file to convert 
#file2Test <- str_c( pdfPath,"/1-pm.pdf")
file2Test <- str_c( pdfPath,args[1L])
#remPdfExt <- str_sub(file2Test, end=-4)
#csvExt <- str_c(remPdfExt, "csv")
print("file to test is: ")
print(file2Test)
# check file extension
fileExt <-  file_ext(file2Test) # returns "pdf" on success
if( file_ext(file2Test) == "pdf")
{
#  extractTxt <- pdf_text(file2Test)
  vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
  #checkData <- pdf_data(file2Test)  list

  list2Vec <- unlist(vecSplittedLines)
  csvSave <- str_c(csvPath, args[1L])
  remPdfExt <- str_sub(csvSave, end=-4)
 # csvExt <- str_c(remPdfExt, "csv")
  csvFileName <- str_c(remPdfExt, "csv")
 # print("Saved file is: ", csvSave)
  write.csv(list2Vec, file = csvFileName)
}