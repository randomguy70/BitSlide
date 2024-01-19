# ***<div align="center"> BitSlide </div>***

## ***BitSlide*** is an lightning-fast encryption program similar to `AES` (Advanced Encryption Standard). It utilizes `SHA-256` hashing to scramble any file into an unrecognizable mess only recoverable with the *exact* original password.

 The fact that ***Bitslide*** is open source **does not** mean that it can be hacked, because the encryption process is entirely dependent on the password. In other words, if you change one character of your password, the encrypted output will be entirely different! If your password is at least 8 characters and not obvious, it would take a very very long time for a hacker to crack it.

![Demo Image](https://github.com/randomguy70/BitSlide/blob/main/images/eyeCandy.gif)

# Building

***BitSlide*** is written in pure C as a command-line tool, so building it from source is super easy!

Make sure you have [Git](https://git-scm.com/downloads) installed &
Run this command in your terminal:

    cd ~/desktop
    git clone https://github.com/randomguy70/BitSlide
    cd BitSlide
    make

The executable binary will be in the folder labeled output.
>
**Note**: The makefile uses the `GCC` compiler, but if you are on Mac and want to use `Clang` instead, simply change [this line](https://github.com/randomguy70/BitSlide/blob/934e07c619478b744b31ad513f0238af093b5a59/makefile#L7) to **`CC = clang`**.

# Running

To run ***BitSlide***, grab the latest release or see the instructions above to build it from source. Once you have an executable, copy and paste the absolute path to the executable into a terminal and hit enter. The absolute path should look something like `/Users/Name/Desktop/BitSlide/output/main` Or, you can drag & drop the executable file into a terminal and hit enter.

**Note**: Make sure you enter the necessary command line arguments!

# Command Line Arguments

    Usage:
	    bitslide [options] -i "filename"
	    
	Required options:
		-i <input file>                       Path to input file 
		-p <password>                         specifies password to be used in encrypting
		-o <option> ("encrypt" or "decrypt")  specifies whether to encrypt or decrypt
	
	Optional commands:
		-help                                 asks for help + arg syntax
Example:

    /Users/Name/Desktop/BitSlide/output/main -i input.txt -p myPasword -o encrypt
