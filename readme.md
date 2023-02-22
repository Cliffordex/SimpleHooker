# SimpleHooker
A program to inject a DLL into a running process for debugging purposes.

Important: The process must match architecture.
If the process is 32bit, SimpleHooker and the payload must be built as 32bit as well.

### Usage
Usually used in a batch file with something like the following lines.

`start SimpleHooker.exe <process_name> <dll_path>`

`<Process.exe>`

`pause`
