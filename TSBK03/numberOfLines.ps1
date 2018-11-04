echo "Number of lines:"

(dir -include *.asm,*.inc,*.h,*.cpp,*.vert,*.frag,*.inl -recurse | select-string .).Count

pause
