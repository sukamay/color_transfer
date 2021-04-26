if [ $# -lt 5 ];
then
echo 'usage:'$0' [bin_path] [source_img_name] [target_img_name] [output_folder_path] [space]'
exit 1;
fi
source=`find imgs | grep test | grep $2`
target=`find imgs | grep test | grep $3`
# echo $1 -source $source -target $target -space $5 -output $4/$5_$2_$3.jpg
for num in $(seq 1 1)
do
# echo $num
$1 -source $source -target $target -space $5 -output $4/$5_$2_$3.jpg
done