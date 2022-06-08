# A Random Password Generator

An easy and flexible password generator. Many websites restrict what
symbols can be used in a password. Lower your standards and meet those
requirements by doing,

    randpass -AaN -c 25 -r "Valid symbols here"

Integrate with password-store by doing,

    randpass -AaN -c 25 -r "Valid symbols here" | pass insert -e Password/Store/Name

If you clone now, we'll even throw in a random MAC generator for free!
