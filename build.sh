echo "Compiling and linking"

echo ${HOST_X86}

if [ -n "$HOST_X86" ]; then
  GLES_LIB="GLESv2"
else
	# This is the ARM case 
  GLES_LIB="brcmGLESv2"
fi

echo "GLES_LIB" $GLES_LIB


g++ ../src/main.cpp -o game `sdl2-config --cflags --libs` -L/opt/vc/lib/ -l$GLES_LIB -I/opt/vc/include -fpermissive


