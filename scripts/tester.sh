#!/bin/sh

VM="../bin/set"
DEST="../compiled_tests/*"


run_code() {
	$VM $1
}

test_compiled() {
	for file in $DEST; do
		if [ -f "$file" ]; then
			run_code $file
		fi
	done
}


test_compiled
