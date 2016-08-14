# mafin - wwi instances solver based on CPLEX
# See LICENSE file for copyright and license details.

include config.mk

SRC = mafin.c mafin_db.c debug.c
OBJ = ${SRC:.c=.o}

all: options mafin

options:
	@echo mafin build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "OBJ      = ${OBJ}"

.c.o:
	@echo GCC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

mafin: ${OBJ}
	@echo CC -o $@ 
	@${CC} -o $@ $^ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${OBJ}

mrproper:
	@echo deep cleaning
	@rm -f mafin ${OBJ} mafin-${VERSION}.tar.gz
	@echo removing database
	@rm mafin.db

dist: mrproper
	@echo creating dist tarball
	@mkdir -p mafin-${VERSION}
	@cp -R LICENSE Makefile config.mk config.def.h README \
		TODO.md mafin.1 ${SRC} mafin-${VERSION}
	@tar -cf mafin-${VERSION}.tar mafin-${VERSION}
	@gzip mafin-${VERSION}.tar
	@rm -rf mafin-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f mafin ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/mafin
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < mafin.1 > ${DESTDIR}${MANPREFIX}/man1/mafin.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/mafin.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/mafin
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/mafin.1

.PHONY: all options clean dist install uninstall

