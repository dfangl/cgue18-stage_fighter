# cgue18-stage_fighter
_"Stage Fighter - The Lost Warrior"_ is a single-player action bullet hell game 
where the player has to eliminate various enemies in an arena.


## Building
1. Make sure a C++11 compiler is installed which does support posix threads (or VS2017 toolchain)
2. This repository uses git-lfs which means that git-lfs must also be installed 
3. Clone repository with `--recursive` if no external dependencies are installed on the
system or use `git submodule --init <name>` to initialize und update the dependency
4. Run cmake an generate all the Makefiles for your environment
5. Build with the Makefiles 

### With Visual Studio 2017:
* `cmake -G "Visual Studio 15 2017 Win64" .`
* Open Project and correct the Linker (/MT to /MD) if needed
* Change the working directory of the build path the the project-root/resources

### Clion:
* Import as new Project or directly clone from github
* Under windows make sure to have either the visual studio toolchain > 2017 or mingw with posix support installed

## Dependencies
The dependencies can either be build from source or installed in a location which is searched by find_package
* Bullet 2.8.7
* GLFW 3.2
* LUA 5.3
* glad (generated from webservice)
* Kaguya
* spdlog