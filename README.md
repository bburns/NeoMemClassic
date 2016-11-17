**NeoMem, an information manager**  
Version 1.2d  
C++/MFC 50k sloc
<hr/>

NeoMem is a kind of cross between a word processor and a database,
with an interface similar to Windows Explorer. 

<img src="screenshots/neomem800.png" />

You can define types of objects you want to store, e.g. Projects, 
Computers, Cars, and properties for them, e.g. Due Date, Processor, 
License Plate. Objects can be stored in folders, and the complete
tree of objects is available in a pane on the left side. 

The backend for the program is called Brooklyn, and is contained in
the BDocument, BData and BObject classes. 

Some things to do in the code are marked with //. comments, 
or //, for lower priority items, or //! for high priority. 

[Rake][1] is used to build the project, except for the actual compilation, 
as calling msdev from the cmdline seems to leave out the version info. 
Call rake -T to see the list of tasks it can do, e.g.

```
> rake -T

rake build     # build the exe (broken!)
rake compress  # Compress the executable
rake help      # Update the help file
rake install   # Update the nsi setup script
rake intro     # gives an overview of this rakefile
rake readme    # Update the readme file
rake setup     # make setup files and zips etc
rake version   # Update the project's version information
rake website   # Update the website
```


[1]: http://rake.rubyforge.org/
