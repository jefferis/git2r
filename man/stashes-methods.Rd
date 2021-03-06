% Generated by roxygen2 (4.0.1): do not edit by hand
\docType{methods}
\name{stashes}
\alias{stashes}
\alias{stashes,git_repository-method}
\title{Stashes}
\usage{
stashes(object)

\S4method{stashes}{git_repository}(object)
}
\arguments{
\item{object}{The repository \code{object}.}
}
\value{
list of stashes in repository
}
\description{
Stashes
}
\examples{
\dontrun{
## Open an existing repository
repo <- repository("path/to/git2r")

## List stashes in repository
stashes(repo)
}
}
\keyword{methods}

