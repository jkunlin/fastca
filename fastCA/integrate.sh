#!/bin/bash
for seed in `seq 10`
do
	for model_file in ./ssbse09\ benchmarks/*.model
	do
#		constraints_file=${model_file%_2way*}.constraints

		model_name=${model_file##*/}
		time_file=./time/${model_name%_2way*}_${seed}.time
		integrate_time_file=./integrate_time/${model_name%_2way*}.time
		touch $integrate_time_file
		tail -1 $time_file >> $integrate_time_file

#		solution_file=./solution/${model_name%_2way*}_${seed}.solution
#		echo "*********************starting: ${model_file}*********************"
#		./runsolver --timestamp -o "${time_file}" ./casa-1.1b "${model_file}" -c  "${constraints_file}" -s $seed -o "${solution_file}"
#		echo "*********************ending: ${model_file}*********************"
	done
done
