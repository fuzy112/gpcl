#!/bin/bash
#
# update-copyright.sh
# ~~~~~~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2026 Zhengyi Fu <i@fuzy.me>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

username=$(git config user.name)
year=$(date +%Y)
declare -a changed_files
git diff --cached --name-only >.file_list
val=0

while IPS='' read file; do
	sed -i "/Copyright (c) \\+[0-9]\\+\\(-[0-9]\\+\\)\\? \\+${username}/{
		/${year}/!{
			s/\\([0-9]\\+\\)\\(-[0-9]\\+\\)\\?/\\1-${year}/
			T
			H
		}
	}
	\${p; x; /./Q 1 ; Q  }" "$file"
	if [ $? != '0' ]; then
		val=1
	fi
done < .file_list

rm .file_list

exit $val
