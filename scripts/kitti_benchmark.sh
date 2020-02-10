#!/bin/bash

USAGE="Usage: ./kitti_benchmark.sh <ENV> <SEQUENCE_NUMBERS>\n"
RESULTSFILE=./results.txt
LOGS=./logs
MONOEXEC=../scripts/mono_slam_disk_demo.sh
RTEXEC=../build/RUNTIME
ACCEXEC=./evaluate_result

echo -ne "\n\t################# BENCHMARK MONO_SLAM ######################\n\n"

# checks if logs dir exists
if [ -f "$LOGS" ]; then
    echo -e "$LOGS already exists.\n"
    echo -e "Press 'y' to overwrite it or q' to quit"
    read op
    if [ "$op" = y ] ; then
        rm -rf $LOGS
        mkdir logs
    elif [ "$op" = q ] ; then
        echo -e "\nExiting...\n"
        exit 0
    else
        echo -e "\nUnknown command... Exiting...\n"
        exit 1
    fi
else
    mkdir logs
fi

# Running Mono depth
echo -ne "\n############################################################################\n"
echo -ne "\nRunning Mono Slam on seq $2\n\n"
${MONOEXEC} ${1} ${2} > ${LOGS}/${2}.txt

# Make file to store runtime results
if [ -f "$RESULTSFILE" ]; then
    echo -e "$RESULTSFILE already exists.\n"
    echo -e "Press 'y' to overwrite it or q' to quit"
    read option
    if [ "$option" = y ] ; then
        rm -rf "$RESULTSFILE"
        echo -ne "SEQ\t\tEncoder time\t\tDecoder time\t\tTrack time\t\tIteration time\n" > $RESULTSFILE
    elif [ "$option" = q ] ; then
        echo -e "\nExiting...\n"
        exit 0
    else
        echo -e "\nUnknown command... Exiting...\n"
        exit 1
    fi
else
    echo -ne "SEQ\t\tEncoder time\t\tDecoder time\t\tTrack time\t\tIteration time\n" > $RESULTSFILE
fi

echo -ne "\n\n\n"

# Compile runtime to store runtimes in results.txt
for seq in logs/* ; do
    echo -e "\nBenchmarking sequence $seq...\n"
    ${RTEXEC} ${seq} >> ${RESULTSFILE}
done

# Compile evaluate_result and stored in stats.txt
# ${ACCEXEC}

# Show runtime bencmark results
echo -ne "\n############################################################################\n\n"
echo -ne "\t############### RUNTIME BENCHMARK RESULTS ###################\n\n"
cat results.txt
echo -ne "\n\n"

# Show accuracy bencmark results
# echo -ne "\n############################################################################\n\n"
# echo -ne "\t############### ACCURACY BENCHMARK RESULTS ###################\n\n"
# cat stats.txt
# echo -ne "\n\n"

exit 0