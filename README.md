# cgue18-stage_fighter

## Building

1. Clone repository with `--recursive` if no external dependencies are installed on the
system or use `git submodule --init <name>` to initialize und update the dependency
2. Run cmake an generate all the Makefiles for your environment
3. Build with the Makefiles 

### With visual studio 2017 console:
```
cmake -G "Visual Studio 15 2017 Win64" .
```

### Clion:
Import as new Project

## Dependencies
* Bullet 2.8.7
* GLFW 3.2
* LUA 5.3
* glad (generated from webservice)
* Kaguya
* spdlog