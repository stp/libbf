# LibBF

STP's mirror of [LibBF](https://bellard.org/libbf/), Fabrice Bellard's
arbitrary-precision floating-point library. MIT licensed; see the copyright
headers in `libbf.c` and `cutils.c`.

Upstream publishes release tarballs and no git repository, so this holds
them. `master` mirrors the releases verbatim, one commit per release,
tagged `upstream-<date>`; the tarball's own hash is recorded in the commit
message. The `stp` branch is `master` plus STP's changes, one commit each,
and is what STP builds against: `scripts/deps/setup-libbf.sh` in
[stp/stp](https://github.com/stp/stp) clones this repository and checks out
a pinned commit of that branch.

STP compiles `libbf.c` and `cutils.c` and nothing else; the tests,
benchmarks, softfp templates and pi/calculator programs are here because
the mirror is verbatim, not because they are used.

## Branches

* `master` — upstream, verbatim. No STP changes here.
* `stp` — what STP builds. Currently `master` plus MSVC portability.

## Updating to a new release

Extract the new tarball over `master` and commit it as `Import LibBF
<date>`, tag it, then rebase `stp` onto it and pin the new commit in
`scripts/deps/setup-libbf.sh`. A conflict during that rebase means an STP
change has met upstream and needs a decision, which is the point of keeping
the two apart.
