#! /bin/bash
bash compile.sh
python3 scripts/tests-layer-class-1.py
python3 scripts/diff-output-layer-class-1.py
