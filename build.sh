echo "Compiling and linking"

g++ ../src/main.cpp -o game `sdl2-config --cflags --libs` -L/opt/vc/lib/ -lbrcmGLESv2 -I/opt/vc/include -fpermissive


