# **Battlesh!p Game**

## About The Project

This is a battlesh!p game developed as an university project.

## Getting Started
How to install?

### Prerequisites
You need an **SFML library**, **SFE Movie library** with all included decoders and **cmake** to build project. Find installation instructions on official websites.  
_SFML version >= 2.5.1_  
_cmake version >= 3.20_  
_SFE Movie version > 1.0_  

### Installation
_Now you can start to build and install game_

1. Clone the repo and go to project folder
  ```sh
  git clone https://gitlab.akhcheck.ru/artiom.borisov/battleship.git
  cd battleship
  ```

2. Make a build directory and go inside
  ```sh
  mkdir build
  cd build
  ```

3. Now you can start to build and install game. num_cores is a number of your processor's cores (usually 16).
  ```sh
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build . --config Release -- -j <num_cores + 1>
  sudo cmake --install . 
  ```
4. Clean build files
  ```sh
  cmake --build . --target clean
  ```

## Usage
  Just start the game using the following command:
  ```sh
  battlesh1p
  ```

----------------------------

### ***By NThemeDEV && ortbor***
