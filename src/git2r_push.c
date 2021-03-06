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

#include "git2r_error.h"
#include "git2r_push.h"
#include "git2r_repository.h"

/**
 * The invoked callback on each status entry
 *
 * @param ref
 * @param msg
 * @param data
 * @return 0
 */
static int git2r_push_status_foreach_cb(
    const char *ref,
    const char *msg,
    void *data)
{
    const char **msg_dst = (const char **)data;
    if (msg != NULL && *msg_dst == NULL)
        *msg_dst = msg;
    return 0;
}

/**
 * Push
 *
 * @param repo
 * @param name
 * @param refspec
 * @return R_NilValue
 */
SEXP git2r_push(SEXP repo, SEXP name, SEXP refspec)
{
    int err;
    const char *msg = NULL;
    git_push *push = NULL;
    git_remote *remote = NULL;
    git_repository *repository = NULL;

    if (git2r_error_check_string_arg(name)
        || git2r_error_check_string_arg(refspec))
        error("Invalid arguments to git2r_push");

    repository = git2r_repository_open(repo);
    if (!repository)
        error(git2r_err_invalid_repository);

    err = git_remote_load(&remote, repository, CHAR(STRING_ELT(name, 0)));
    if (err < 0)
        goto cleanup;

    err = git_push_new(&push, remote);
    if (err < 0)
        goto cleanup;

    err = git_push_add_refspec(push, CHAR(STRING_ELT(refspec, 0)));
    if (err < 0)
        goto cleanup;

    err = git_push_finish(push);
    if (err < 0)
        goto cleanup;

    err = git_push_unpack_ok(push);
    if (err < 0)
        goto cleanup;

    err = git_push_status_foreach(push, git2r_push_status_foreach_cb, &msg);
    if (err < 0)
        goto cleanup;
    if (msg != NULL) {
        err = -1;
        goto cleanup;
    }

    err = git_push_update_tips(push, NULL, NULL);
    if (err < 0)
        goto cleanup;

cleanup:
    if (push)
        git_push_free(push);

    if (remote)
        git_remote_disconnect(remote);

    if (remote)
        git_remote_free(remote);

    if (repository)
        git_repository_free(repository);

    if (err < 0) {
        if (NULL != msg)
            error("Error: %s\n", msg);
        else
            error("Error: %s\n", giterr_last()->message);
    }

    return R_NilValue;
}
