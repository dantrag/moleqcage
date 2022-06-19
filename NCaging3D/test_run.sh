#!/bin/bash

objects_prefix="./experiments/guests/"
objects_suffix=".xyzr"
objects=(1517448 788958 721750 145698)

epsilons=(0.4456 0.5154 0.5444 0.5821)

obstacles_prefix="./experiments/hosts/"
obstacles_suffix=".xyzr"
obstacles=(1020548 1228171 196141 279300 720848 720850)

grid_file="./experiments/grids/yershova_1_3_graph.dat"

for obs in ${obstacles[@]}; do
	obs_file=$obstacles_prefix$obs$obstacles_suffix
	for ((i=0;i<${#objects[@]};++i)); do
		obj=${objects[i]}
		eps=${epsilons[i]}
		obj_file=$objects_prefix$obj$objects_suffix
		echo Running guest $obj and host $obs with epsilon $eps ...
		eval ./NCaging3D -g $grid_file -o $obj_file -o $obs_file -e $eps -i 100000 -f ./experiments/results/$obs\_$obj\_yershova_1_3_graph\_$eps\_100000_nongui_1.cjb
	done
done

