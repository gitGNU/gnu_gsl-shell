CAMLC = ocamlc
CAMLO = ocamlopt
COMPFLAGS = -g
LINKFLAGS = -g

IMPL_FILES = lexer.ml variable.ml math.ml exprPrint.ml proto.ml mathParser.ml testing.ml

INTFC_FILES = variable.mli proto.mli mathParser.mli exprPrint.mli

CMI_FILES = $(INTFC_FILES:%.mli=%.cmi) 
CMO_FILES = $(IMPL_FILES:%.ml=%.cmo) 
CMX_FILES = $(IMPL_FILES:%.ml=%.cmx)

DEP_MAGIC := $(shell ocamldep $(IMPL_FILES) $(INTFC_FILES) > .depend )

.SUFFIXES: .mli .cmi .cmx .ml .mll .mly .cmo

all: mytest

%.ml: %.mll
	ocamllex $<

%.ml %.mli: %.mly
	@rm -fr $(<:%.mly=%.ml) $(<:%.mly=%.mli)
	ocamlyacc $<
	chmod a-w $(<:%.mly=%.ml) $(<:%.mly=%.mli)

%.cmi: %.mli
	$(CAMLC) $(COMPFLAGS) -c $<

%.cmo: %.ml
	$(CAMLC) $(COMPFLAGS) -c $<

%.cmx: %.ml
	$(CAMLO) -c $<

mytest: $(CMI_FILES) $(CMO_FILES)
	ocamlmktop $(LINKFLAGS) -o mytest nums.cma $(CMO_FILES)

mytest.opt: $(CMI_FILES) $(CMX_FILES)
	$(CAMLO) -o math.opt nums.cmxa $(CMX_FILES)

clean:
	rm -fr *.o *.cmx *.cmo *.cmi mytest mytest.opt

-include .depend
