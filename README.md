# RUNCPP : An Easy-to-use Build System

Runcpp is an open-source software tool that has been meticulously designed to facilitate the generation of executable files and other non-source files from program source files. This cutting-edge tool provides users with a seamless and hassle-free means of automating their build process, while ensuring the absence of invalid memory access or leaks.

One of the key features of runcpp is its reliance on the `openutils` library, which has been developed entirely by me and has memory leak-proof nature. The tool has been implemented in C/C++, which includes a parser, as well as processes for creation, input and output. This unique combination of features enables users to generate executable files with ease and in a fraction of the time it would take to do so manually.

Another noteworthy aspect of runcpp is its cross-platform compatibility. Whether you are working on a Windows-based system or a Unix-based one, runcpp can be seamlessly integrated into your workflow. For Windows-based systems, runcpp relies on the `windows.h` API, while for Linux, Mac or Unix-based systems, it relies on `fork()` and `execvp()`. This versatility makes it an ideal tool for developers who work across multiple platforms.

Moreover, runcpp boasts a user-friendly syntax that is both simple to understand and maintain. It features an intuitive set of commands that allow users to easily manage and generate executable files from their source files. To further enhance its accessibility, runcpp is accompanied by a comprehensive set of documentation that provides detailed information on its syntax, as well as its various features and capabilities.

Overall, runcpp is a powerful and sophisticated tool that can streamline your build process and help you generate executable files with ease. Its advanced features, cross-platform compatibility, and user-friendly syntax make it an invaluable asset to any developer looking to optimize their workflow and simplify their code generation process.

## Features
- [X] Comments
- [X] Import another file
- [X] Better syntax and logical error detection
- [X] Cross-platform
- [X] Optimized and fast
- [X] Memory safe
- [X] Target calling
- [X] Get target's particular values using indexes
- [X] Run and parse other program's output using `shell`
- [X] Dependency checker using `depends`
- [X] Compiles on `g++`, `clang++`, `mingw` and `msvc`
- [X] Custom written lexer and parser
- [X] Serialization and deserialization for faster execution of commands
- [X] If-else statements
- [ ] GUI Client
