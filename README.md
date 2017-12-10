# Classic Arcade Shoot'em'up for Raspberry PI

This is a small hobby project for producing a classical topdown shoot'em'up game,
specifically for the RaspberryPi. 

I programmed it on a model 3 and there is no tests made whether it 
runs on another Raspberry platform. 

#### BUILDING
The building is done with a custom script which will build on an 
actual ARM processor based Raspberry. 

If you want to build the game on an x86 processor, just export the 
environment variable HOST_X86 like so: 

`export HOST_X86=true`

This will then lead to lookup for different libraries inside the build script. 

To invoke the build script, I recommend to create a separate folder "build", 
and to invoke the building from inside this folder: 

```bash
$ mkdir build
$ cd build
$ ../build.sh
```
