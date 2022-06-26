#!/usr/bin/bash
#ASAN_OPTIONS=halt_on_error=0 ## only for # -fsanitize-recover=address
ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer ./Messenger 10.0.2.15 2000 2001 2002 2003 2004 user1 FALSE 7656 TRUE 5000 7000
