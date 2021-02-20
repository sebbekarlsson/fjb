gprof2dot --format=callgrind -n0 -e0 --path=fjb --output=out.dot $1 ; dot -Tsvg out.dot -o graph.svg
