folder_path=$1
imgs=$(ls ${folder_path} | grep up)
bin_path=$2
cd ${folder_path}
for up_img in ${imgs}
do
down_img=${up_img/up/down}
${bin_path} ${folder_path}'/'${down_img} ${folder_path}'/'${up_img} ${folder_path}'/trans'${up_img/up/}
done
