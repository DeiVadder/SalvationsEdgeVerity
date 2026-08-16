# SalvationsEdgeVerity

Welcome to the **SalvationsEdgeVerity** project repository. This project aims to provide a solution for 2D and 3D shape selection and manipulation, specifically designed to assist players in the "Salvation's Edge" Raid in Destiny 2.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation

### Method 1: Launch from github [recommended]
Visit the following git hub webside to launch the latest version of the app inside your browser:
[https://deivadder.github.io/SalvationsEdgeVerity/]

-------

### Method 2: Build from Source

### Prerequisites

- [Qt 6.11.1 or later](https://www.qt.io/download) (open-source)
- CMake 3.21+
- A C++17 compiler compatible with your system

Supported platforms: macOS (desktop) and WebAssembly (browser, see Method 1). Android/iOS are not currently supported.

### Steps:
1. Clone the repository to your local machine:

    ```sh
    git clone https://github.com/DeiVadder/SalvationsEdgeVerity.git
    cd SalvationsEdgeVerity
    ```

2. Configure and build:

    ```sh
    cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.11.1/macos
    cmake --build build
    ```

3. Run the tests:

    ```sh
    ctest --test-dir build
    ```

4. Run the application:

    ```sh
    ./build/SalvationsEdgeVerity
    ```

## Usage

**SalvationsEdgeVerity** is a helper app for the Destiny 2 raid "Salvation's Edge," specifically for Encounter 4: Verity. The header switches between three modes:

- **Outside**: teleported players call out the 2D symbol (Triangle/Square/Circle) each statue shows, left to right - enter those under "Select all Inside 2D shapes". Players outside then read off the 3D shape each statue currently holds - enter those under "Select all outside 3D shapes". The app calculates the swap sequence needed to reach the correct shapes, one numbered step at a time, plus a "Copy for in-game chat" button to paste the result straight into chat.
- **Inside**: for the 3 solo players in the Verity room. Pick which statue you are, everyone's own symbol, and your current wall - the app gives you your personal sort/distribute steps and final combine shape.
- **Ghosts**: a 6-player roster helper for the Ghost resurrection phase (name, class, notes).

Both Outside and Inside support an optional **Challenge Mode** ("Varied Geometry") toggle, which restricts the target escape shape to one of the 3 "pure" shapes (Pyramid/Cube/Sphere) instead of the default. The app itself is available in English and German (flag toggle, top right).


## Contributing

Contributions are welcome! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes and commit them (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Create a new Pull Request.

Please make sure to update tests as appropriate.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any questions or suggestions, feel free to reach out to:

- GitHub: [DeiVadder](https://github.com/DeiVadder)

---

Thank you for using **SalvationsEdgeVerity**!
