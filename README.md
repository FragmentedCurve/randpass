# randpass - A Random Password Generator

An easy and flexible password generator. Many websites restrict what
symbols can be used in a password. Lower your standards and meet those
requirements by doing,

    randpass -AaN -c 25 -r "Valid symbols here"

Integrate with password-store by doing,

    randpass -AaN -c 25 -r "Valid symbols here" | pass insert -e Password/Store/Name

Do you need a random 8 bit byte, randpass has you covered,

    randpass -r 01 -c 8

On your friend's computer and need a random octal when he's not
looking -- no problem, just do

    for f in $(find ~/); do
        chmod $(randpass -n -r 01234567 -c 3) $f
    done

~~If you clone now, we'll even throw in a random MAC generator for
free!~~ _Offer expired._

## Off-label Uses

Random MAC:

    for i in `seq 1 6`; do 
	    ./randpass -c 2 -N -r ABCDEF;
    done | tr '\n' : | rev | cut -b 2-

Flip a coin:

    randpass -r HT -c 1
	
A base64 encoded AES-256 key:

    AESKEY=`randpass -n -r +/ -AaN -c 43`=


