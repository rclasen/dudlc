
CC		:= gcc
LD		:= gcc

LIBS		:= -lreadline
DEPFLAGS	:= -I.
LDFLAGS		:=
CFLAGS		:= -g -W -Wall -Wunused -Wmissing-prototypes \
		-Wcast-qual -Wcast-align -Werror \
		$(DEPFLAGS)



all: x-all


LNK	:=
SRC	:=
OBJ	:=


############################################################
# mservcli

SRC_mservcli	:= \
		server.c proto.c \
		tty.c \
		mservcli.c
OBJ_mservcli	:= $(patsubst %.c,%.o,$(SRC_mservcli))
LNK += mservcli
SRC += $(SRC_mservcli)
OBJ += $(OBJ_mservcli)
mservcli: $(OBJ_mservcli)

############################################################

x-all: $(LNK)

DEPS	:= $(OBJ:.o=.d)
include $(DEPS)

%.d: %.c
	@echo mkdep $<
	@$(CC) $(DEPFLAGS) -M -MG $< | \
		sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@


.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(LNK):
	$(LD) $(LDFLADS) -o $@ $^ $(LIBS)

todo:
	@find . -type f | grep '\.[ch]$$' | xargs grep -i todo 

clean:
	rm -f $(OBJ)
	rm -f $(LNK)
	rm -f $(DEPS)


