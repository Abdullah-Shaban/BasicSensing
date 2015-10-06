INCLUDE_DIR=/usr/local/include
SOURCE_DIR=.
LIB_DIR=/usr/local/lib

INCLUDE_PATHS=-I. -I$(INCLUDE_DIR) -I$(LIB_DIR)
LIB_PATHS=-L. -L$(LIB_DIR)

all: Objects/main.o

Objects/%.o: $(SOURCE_DIR)/%.c
	@echo creating Objects/ dir...
	@mkdir -p Objects
	@echo   ... done.
	@echo creating Output/ dir...
	@mkdir -p Output
	@echo   ... done.
	@echo
	@echo compiling...
	@gcc $(INCLUDE_PATHS) -c $< -o $@
	@echo   ... done.
	@echo
	@echo building...
	@gcc $(LIB_PATHS) $(INCLUDE_PATHS) $@ -lsensing -lswi -lpthread -lm -lusb -o Output/MAIN
	@echo   ... done.
	@echo
	
clean:
	@echo cleaning...
	@rm -f *.o *~
	@rm -f Output/*
	@rm -f Objects/*
	@echo   ... done.
	@echo

cleanall: clean
	@echo cleaning all...
	@rm -rf Output
	@rm -rf Objects
	@echo   ... done.
	@echo

# DO NOT DELETE
