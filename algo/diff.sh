#! /bin/bash
bash compile.sh
rm -rf scripts/output-layer-class-1
mkdir scripts/output-layer-class-1
python3 scripts/tests-layer-class-1.py
python3 scripts/diff-output-layer-class-1.py
