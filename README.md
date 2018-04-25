# simple_balloon

A simple Linux balloon driver module. Allocate a specified number of pages.

## How to build and use the module

To build the module,

   $ cd module; make

To allocate memory (XXX is the number of pages to allocate),

   $ insmod balloon.ko nr_pages=XXX

To free memory,

   $ rmmod balloon

