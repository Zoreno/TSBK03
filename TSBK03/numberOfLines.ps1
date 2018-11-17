echo "Number of lines:"

(dir -include *.asm,*.inc,*.h,*.cpp,*.vert,*.frag,*.geom,*.tctl,*.tevl,*.inl -exclude stb_image.h,imgui.cpp,imgui_demo.cpp,imgui_draw.cpp,imgui_impl_glfw.cpp,imgui_impl_opengl3.cpp,imgui_widgets.cpp,imconfig.h,imgui.h,imgui_impl_glfw.h,imgui_impl_opengl3.h,imgui_internal.h,imstb_rectpack.h,imstb_textedit.h,imstb_truetype.h -recurse | select-string .).Count

echo "Number of files:"
(dir -include *.asm,*.inc,*.h,*.cpp,*.vert,*.frag,*.geom,*.tctl,*.tevl,*.inl -exclude stb_image.h,imgui.cpp,imgui_demo.cpp,imgui_draw.cpp,imgui_impl_glfw.cpp,imgui_impl_opengl3.cpp,imgui_widgets.cpp,imconfig.h,imgui.h,imgui_impl_glfw.h,imgui_impl_opengl3.h,imgui_internal.h,imstb_rectpack.h,imstb_textedit.h,imstb_truetype.h -recurse).Count

pause
