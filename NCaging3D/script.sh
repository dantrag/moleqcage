#!/bin/bash

objects_prefix="~/cagingForMolecules/molecules/guests/"
objects_suffix=".txt"
objects=(1012397 1104379 1169565 1421872 1449823 1567328 1588680 172619 297677 673626 797298 848252 907758)

obstacles_prefix="~/cagingForMolecules/molecules/hosts/"
obstacles_suffix=".txt"
obstacles=(1006531 1018380 1020548 1020551 1021396 1027975 1027976 1100541 1228171 131990 1578448 196141 211386 279300 707056 717929 720848 720849 720850 760652 760653 778897 789520 809740 814042 819686 819687 819689 848479 860485 860486 867149 867152 883369 904717 913184 935198 955292 955293 955294 955295 979941 991216 991217 991218 998365)

grid_file="~/cagingForMolecules/build/yershova_1_1_graph.dat"

for obj in ${objects[@]}; do
	obj_file=$objects_prefix$obj$objects_suffix
	for obs in ${obstacles[@]}; do
		obs_file=$obstacles_prefix$obs$obstacles_suffix
		echo Running object $obj and obstacle $obs...
		eval ./NCaging3D -g $grid_file -o $obj_file -o $obs_file -e 0.36 -i 100000 -f $obj\_$obs
	done
done

