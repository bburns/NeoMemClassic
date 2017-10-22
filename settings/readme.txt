


different compiler/project settings or makefiles go here
for now, only vs6 is being used
but could put it in a subfolder and add others if needed

eg
* linux-gcc
* mac
* win-gcc
* win-vs6
* ...

note: the neomem.opt file stores the vs6 project folders 
(ie the ones used to organize your source files), and breakpoint
positions, window configurations etc. 
unfortunately it uses a binary format. 
it seems to use hardcoded paths, but is able to reconfigure itself
if the location changes.
but if worse comes to worse, it can be deleted, and it'll be regenerated.
some vs6 projects just don't include the .opt file, but i used the
project folders to try to organize the code and make it easier to understand,
so i'm including it. 


for info on the different vs6 files, see 
  http://msdn.microsoft.com/en-us/library/aa239568(VS.60).aspx
