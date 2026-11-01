# Git workflow

Git Flow, simplified.

## Branches

| Branch | Purpose |
|---|---|
| `main` | Releases only. |
| `develop` | Integration branch. All work starts here. |
| `feature/<name>` | New functionality. From `develop`, back into `develop`. |
| `bugfix/<name>` | Bug fix during development. From `develop`, back into `develop`. |
| `release/<version>` | Release prep. From `develop`, into `main` and `develop`. |
| `hotfix/<version>` | Urgent fix on a release. From `main`, into `main` and `develop`. |

Other prefixes when needed: `refactor/`, `docs/`, `test/`, `chore/`, `experiment/`.

Names are kebab-case: `feature/deferred-rendering`.

While a feature lives, merge `develop` into it regularly.

## Commits

Conventional Commits: `<type>(<scope>): <short lowercase summary>`, imperative mood, no trailing period.
Scope is optional.

| Type | Use for |
|---|---|
| `feat` | new functionality |
| `fix` | bug fix |
| `refactor` | code change without behavior change |
| `perf` | performance |
| `docs` | documentation |
| `test` | tests |
| `build` | build system, CMake, dependencies |
| `chore` | configs, formatter, other routine |

Scope is a module or area: `core`, `gl`, `gfx`, `game`, `editor`.

```
feat(gl): add G-buffer render target
fix(game): correct shader include order
refactor(gl): rework render target / resource pooling
docs: add git workflow
chore: use BlockIndent in clang-format
```

Older history uses `<scope>: <summary>` or plain sentences; it is not rewritten.

One logical change per commit. Code and the docs that describe it may go in separate commits.

## Daily cycle

```
git switch develop && git pull
git switch -c feature/my-thing
git add -p
git commit -m "gl: add gbuffer pass"
git push -u origin feature/my-thing
```

Commit often and small, clean up later.

## Updating a feature from develop

Prefer rebase on your own branch: `git rebase develop`. It keeps history linear and drops
"Merge branch 'develop' into ..." commits. Never rewrite `main` or `develop`.

## Cleanup before merge

Collapse `wip`, `fix typo`, `oops` commits:

```
git rebase -i develop
```

| Command | Effect |
|---|---|
| `pick` | keep |
| `reword` | change message |
| `squash` | meld into previous, keep both messages |
| `fixup` | meld into previous, drop message |
| `drop` | delete commit |

Shortcut for a fix to an older commit:

```
git commit --fixup <hash>
git rebase -i --autosquash develop
```

Before rebasing: commit or stash local changes, make a backup (`git branch backup/<name>`).
After rebasing a pushed branch: `git push --force-with-lease` (own branches only).

## Merging a feature

Always with `--no-ff`, so the feature stays one visible group and can be reverted as a whole:

```
git switch develop && git pull
git merge --no-ff feature/my-thing
git push
```

Revert a whole feature: `git revert -m 1 <merge-hash>`.

## Undo cheatsheet

| Situation | Command |
|---|---|
| Fix last commit | `git commit --amend` |
| Discard file changes | `git restore <file>` |
| Unstage file | `git restore --staged <file>` |
| Shelve changes | `git stash` / `git stash pop` |
| Undo a published commit | `git revert <hash>` |
| Undo local commits, keep changes | `git reset --soft HEAD~1` |
| Recover lost commits | `git reflog`, then `git reset --hard <hash>` |

## Releases

Tag on `main`: `git tag v1.2.0 && git push --tags`.
