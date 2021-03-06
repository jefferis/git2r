% Generated by roxygen2 (4.0.1): do not edit by hand
\docType{methods}
\name{hash}
\alias{hash}
\alias{hash,character-method}
\title{Determine the sha1 hex of a blob from a string}
\usage{
hash(data)

\S4method{hash}{character}(data)
}
\arguments{
\item{data}{The string vector to hash.}
}
\value{
A string vector with the sha1 hex for each string in data.
}
\description{
The blob is not written to the object database.
}
\examples{
\dontrun{
identical(hash(c("Hello, world!\\n",
                 "test content\\n")),
               c("af5626b4a114abcb82d63db7c8082c3c4756e51b",
                 "d670460b4b4aece5915caf5c68d12f560a9fe3e4"))
}
}
\keyword{methods}

