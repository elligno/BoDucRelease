
library(tidyverse)
library(stringr)
library(pdftools)
library(data.table)
library(R.utils)
library(xfun)


# Design Note (algorithm)
# -----------------------
#
#  check this link for the assignment (insert element in a vector by overwriting)
#  which is exactly what we want (insert the shipped address to replace original)
#  which is a big piece of shit.
#  http://adv-r.had.co.nz/Subsetting.html#subassignment 
#
#  Problem with the shipped address will be adressed before
#  we export character vector.
#  From de pdf_data() we have a list of character with x,y, coordinate and Text value
# 
#  First we need to retrieve the range ['Shipped' -- 'Contact'] which contains all the 
#  information (both address: 'Sold To' and 'Shipped To'). Maybe by index [10-21]. Search
#  in the vector for the keywords "Shiped" and "Contact".
#  can we do that? vecAdrrs <- extract[10:21] subrange of the whole vector. Need to test!!  
#  Try this: extractText[grepl("Shipped" | "Contact",extractText) return a vector of TRUE | FALSE
#  It is actually a way to access one or more indexes within a vector,
#  There is only 1 "Shipped" and "Contact" respectively, i have the indexes 
#
#  Next step is to split these two. First address is a sequence of character with x,y
#  where y is the line number. We need to check for first y (first line in the address)
#  (each word in the address has its own (x,y))
# x=2,y=4 'RP2R'
# x=3,y=4 'sec'
# and so on ...
# second address start at the same 'y' as the first one. Since we know y of the first 
# look for this 'y'. Once found, take that chunk to 'Contact' and you have the whole 
# shipped addrress. Use std_c() to put words that belongs to same line in a single string.
# We end up with a vector of strings which represent the Shipped address.
#
#  Passing argument to Rscript
#
#  https://community.rstudio.com/t/how-to-parse-command-line-arguments-with-flags/33797
#
# IMPORTANT
# ---------
#
#  Need to count the number of lines of the first address and this is why?
#  We will insert (assign by overwerting) this shipped in the export vector.
#  The one obtained with pdf_text() in which address is a mess. 

# removes all objects except for functions
rm(list = setdiff( ls(), lsf.str()))

# just testing command line argument
args <- commandArgs(trailingOnly = TRUE)
testListArgs <- R.utils::commandArgs(asValues = TRUE)

checkLength <- length(testListArgs)
for( argsCheck in testListArgs) {
  message(sprintf("Args is %s", argsCheck))
}

i <- 4
message(sprintf("Last Args is %s", testListArgs[i+1]))


#args = commandArgs(trailingOnly=FALSE, asValues=TRUE)
#checkLength <- length(args)
message(sprintf("Args length is %s", checkLength))
message(sprintf("Hello %s", args[1L]))

# project folder ()
projectPath <- getwd() # retrieve working folder
dataPath <-  str_c(projectPath,"/data/") #set data path  
print(dataPath) # debugging purpose

csvPath <- str_c(projectPath,"/CSV Files/")

#PDF files to convert
pdfPath <- str_c(projectPath,"/PDF Files/")
file2Test <- str_c( pdfPath,args[1L])

# Concatenate string (some test with single file)
#file2Test <- str_c( dataPath,"/jani3163190617.pdf") # (it works!)
#file2Test <- str_c( dataPath,"/2017-09-11reports_ve2_memo_bon_coop.pdf") # 
#file2Test <- str_c( dataPath,"/valleroy 5620 10817.pdf") # single facture (it  works!)
#file2Test <- str_c( dataPath,"/Martinco co 200957-200969.pdf") # 2 factures (it works!)
#file2Test <- str_c( dataPath,"/explorateur 5563 10817.pdf") # 2 factures (it works!)
#file2Test <- str_c( dataPath,"/2019-02-27 1116 Algie co 206633  Algie 10 tm.pdf") (it works!)
#file2Test <- str_c( dataPath,"/2019-02-28 846 s blais 46034 4-3-19.pdf") #(it works!)
#file2Test <- str_c( dataPath,"/2019-02-28 1044 taratuta 46040 4-3-19.pdf") #(it works!)

# sharepoint path (as an example from OpenMind Capital with Sharepoint)
# I remember 
#lSharePtPath <- "\\Openmind Capital\\Logiciels - Documents\\Dev\\R_dev\\Package Examples\\Data"

# Put all together
#lDataPath <- str_c( ldata,lSharePtPath)   

# NOTE
# i do think this is the last version, just processing all files
# in a given folder and extract data from it. For this script the
# current location is the data folder

# list all files in a given location  
#listFilesIn <- list.files(dataPath)
#listPDFiles <- list.files(pdfPath)
#for( file2check in listFilesIn) {
#  file2Test <- str_c( dataPath, file2check)

  #print all files in the given dir
#  print("File to proceed: ")
#  print(file2check)
  
  # ...
  xtractTxt <- pdf_text(file2Test) 
  vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
  checkData <- pdf_data(file2Test)  #list
  list2Vec <- unlist(vecSplittedLines)
  
  # split file2Test and then re-use file name with .csv extension
  #fileName <- tools::file_path_sans_ext(file2Test)
  #csvFileName <- str_c(fileName,'.csv')
  #message(sprintf("Csv file name is is %s", csvFileName))
  #write.csv( list2Vec, file = csvFileName)
  
  for( argsCheck in testListArgs) {
    message(sprintf("Checking file is %s", argsCheck))
    #fileNametst <- tools::file_path_sans_ext(args[1L])
    if( file_ext(argsCheck) == "pdf")
    {
      fileNametst <- tools::file_path_sans_ext(argsCheck)
      csvFileNametst <- str_c(fileNametst, '.csv')
      #csvPath <- str_c(projectPath,"/CSV Files/")
      file2Res <- str_c(csvPath, csvFileNametst)
      message(sprintf("Csv file name is %s", file2Res))
      write.csv(list2Vec, file = file2Res)
    }
    else 
    {
      next
    }
  }
  
#}

# Process each ...
# Stored all of them within the ‘Data’ folder, use list.files() to get them
#file_vector <- list.files(path = "data")

# Slice our list, selecting only those records where our grepl() call 
# returns TRUE
# "file_list" is R function that return list of all project files 
# pdf_list <- file_vector[ grepl( ".pdf", file_list)]


#pc <- pdf_fonts(file2Test)
#testFont <- pdfFonts(file2Test)

#print(dataPath)

# this is a simple facture that should do for testing
# expect list of character vector
# get all of the textual information contained within each of the PDF files

# test single file
# extractTxt <- pdf_text(file2Test)
# vecSplittedLines <- pdf_text(file2Test) %>% strsplit(split = "\n")
# checkData <- pdf_data(file2Test)  #list
# 
# print(extractTxt)

#list2Vec <- unlist(vecSplittedLines)
##myDataTbl <- setDT(list2Vec)

#write.csv( list2Vec, file = "TestMartino.csv")

print("Finished writing to csv file")
