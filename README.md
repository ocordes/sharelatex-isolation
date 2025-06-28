# sharelatex-isolation
Implement primitive sharelatex compiler isolation


Short information about the first tests:

* in the test only read/write operations are monitored, but one can see, that the open command is one of the commands which should be limited by white- and blacklists
* using the LD_PRELOAD-method with the environments set also works on programs which are called by system or shell commands, which is pretty nice, so the method can be configured to wrap latexmk only
* ls command is a littly bit tricky; there are different approaches for an ls command:
  * opendir/closedir/readdir was the old style
  * stat is a the better approach
  * statat must be evaluated if it based in the end on stat, so one overload is sufficient



How to use whitelists

Whitelists for read and write cases can be defined as environment variables. All entries are 
seperated with ":" or ",". The entries can be directories or individual filenames. This makes 
the masking easier.
