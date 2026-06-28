# acronym-builder

Utility for building acronyms for a given set of words

## Building 

To build this project you will need a version of CMake > 3.12, a compiler that supports at least C++17, and a build system (eg. make).

```bash
# clone and setup the repo:
git clone https://github.com/khelmka/acronym-builder.git
cd acronym-builder

# clone the git submodules
cd deps
git submodule update --init --recursive
cd ..

# create the build directory
mkdir build
cd build

# invoke cmake and make
cmake .. && make
```

## Usage 
```bash
-i    # specify Input .txt filepath
-d    # specify Dictionary .txt filepath
-o    # specify Output .txt filepath
-r    # Allow repetitions of letters in the acronym (optional)
```

## File Format
All file formats (input, output, dictionary) (.txt) must contain words that are separated by newline characters. i.e. each line in the text file represents a unique word.

For example, your **input** file may look like this:
```
Engineering
Mathematics
Science
Technology
```

Your **dictionary** file may look like this:
```
stellulate
stelography
stem
stema
stembok
stemform
```

Your **output** file may look like this:
```
stem
```

## Dependencies
The following project dependencies are provided as git submodules:
```bash
https://github.com/adishavit/argh.git
```