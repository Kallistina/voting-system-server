#!/bin/bash  
#this is a bash file

# 1.1) check if poll-log exists
File=$1
if ! [[ -f "$File" ]]; then  
    echo "poll-log File does not exist"  
    exit
fi  

# 1.2) check if we have the proper persmissions
if ! [[ $(stat -c "%A" $File) =~ "r" ]]; then
  echo "No permissions!"
  exit
fi

# 2) read from poll-log and count the votes
awk '!a[$1$2]++' $File> poll-log_noDuplicates               #eliminate duplicates(same name-same last name) from poll-log and put the output in poll-log_noDuplicates
awk '{sub(" *$","");s=substr($0, index($0,$3));o[s]++;}END { for (i in o) printf("%-20s %d\n", i, o[i]);}' poll-log_noDuplicates > pollerResultsFile_unsorted      #put results in file pollerResultsFile
sort -k2 -nr pollerResultsFile_unsorted  >pollerResultsFile
rm poll-log_noDuplicates  #delete file
rm pollerResultsFile_unsorted