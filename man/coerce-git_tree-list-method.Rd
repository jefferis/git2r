% Generated by roxygen2 (4.0.1): do not edit by hand
\docType{methods}
\name{coerce-git_tree-list-method}
\alias{coerce-git_tree-list-method}
\title{Coerce entries in a git_tree to a list of entry objects}
\arguments{
\item{from}{The tree \code{object}}
}
\value{
list of entry objects
}
\description{
Coerce entries in a git_tree to a list of entry objects
}
\examples{
\dontrun{
## Open an existing repository
repo <- repository("path/to/git2r")

## Inspect size of each blob in tree
invisible(lapply(as(tree(commits(repo)[[1]]), "list"),
  function(obj) {
    if(is_blob(obj))
      summary(obj)
    NULL
  }))
}
}
\keyword{methods}

