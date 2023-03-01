#! /bin/bash

ROOT_PATH="../dat"

#Needs: thres.dat thres_0.2.dat
#Create normalized threshold graphs
if [ ! -f ${ROOT_PATH}/thres_0.2.dat ] || [ ! -f ${ROOT_PATH}/thres.dat ]; then
    echo "Make sure both ${ROOT_PATH}/thres_0.2.dat and ${ROOT_PATH}/thres.dat exist!"
    exit 1
fi




FILE02a=`mktemp __threshold02a.XXXXXX`
FILE05a=`mktemp __threshold05a.XXXXXX`
FILE02=`mktemp __threshold02.XXXXXX`
FILE05=`mktemp __threshold05.XXXXXX`
FILEX=`mktemp __x_axis.XXXXXX`

echo -e "1\n5\n10\n20\n50\n100\n200\n500\n1000" > $FILEX

NoUpdates=1
NoQueries=4
cat ${ROOT_PATH}/thres_0.2.dat | awk '{print $1}' | awk '{printf("%s ",$1); if (NR%2==0) print "";}' | awk '{printf("%d %d %d\n",$1, $2, $1*'$NoUpdates'+$2*'$NoQueries');}' > $FILE02a
MAX02=`cat $FILE02a | awk 'BEGIN{MAX=0}{if ($3>MAX) MAX=$3}END{print MAX}'`
cat $FILE02a | awk '{printf("%lf\n",$3/'$MAX02');}' > $FILE02


NoUpdates=1
NoQueries=1
cat ${ROOT_PATH}/thres.dat | awk '{print $1}' | awk '{printf("%s ",$1); if (NR%2==0) print "";}' | awk '{printf("%d %d %d\n",$1, $2, $1*'$NoUpdates'+$2*'$NoQueries');}' > $FILE05a
MAX05=`cat $FILE05a | awk 'BEGIN{MAX=0}{if ($3>MAX) MAX=$3}END{print MAX}'`
cat $FILE05a | awk '{printf("%lf\n",$3/'$MAX05');}' > $FILE05

paste $FILEX $FILE05 $FILE02 > ${ROOT_PATH}/threshold_normalized.dat

rm $FILE02a
rm $FILE05a
rm $FILE02
rm $FILE05
rm $FILEX
