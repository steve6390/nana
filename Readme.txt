




INPUT_FILTER = "sed -e 's/\/\/.*FIXME/\/\/\/ \\todo/'"

This filter replaces all instances of // FIXME (with any amount (or none) of whitespace between // and FIXME) 
with /// \todo. This substitution is made internally by Doxygen only: your source files are not modified on disk.

http://stackoverflow.com/questions/8535635/can-doxygen-easily-be-configured-to-recognise-todo-and-fixme-lines