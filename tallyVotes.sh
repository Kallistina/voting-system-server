#!/bin/bash  
#this is a bash file


# 1.1) check if inputFile exists
File=inputFile
if ! [[ -f "$File" ]]; then  
    echo "inputFile File does not exist"  
    exit
fi  

# 1.2) check if we have the proper persmissions
if ! [[ $(stat -c "%A" $File) =~ "r" ]]; then
  echo "No permissions!"
  exit
fi

# 2) read from inputFile and count the votes
awk '!a[$1$2]++' $File > inputFile_noDuplicates               #eliminate duplicates(same name-same last name) from inputFile and put the output in inputFile_noDuplicates
awk '{sub(" *$","");s=substr($0, index($0,$3));o[s]++;}END { for (i in o) printf("%-20s %d\n", i, o[i]);}' inputFile_noDuplicates > tallyResultsFile_unsorted      #put results in file tallyResultsFile
sort -k2 -nr tallyResultsFile_unsorted > tallyResultsFile
rm inputFile_noDuplicates  #delete file
rm tallyResultsFile_unsorted