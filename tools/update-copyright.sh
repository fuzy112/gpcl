#!/bin/sh
#
# update-copyright.sh
# ~~~~~~~~~~~~~~~~~~~
#
# Copyright (c) 2020-2023 Zhengyi Fu (tsingyat at outlook dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

set -e

username=$(git config user.name)
year=$(date +%Y)
git diff --cached --name-only |
	xargs sed -i "/Copyright (c) [0-9]\\+\\(-[0-9]\\+\\)\\? ${username}/{
		/${year}/!{
			s/\\([0-9]\\+\\)\\(-[0-9]\\+\\)\\?/\\1-${year}/
			T
			H
		}
	}
	\${p; x; /./Q 1 ; Q  }"
