echo "Number of lines:"

(dir -include *.asm,*.inc,*.h,*.cpp,*.vert,*.frag,*.geom,*.tctl,*.tevl,*.inl -recurse | select-string .).Count

pause
