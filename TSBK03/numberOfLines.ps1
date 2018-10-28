echo "Number of lines:"

(dir -include *.asm,*.inc,*.h,*.cpp,*.vert,*.frag -recurse | select-string .).Count

pause
