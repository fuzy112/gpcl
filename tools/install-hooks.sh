#!/bin/sh

worktree_path=$(git worktree list --porcelain  | sed -n 's/worktree // ; T ; p')
install -vm755 "$worktree_path/tools/pre-commit" -Dt "$worktree_path/.git/hooks/"
