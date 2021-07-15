# Specify extensions of files to delete when cleaning
CLEANEXTS   = o so d

# Specify the source files, the target files, 
# and the install directory 
NAME = bangbang
HMACDIR=./hmac256
SOURCES     = $(NAME).c bangbang_sharedlib.c  $(HMACDIR)/hmac-sha256.c $(HMACDIR)/sha256.c
CPPFLAGS+=-I$(HMACDIR)
CPPFLAGS+=-I../../../uberxmhf/uxmhf-rpi3/uobjcoll/uobjs/main/include
CPPFLAGS+=-I../../../uberxmhf/uxmhf-rpi3/rgapps/linux/libs/libuhcall/include
CPPFLAGS+=-DUOBJCOLL
LDFLAGS+=-L../../../uberxmhf/uxmhf-rpi3/rgapps/linux/libs/libuhcall
OUTPUTFILE  = lib$(NAME).so
INSTALLDIR  = .
CC=arm-linux-gnueabihf-cc
LD=arm-linux-gnueabihf-ld
CXX=arm-linux-gnueabihf-g++

.PHONY: all
all: $(OUTPUTFILE)

# Build .so from .o, subst is the search-and-replace 
$(OUTPUTFILE): $(subst .c,.o,$(SOURCES)) 
	$(CXX) -shared -fPIC $(LDFLAGS)  -Wl,--whole-archive -luhcall -Wl,--no-whole-archive -o $@ $^ 

.PHONY: install
install:
	mkdir -p $(INSTALLDIR)
	cp -p $(OUTPUTFILE) $(INSTALLDIR)

.PHONY: clean 
clean:
	for file in $(CLEANEXTS); do rm -f *.$$file; rm -f $(HMACDIR)/*.$$file; done

# Generate dependencies of .c files on .h files
include $(subst .c,.d,$(SOURCES))

%.d: %.c
	$(CC)  -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
rm -f $@.$$$$
