test_one.c    compile command
gcc test_one.c -I ../include/ -L ../lib/ -lavcodec -lavformat -lavutil
gcc test_two.c -I ../include/ -L ../lib/ -lavcodec -lavformat -lavutil -lswscale
