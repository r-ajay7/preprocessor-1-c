
MY PREPROCESSOR - BUILD AND TEST

Files:
  main.c             Program entry point
  comments.c         Removes // and /* ... */ comments
  macros.c           Handles #define and macro substitution
  file_inclusion.c   Handles #include and creates the .i file
  preprocessor.h     Shared declarations
  Makefile           Builds the executable
  test_input.c       Sample input file
  test_header.h      Header used by test_input.c

Build:
  make

Run:
  ./mypreprocessor test_input.c

Expected output file:
  test_input.i

Check:
  1. Open test_input.i.
  2. Comments should be removed.
  3. test_header.h contents should appear instead of the #include line.
  4. PI, COUNT, HEADER_VALUE and SQUARE(COUNT) should be substituted.

Note:
  This is a small educational preprocessor for the requirements in the
  supplied project sheet. It handles local quoted/angle-bracket include
  files; it does not search system include directories or implement every
  feature of the standard C preprocessor.
