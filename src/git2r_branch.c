/*
 *  git2r, R bindings to the libgit2 library.
 *  Copyright (C) 2013-2014 The git2r contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, version 2,
 *  as published by the Free Software Foundation.
 *
 *  git2r is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <Rdefines.h>
#include "git2.h"
#include "git2r_error.h"
#include "git2r_reference.h"
#include "git2r_repository.h"

/**
 * Count number of branches.
 *
 * @param repo S4 class git_repository
 * @param flags
 * @return
 */
static int git2r_branch_count(git_repository *repo, int flags, size_t *n)
{
    int err;
    git_branch_iterator *iter;
    git_branch_t type;
    git_reference *ref;

    *n = 0;

    err = git_branch_iterator_new(&iter, repo, flags);
    if (err < 0)
        return err;

    for (;;) {
        err = git_branch_next(&ref, &type, iter);
        if (err < 0)
            break;
        git_reference_free(ref);
        (*n)++;
    }

    git_branch_iterator_free(iter);

    if (GIT_ITEROVER != err)
        return err;
    return 0;
}

/**
 * List branches in a repository
 *
 * @param repo S4 class git_repository
 * @return VECXSP with S4 objects of class git_branch
 */
SEXP git2r_branch_list(SEXP repo, SEXP flags)
{
    SEXP list;
    int err = 0;
    const char* err_msg = NULL;
    git_branch_iterator *iter = NULL;
    size_t i = 0, n = 0;
    size_t protected = 0;
    git_repository *repository = NULL;

    repository = git2r_repository_open(repo);
    if (!repository)
        error(git2r_err_invalid_repository);

    /* Count number of branches before creating the list */
    err = git2r_branch_count(repository, INTEGER(flags)[0], &n);
    if (err < 0)
        goto cleanup;

    PROTECT(list = allocVector(VECSXP, n));
    protected++;

    err = git_branch_iterator_new(&iter, repository,  INTEGER(flags)[0]);
    if (err < 0)
        goto cleanup;

    for (;;) {
        SEXP branch;
        git_branch_t type;
        git_reference *ref;
        const char *refname;

        err = git_branch_next(&ref, &type, iter);
        if (err < 0) {
            if (GIT_ITEROVER == err) {
                err = 0;
                break;
            }
            goto cleanup;
        }

        PROTECT(branch = NEW_OBJECT(MAKE_CLASS("git_branch")));
        protected++;

        refname = git_reference_name(ref);
        git2r_reference_init(ref, branch);

        switch (type) {
        case GIT_BRANCH_LOCAL:
            break;
        case GIT_BRANCH_REMOTE: {
            git_buf buf = {0};
            git_remote *remote = NULL;

            err = git_branch_remote_name(&buf, repository, refname);
            if (err < 0)
                goto cleanup;
            SET_SLOT(branch, Rf_install("remote"), ScalarString(mkChar(buf.ptr)));

            err = git_remote_load(&remote, repository, buf.ptr);
            if (err < 0) {
                err = git_remote_create_anonymous(&remote, repository, buf.ptr, NULL);
                if (err < 0) {
                    git_buf_free(&buf);
                    goto cleanup;
                }
            }

            SET_SLOT(branch,
                     Rf_install("url"),
                     ScalarString(mkChar(git_remote_url(remote))));

            git_buf_free(&buf);
            git_remote_free(remote);
            break;
        }
        default:
            err = -1;
            err_msg = git2r_err_unexpected_type_of_branch;
            goto cleanup;
        }

        switch (git_branch_is_head(ref)) {
        case 0:
            SET_SLOT(branch, Rf_install("head"), ScalarLogical(0));
            break;
        case 1:
            SET_SLOT(branch, Rf_install("head"), ScalarLogical(1));
            break;
        default:
            err = -1;
            err_msg = git2r_err_unexpected_head_of_branch;
            goto cleanup;
        }

        git_reference_free(ref);
        SET_VECTOR_ELT(list, i, branch);
        UNPROTECT(1);
        protected--;
        i++;
    }

cleanup:
    if (iter)
        git_branch_iterator_free(iter);

    if (repository)
        git_repository_free(repository);

    if (protected)
        UNPROTECT(protected);

    if (err < 0) {
        if (err_msg)
            error(err_msg);
        else
            error("Error: %s\n", giterr_last()->message);
    }

    return list;
}
