#!/bin/bash     
#this is a bash file

# 1) check input args
re='^[0-9]+$'                                                                                       #check if the numLines argument is an integer
if ! [[ $2 =~ $re ]] ; then
   echo "error: NumLines Not a number" >&2; exit 1
   exit
fi

# 2) create inputFile
>inputFile2                                                                                         

# 3) fill in inputFile
for((i=1; i<=$2; i++))                                                                            
do
    RANDOM_LENGTH=$((($RANDOM % 10)+3))                                                             #length from 3 to 12 chars for name and last name
    NAME=$(< /dev/urandom tr -dc 'a-z' | fold -w $RANDOM_LENGTH | head -n 1)                        #random name
    RANDOM_LENGTH=$((($RANDOM % 10)+3))                                                 
    LAST_NAME=$(< /dev/urandom tr -dc 'a-z' | fold -w $RANDOM_LENGTH | head -n 1)                   #random last name
    PP=$(shuf -n 1 $1)                                                                              #random political party
    echo $NAME $LAST_NAME $PP >> inputFile2                                                          # put the new line in the file
done