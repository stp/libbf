# LibBF

STP's mirror of [LibBF](https://bellard.org/libbf/), Fabrice Bellard's
arbitrary-precision floating-point library. MIT licensed; see the copyright
headers in `libbf.c` and `cutils.c`.

Upstream publishes release tarballs and no git repository, so this holds
them. `master` mirrors the releases verbatim, one commit per release,
tagged `upstream-<date>`; the tarball's own hash is recorded in the commit
message. The `stp` branch is `master` plus STP's changes, one commit each,
and is what STP builds against: [stp/stp](https://github.com/stp/stp)
carries this repository as the submodule `lib/extlib-libbf`, pinned to a
commit of that branch.

STP compiles `libbf.c` and `cutils.c` and nothing else; the tests,
benchmarks, softfp templates and pi/calculator programs are here because
the mirror is verbatim, not because they are used.

## Branches

* `master` — upstream, verbatim. No STP changes here.
* `stp` — what STP builds. Currently `master` plus MSVC portability and a
  fix for an empty `memcpy` between null pointers.

## Updating to a new release

Extract the new tarball over `master` and commit it as `Import LibBF
<date>`, tag it, then rebase `stp` onto it and move STP's submodule pin.
A conflict during that rebase means an STP change has met upstream and
needs a decision, which is the point of keeping the two apart.

## A note on changes here

STP compiles these files with its own flags, which include
`-fsanitize=undefined` in one of its CI jobs, so undefined behaviour that
an ordinary build tolerates will stop that build. Fixes for it belong on
`stp`, not in a suppression on STP's side.
