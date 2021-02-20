# this is a profiling tool.

dir=$(ls tests/fjb-samples/* -d| fzf)
file=$(ls $dir | fzf)
rm *callgrind*
valgrind --tool=callgrind --dump-instr=yes --simulate-cache=no --collect-jumps=yes ./fjb.out $dir/$file
gprof2dot --format=callgrind -n0 -e0 --path=fjb --output=out.dot *callgrind*out* ; dot -Tsvg out.dot -o graph.svg && echo "done" &
kcachegrind ./*callgrind
