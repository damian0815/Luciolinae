#!/bin/bash

echo $1 | awk '{printf "%c",$1 }'
