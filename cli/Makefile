
CC		:= gcc
LD		:= gcc
AR		:= ar

LIBS		:= -lreadline
DEPFLAGS	:= -I.
LDFLAGS		:=
CFLAGS		:= -g -W -Wall -Wunused -Wmissing-prototypes \
		-Wcast-qual -Wcast-align -Werror \
		$(DEPFLAGS)



all: x-all


BLNK	:=
LLNK	:=
SRC	:=
OBJ	:=



############################################################
# lib

SRC_lib		:= sock.c session.c client.c proto.c event.c iterate.c \
		user.c track.c player.c random.c queue.c sleep.c \
		history.c help.c tag.c artist.c album.c
SRC_lib		:= $(patsubst %,mservclient/%,$(SRC_lib))
OBJ_lib		:= $(patsubst %.c,%.o,$(SRC_lib))
LLNK += libmservclient.a
SRC += $(SRC_lib)
OBJ += $(OBJ_lib)
libmservclient.a: $(OBJ_lib)

############################################################
# mservcli

SRC_mservcli	:= \
		tty.c formatter.c events.c command.c \
		mservcli.c
OBJ_mservcli	:= $(patsubst %.c,%.o,$(SRC_mservcli))
BLNK += mservcli
SRC += $(SRC_mservcli)
OBJ += $(OBJ_mservcli)
mservcli:  $(OBJ_mservcli) libmservclient.a

############################################################

x-all: $(LLNK) $(BLNK) 

DEPS	:= $(OBJ:.o=.d)
include $(DEPS)

%.d: %.c
	@echo mkdep $<
	@$(CC) $(DEPFLAGS) -M -MG $< | \
		sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@


.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(LLNK):
	$(AR) -r $@ $^

$(BLNK):
	$(LD) $(LDFLADS) -o $@ $^ $(LIBS)

todo:
	@find . -type f | grep '\.[ch]$$' | xargs grep -i todo 

clean:
	rm -f $(OBJ)
	rm -f $(LLNK) $(BLNK)
	rm -f $(DEPS)


