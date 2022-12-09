data_arr=("friendster")
# "pokec" "twitter" "friendster" "orkut"
pdists=("uniform" "exp" "weibull" "wc")
pedge=0.1
wcvariant=1
rrsetnum=1024000

for data_name in "${data_arr[@]}"
do	
{
  for pdist in "${pdists[@]}"
  do
  {
    if [ ${pdist} == "uniform" ];then 
      ./subsim -func=format -gname=${data_name} -pdist=${pdist} -pedge=${pedge} -dir=/data/lu_yi/dataset/${data_name} > ./runlog/${data_name}_${pdist}_${pedge}_format 
      ./subsim -func=im -gname=${data_name} -pdist=${pdist} -pedge=${pedge} -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} > ./runlog/${data_name}_${pdist}_${pedge}
      ./subsim -func=im -gname=${data_name} -pdist=${pdist} -pedge=${pedge}  -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} -vanilla=1 > ./runlog/${data_name}_${pdist}_${pedge}_vanilla
    elif [ ${pdist} == "wc" ];then
      ./subsim -func=format -gname=${data_name} -pdist=${pdist} -wcvariant=${wcvariant} -dir=/data/lu_yi/dataset/${data_name} > ./runlog/${data_name}_${pdist}_${wcvariant}_format 
      ./subsim -func=im -gname=${data_name} -pdist=${pdist} -wcvariant=${wcvariant} -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} > ./runlog/${data_name}_${pdist}_${wcvariant}
      ./subsim -func=im -gname=${data_name} -pdist=${pdist} -wcvariant=${wcvariant} -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} -vanilla=1 > ./runlog/${data_name}_${pdist}_${wcvariant}_vanilla
    elif [ ${pdist} == "exp" -o ${pdist} == "weibull" ];then
      ./subsim -func=format -gname=${data_name} -pdist=skewed -skew=${pdist} -dir=/data/lu_yi/dataset/${data_name} > ./runlog/${data_name}_${pdist}_format 
      ./subsim -func=im -gname=${data_name} -pdist=skewed -skew=${pdist} -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} > ./runlog/${data_name}_${pdist}
      ./subsim -func=im -gname=${data_name} -pdist=skewed -skew=${pdist} -dir=/data/lu_yi/dataset/${data_name} -rrset=1 -rrsetnum=${rrsetnum} -vanilla=1 > ./runlog/${data_name}_${pdist}_vanilla
    fi
  }&
  done 
}&
done
wait 
echo "finish"