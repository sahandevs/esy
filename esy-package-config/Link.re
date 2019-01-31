open Sexplib0.Sexp_conv;

[@deriving (ord, sexp_of)]
type t = {
  path: DistPath.t,
  manifest: option(ManifestSpec.t),
  kind,
}
and kind =
  | LinkRegular
  | LinkDev;

module Parse = {
  include Parse;

  let withPrefix = (prefix, p) => string(prefix) *> p;

  let pathLike = (~requirePathSep, make) => {
    let make = path => {
      let path = Path.(normalizeAndRemoveEmptySeg(v(path)));
      let (path, manifest) =
        switch (ManifestSpec.ofString(Path.basename(path))) {
        | Ok(manifest) =>
          let path = Path.(remEmptySeg(parent(path)));
          (path, Some(manifest));
        | Error(_) => (path, None)
        };

      make(DistPath.ofPath(path), manifest);
    };

    let path =
      scan(false, (seenPathSep, c) => Some(seenPathSep || c == '/'));

    let%bind (path, seenPathSep) = path;
    if (!requirePathSep || seenPathSep) {
      return(make(path));
    } else {
      fail("not a path");
    };
  };

  let link = kind => {
    let make = (path, manifest) => {path, manifest, kind};

    pathLike(make);
  };

  let parser =
    withPrefix("link:", link(LinkRegular, ~requirePathSep=false))
    <|> withPrefix("link-dev:", link(LinkDev, ~requirePathSep=false));
};

let parser = Parse.parser;

let show' = (showPath, link) =>
  switch (link) {
  | {path, manifest: None, kind: LinkDev} =>
    Printf.sprintf("link-dev:%s", showPath(path))
  | {path, manifest: Some(manifest), kind: LinkDev} =>
    Printf.sprintf(
      "link-dev:%s/%s",
      showPath(path),
      ManifestSpec.show(manifest),
    )
  | {path, manifest: None, kind: LinkRegular} =>
    Printf.sprintf("link:%s", showPath(path))
  | {path, manifest: Some(manifest), kind: LinkRegular} =>
    Printf.sprintf(
      "link:%s/%s",
      showPath(path),
      ManifestSpec.show(manifest),
    )
  };

let show = show'(DistPath.show);
let showPretty = show'(DistPath.showPretty);
let pp = (fmt, link) => Fmt.string(fmt, show(link));
let ppPretty = (fmt, link) => Fmt.string(fmt, showPretty(link));

let of_yojson = json =>
  switch (json) {
  | `String(v) => Parse.parse(parser, v)
  | _ => Error("expected string")
  };
