times=0
counts=0
for num in $(cat $1 | grep -i colortransfer | grep ms | awk '{print $12}')
do 
times=$((10#${num/ms,/}+${times}))
counts=`expr ${counts} + 1`
# echo ${num/ms,/} ${times} ${counts}
done
echo 'avg time: '`expr ${times} / ${counts}`'ms'