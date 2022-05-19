# R packages
library(tidyverse) # ... to be completed
library(stringr)   # string library
library(pdftools)  # pdf util
library(tools)     # file utils

#
# BoDuc Report Creator Tool to extract text from pdf
#

# removes all objects except for functions
rm(list = setdiff( ls(), lsf.str()))

# project folder
projectPath <- getwd() # retrieve working folder
pdfPath <-  str_c(projectPath,"/PDF Files/") #set pdf path 
csvPath <-  str_c(projectPath,"/CSV Files/") #set csv path 

# retrieve file to convert 
file2Test <- str_c( pdfPath,"/Martinco co 200957-200969.pdf")
# check file extension
fileExt <-  file_ext(file2Test) # returns "pdf" on success
if( file_ext(file2Test) == "pdf")
{
  vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
  
  list2Vec <- unlist(vecSplittedLines)
  csvSave <- str_c(csvPath, "Martinco co 200957-200969.csv")
  write.csv(list2Vec, file = csvSave)
}