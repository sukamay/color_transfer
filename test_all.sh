# echo "001 002 LAB:"
# time sh test_perf.sh build/ColorTransfer 001 002 trans_results LAB > perf.log
# sh cal_time.sh perf.log
# echo "001 002 CAM97:"
# time sh test_perf.sh build/ColorTransfer 001 002 trans_results CAM97 > perf.log
# sh cal_time.sh perf.log
# echo "001 002 OPENCV_LAB:"
# time sh test_perf.sh build/ColorTransfer 001 002 trans_results OPENCV_LAB > perf.log
# sh cal_time.sh perf.log
# echo "001 002 RGB:"
# time sh test_perf.sh build/ColorTransfer 001 002 trans_results RGB > perf.log
# sh cal_time.sh perf.log

# echo "008 010 LAB:"
# time sh test_perf.sh build/ColorTransfer 008 010 trans_results LAB > perf.log
# sh cal_time.sh perf.log
# echo "008 010 CAM97:"
# time sh test_perf.sh build/ColorTransfer 008 010 trans_results CAM97 > perf.log
# sh cal_time.sh perf.log
# echo "008 010 OPENCV_LAB:"
# time sh test_perf.sh build/ColorTransfer 008 010 trans_results OPENCV_LAB > perf.log
# sh cal_time.sh perf.log
# echo "008 010 RGB:"
# time sh test_perf.sh build/ColorTransfer 001 010 trans_results RGB > perf.log
# sh cal_time.sh perf.log

# echo "006 003 LAB:"
# time sh test_perf.sh build/ColorTransfer 006 003 trans_results LAB > perf.log
# sh cal_time.sh perf.log
# echo "006 003 CAM97:"
# time sh test_perf.sh build/ColorTransfer 006 003 trans_results CAM97 > perf.log
# sh cal_time.sh perf.log
# echo "006 003 OPENCV_LAB:"
# time sh test_perf.sh build/ColorTransfer 006 003 trans_results OPENCV_LAB > perf.log
# sh cal_time.sh perf.log
# echo "006 003 RGB:"
# time sh test_perf.sh build/ColorTransfer 001 003 trans_results RGB > perf.log
# sh cal_time.sh perf.log
sources=(001 008 006)
targets=(002 010 003)
# color_spaces=(LAB CAM97 OPENCV_LAB RGB)
color_spaces=(OPENCV_LAB)
for i in "${!sources[@]}";   
do   
    for space in ${color_spaces[*]}
    do
    echo ${sources[$i]} ${targets[$i]} ${space}
    time sh test_perf.sh build/ColorTransfer ${sources[$i]} ${targets[$i]} trans_results ${space} > logs/perf_${sources[$i]}_${targets[$i]}_${space}.log 2>&1
    sh cal_time.sh logs/perf_${sources[$i]}_${targets[$i]}_${space}.log
    done
done 