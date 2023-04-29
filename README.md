# About
This program is a simple database with primary goal of being used through bash scripts or launchers like Dmenu or Rofi to store and recall data, primarely web links.

The program stores data in a plain text file, allowing it to be version controlled. The program allows storing data in multiple independent databases. It can save and recall data either by index or by a text alias.

Need for this program came up for me after my browser randomly lost all my bookmarks, proving itself unreliable at best. Second motivation was an to learn to code in C.

# Start
The `build.sh` script builds the program and installs it to $HOME/.local/bin. The script is set up to use clang to compile the code.
```
./build.sh
```

 After installation use bookkeeper -h flag to see all the options available.
```
bookkeeper -h
```

# License
Copyright © 2023 Purrie Brightstar.
License  GPLv3: GNU GPL version 3
<https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
