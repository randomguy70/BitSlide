# *BitSlide*

## ***BitSlide*** is an lightning-fast encryption program similar to `AES` (Advanced Encryption Standard). It utilizes `SHA-256` hashing to scramble any file into an unrecognizable mess only recoverable with the *exact* original password.

 The fact that ***Bitslide*** is open source **does not** mean that it can be hacked, because the encryption process is entirely dependent on the password. In other words, if you change one character of your password, the encrypted output will be entirely different! If your password is at least 8 characters and not obvious, it would take many millions of years for a hacker to crack it.

## Building

***BitSlide*** is written in pure C as a command-line tool, so building it from source is super easy! 
>-  Clone [the repository here](https://github.com/randomguy70/BitSlide) and navigate to it in a terminal
>- Run the command **`make`**. The executable binary will be in the folder labeled output.
>
**Note**: The makefile uses the `GCC` compiler, but if you are on Mac and want to use `Clang` instead, simply change [this line](https://github.com/randomguy70/BitSlide/blob/32333ceb2214c1b55195bcaac7c11b0240dce669/makefile#L7) to **`CC = clang`**.