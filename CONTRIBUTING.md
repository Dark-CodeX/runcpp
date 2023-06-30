# Contributing

## Reporting Bugs

If you find a bug, please open an issue on the [GitHub repository](https://github.com/Dark-CodeX/runcpp/issues) with a clear description of the problem. Please include any relevant information such as error messages, steps to reproduce the problem, and your operating system and compiler version.

## Suggesting Enhancements

If you have an idea for how to improve the project, please open an issue on the [GitHub repository](https://github.com/Dark-CodeX/runcpp/issues) with a clear description of your proposed enhancement. Please include any relevant information such as use cases, code examples, and any related issues or pull requests.

## Want To Improve Something

If you had done some changes in the repository on your local system, then you can open a pull request on the [GitHub repository](https://github.com/Dark-CodeX/runcpp/pulls) with a clear description of your changes and some examples, be sure to sign the commit and follow the below rules.

## Rules

- Use C standard `printf`, `fprintf` or any other functions instead of C++ standard `std::cout` or `std::cin`.
- Use C standard `FILE` or `openutils.chunkio` instead of C++ standard streams when handling I/O on disk.
- When performing disk I/O make sure to report any error with very descriptive errors and also add `strerror(errno)`.
- Make sure to handle errors by various function using their return values.
- You can use `std::string`, but `openutils::sstring` is recommended, similar case with `std::vector`, `std::pair`, `std::optional` or `std::array`, I recommend you to use `openutils` library.
- Make sure that there is no memory leaks in your code.
- Make your code flow a direction even if something goes wrong try to exit using only `main()` function instead of instant exit using `exit()` function.
- Add descriptive comments to your code if it's complicated or something innovative.
- NOTE: Do NOT use `openutils.map` as it's deprecated, instead use C++ STL library.
- Format your using `clang-format`.