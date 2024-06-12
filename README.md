# LemASM
LemASM is an Assembly-like language that revolves around a stack, this is the bundled LemASM interpreter.

## Getting Started
### Setup For Windows User
If you are on Windows you have two options:<br>
1. You can either follow the Setup For Unix-Like OS Users.<br>
2. You can download the bundled LemASM.exe file and utilize it (however keep in mind it may not be up-to-date).<br>

Then create a ".lemasm" or ".lasm" file.<br>
There is no advantage to one or another ".lasm" is just short for ".lemasm".<br>
Lastly interpet your code with> .\LemASM.exe .<file_name>.<lemasm_file_extension> <[additonal_tags]><br>

### Setup For Unix-Like OS Users
If you are on BSD, Linux, Mac, or another Unix-Like operating system you will need to compile the interpreter.<br>
The easiest way to accomplish this is using the Makefile and running the "make" command.<br>

Then create a ".lemasm" or ".lasm" file.<br>
There is no advantage to one or another ".lasm" is just short for ".lemasm".<br>
Lastly interpet your code with> .\LemASM .<file_name>.<lemasm_file_extension> <[additonal_tags]><br>

### Additional Interpreter Tags
You can use -d to get additional debug information. Example: ./LemASM <file_name>.lemasm -d<br>
You can use -h to get directed directly to the embedded help HTML and documentaion file. Example: ./LemASM <file_name>.lemasm -h<br>
This is not yet implemented:<br>
You can use -o <output_file_name> to get an output_file. Example: ./LemASM <file_name>.lemasm -o <output_file_name>
