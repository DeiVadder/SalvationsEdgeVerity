# SalvationsEdgeVerity

Welcome to the **SalvationsEdgeVerity** project repository. This project aims to provide a solution for 2D and 3D shape selection and manipulation, specifically designed to assist players in the "Salvation's Edge" Raid in Destiny 2.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation

### Prerequisites

- [Qt 5.x or later](https://www.qt.io/download)
- A C++ compiler compatible with your system

### Method 1: Build from Source

1. Clone the repository to your local machine:

    ```sh
    git clone https://github.com/DeiVadder/SalvationsEdgeVerity.git
    cd SalvationsEdgeVerity
    ```

2. Set up the build environment:

    ```sh
    mkdir build
    cd build
    qmake ..
    make
    ```

3. Run the application:

    ```sh
    ./SalvationsEdgeVerity
    ```

### Method 2: Use Precompiled Binary

1. Download the precompiled binary from the [releases page](https://github.com/DeiVadder/SalvationsEdgeVerity/releases/download/1.0.0/Salvations.Edge.Encounter.4.Verity.zip).

2. Unzip the downloaded file to a location of your choice.

3. Navigate to the unzipped directory and run the executable `SalvationsEdgeEncounter.exe`.

## Usage

**SalvationsEdgeVerity** is a helper app for the Destiny 2 raid "Salvation's Edge," specifically for Encounter 4: Verity.

1. **Call Outs by Teleported Players**: Players who are teleported call out the symbols (Triangle, Square, or Circle) from left to right for each statue. Select the order of these symbols in Line 1 of the app.
2. ![image](https://github.com/DeiVadder/SalvationsEdgeVerity/assets/31315448/56e00e36-0fea-40c1-bf14-2c3222354348)

3. **3D Forms Entry by Outside Players**: Players outside observe the 3D forms that the statues are holding and select the 3d shapes they see from the selection in Line 2 of the app.
4. ![image](https://github.com/DeiVadder/SalvationsEdgeVerity/assets/31315448/6d28ee91-22ba-4402-90f9-05768e99ad5d)

5. **Calculate Steps**: The app calculates the necessary steps (Step 1, Step 2, etc.) required to complete the encounter successfully.
6. ![image](https://github.com/DeiVadder/SalvationsEdgeVerity/assets/31315448/334b0157-6e91-413b-8da5-5a1d25bd0ec6)


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
