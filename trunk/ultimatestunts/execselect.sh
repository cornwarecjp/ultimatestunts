#!/bin/bash

#File written by CJP
#Purpose: choose between binaries
#Fill in the applications below


PATH="./ultimatestunts:./stuntsserver:./stuntsai:./stunts3dedit:./trackedit:$PATH"
#ustuntsserver
#ustuntstrackedit
#stunts3dedit
ustunts 2> debuglog.txt

echo "Program stopped. Press Enter."
read
