#! /bin/bash

RED="\x1b[31m"
GRN="\x1b[32m"
RST="\x1b[0m"
GREEN_OK="[${GRN}ok${RST}]"
RED_FAIL="[${RED}FAIL${RST}]"


echo "**********************"
echo "Checking whether we have all dat files needed ..."
echo "**********************"

MISSING=0
cat needed_dat_files | \
while read FILE; do
    ls ${FILE} &> /dev/null
    if [ $? -ne 0 ]; then
    	echo -e "File \"$FILE\" is missing. $RED_FAIL"
    	# echo "File \"$FILE\" is missing (maybe more). Please make sure you created all necessary dat files."
    else
        echo -e "File \"$FILE\" exists! $GREEN_OK"
    fi
done 
echo "**********************"

