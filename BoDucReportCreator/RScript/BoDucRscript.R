
library(tidyverse) # ... to be completed
library(stringr)   # string library
library(pdftools)  # pdf util
library(tools)     # file utils

#
# BoDuc Report Creator Tool to extract text from pdf
#

# removes all objects except for functions
rm(list = setdiff( ls(), lsf.str()))

# project folder ()
projectPath <- getwd() # retrieve working folder
print(projectPath) # debugging purpose
pdfPath <-  str_c(projectPath,"/PDF Files/") #set data path 
csvPath <-  str_c(projectPath,"/CSV Files/") #set data path 
print(pdfPath) # debugging purpose

# retrieve file to convert 
file2Test <- str_c( pdfPath,"/1-pm.pdf")
# check file extension
fileExt <- file_ext(file2Test) # returns "pdf"
if(fileExt == "pdf")
{
  print("Its a pdf file")
}

extractTxt <- pdf_text(file2Test)
vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
checkData <- pdf_data(file2Test)  #list

#print(extractTxt) debugging

list2Vec <- unlist(vecSplittedLines)

csvSave <- str_c(csvPath,"1-pm.csv")
write.csv(list2Vec, file = csvSave)

print("Finished writing to csv file")