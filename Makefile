REBAR := rebar3

.PHONY: all
all: compile

compile:
	$(REBAR) compile

.PHONY: clean
clean: distclean

.PHONY: distclean
distclean:
	@rm -rf _build erl_crash.dump rebar3.crashdump rebar.lock

.PHONY: xref
xref:
	$(REBAR) xref

.PHONY: eunit
eunit: compile
	$(REBAR) eunit verbose=truen

.PHONY: ct
ct: compile
	$(REBAR) as test ct -v

cover:
	$(REBAR) cover

.PHONY: dialyzer
dialyzer:
	$(REBAR) dialyzer

.PHONY: test
test: ct

.PHONY: check
check: clang-format

.PHONY: clang-format
clang-format:
	clang-format-10 --Werror --dry-run c_src/*
